#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <elf.h>

#ifndef EM_RISCV
#define EM_RISCV 243
#endif

#define MAX_LINES 2000
#define MAX_LABELS 100
#define MAX_TEXT_SIZE 4096

// Tokenizer & structures
typedef struct {
    char name[64];
    uint32_t address;
} Label;

Label labels[MAX_LABELS];
int label_count = 0;

uint8_t text_section[MAX_TEXT_SIZE];
uint32_t text_size = 0;

// RISC-V Registers map
const char* reg_names[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// RISC-V Floating-Point Registers map
const char* freg_names[32] = {
    "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
    "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
    "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
    "fs8", "fs9", "fs10", "fs11", "ft12", "ft13", "ft14", "ft15"
};

int get_reg(const char *name) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(name, reg_names[i]) == 0) return i;
    }
    return 0; // fallback to zero
}

int get_freg(const char *name) {
    for (int i = 0; i < 32; i++) {
        if (strcmp(name, freg_names[i]) == 0) return i;
    }
    return 0; // fallback to ft0
}

uint32_t resolve_label(const char *name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i].name, name) == 0) return labels[i].address;
    }
    return 0;
}

// RISC-V Instruction Encoders
uint32_t enc_R(int op, int f3, int f7, int rd, int rs1, int rs2) {
    return (f7 << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | op;
}
uint32_t enc_I(int op, int f3, int rd, int rs1, int imm) {
    return ((imm & 0xFFF) << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | op;
}
uint32_t enc_S(int op, int f3, int rs1, int rs2, int imm) {
    return (((imm >> 5) & 0x7F) << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | ((imm & 0x1F) << 7) | op;
}
uint32_t enc_B(int op, int f3, int rs1, int rs2, int imm) {
    return (((imm >> 12) & 1) << 31) | (((imm >> 5) & 0x3F) << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | (((imm >> 1) & 0xF) << 8) | (((imm >> 11) & 1) << 7) | op;
}
uint32_t enc_J(int op, int rd, int imm) {
    return (((imm >> 20) & 1) << 31) | (((imm >> 1) & 0x3FF) << 21) | (((imm >> 11) & 1) << 20) | (((imm >> 12) & 0xFF) << 12) | (rd << 7) | op;
}
uint32_t enc_U(int op, int rd, int imm) {
    return ((imm & 0xFFFFF) << 12) | (rd << 7) | op;
}

// RISC-V Floating-Point Encoders
// FP R-type: opcode(7) | rs3(5) | rs2(5) | rs1(5) | f3(3) | rd(5) | opcode2(7)
uint32_t enc_R4(int op, int f3, int rd, int rs1, int rs2, int rs3) {
    return (rs3 << 27) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | op;
}
// FP R-type (2 operands): opcode(7) | f7 | rs2 | rs1 | f3 | rd | opcode2
uint32_t enc_FP_R(int op, int f7, int rd, int rs1, int rs2, int f3) {
    return (f7 << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | op;
}
// FP I-type: opcode(7) | imm[11:0] | rs1 | f3 | rd | opcode2
uint32_t enc_FP_I(int op, int f3, int rd, int rs1, int imm) {
    return ((imm & 0xFFF) << 20) | (rs1 << 15) | (f3 << 12) | (rd << 7) | op;
}
// FP S-type: opcode(7) | imm[11:5] | rs2 | rs1 | f3 | imm[4:0] | opcode2
uint32_t enc_FP_S(int op, int f3, int rs2, int rs1, int imm) {
    return (((imm >> 5) & 0x7F) << 25) | (rs2 << 20) | (rs1 << 15) | (f3 << 12) | ((imm & 0x1F) << 7) | op;
}

// Helper to strip commas and whitespace
void clean_operand(char *op) {
    char temp[32];
    char *p = op;
    int j = 0;
    while (*p) {
        if (*p != ',' && *p != '\n' && *p != '\r') {
            temp[j++] = *p;
        }
        p++;
    }
    temp[j] = '\0';
    strcpy(op, temp);
}

// Parse number (supports decimal and hex like 0x100)
int parse_imm(const char *s) {
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return (int)strtol(s, NULL, 16);
    }
    return atoi(s);
}

int get_rounding_mode(const char *rm) {
    if (strcmp(rm, "rne") == 0 || strcmp(rm, "") == 0) return 0; // round to nearest, ties to even
    if (strcmp(rm, "rtz") == 0) return 1; // round towards zero
    if (strcmp(rm, "rdn") == 0) return 2; // round down (towards -infinity)
    if (strcmp(rm, "rup") == 0) return 3; // round up (towards +infinity)
    if (strcmp(rm, "rmm") == 0) return 4; // round to nearest, ties to max magnitude
    return 0; // default
}

// Two-pass assemble function
void assemble(char lines[][256], int line_count) {
    // Pass 1: Compute addresses for labels
    uint32_t pc = 0;
    for (int i = 0; i < line_count; i++) {
        char *line = lines[i];
        if (strchr(line, ':')) {
            char name[64];
            sscanf(line, "%[^:]", name);
            strcpy(labels[label_count].name, name);
            labels[label_count].address = pc;
            label_count++;
            continue;
        }
        if (line[0] == '.' || strlen(line) < 2) continue; // Skip directives
        
        char mnemonic[32];
        sscanf(line, "%s", mnemonic);
        if (strcmp(mnemonic, "call") == 0) pc += 8; // auipc + jalr
        else if (strcmp(mnemonic, "li") == 0) pc += 4; // Simplified li (1 inst)
        else if (strcmp(mnemonic, "mv") == 0 || strcmp(mnemonic, "ret") == 0) pc += 4;
        else pc += 4;
    }

    // Pass 2: Emit Machine Code
    pc = 0;
    for (int i = 0; i < line_count; i++) {
        char *line = lines[i];
        if (strchr(line, ':') || line[0] == '.' || strlen(line) < 2) continue;
        
        char mnem[32] = {0}, op1[32] = {0}, op2[32] = {0}, op3[32] = {0};
        sscanf(line, "%s %s %s %s", mnem, op1, op2, op3);
        clean_operand(op1); clean_operand(op2); clean_operand(op3);
        
        uint32_t inst = 0;

        // Parse format like offset(reg) => e.g., 40(sp)
        int mem_imm = 0;
        char mem_reg[32] = {0};
        char offset_str[32] = {0};
        if (strchr(op2, '(')) {
            sscanf(op2, "%[^ (](%[^)])", offset_str, mem_reg);
            mem_imm = parse_imm(offset_str);
            strcpy(op2, mem_reg);
        }
        
        // Parse format like (reg) for AMO instructions => e.g., (t2)
        if (op3[0] == '(') {
            sscanf(op3, "(%[^)])", mem_reg);
            strcpy(op3, mem_reg);
        }
        
        // Clean up any remaining parentheses in all operands
        for (int i = 0; op1[i]; i++) if (op1[i] == '(' || op1[i] == ')') op1[i] = 0;
        for (int i = 0; op2[i]; i++) if (op2[i] == '(' || op2[i] == ')') op2[i] = 0;
        for (int i = 0; op3[i]; i++) if (op3[i] == '(' || op3[i] == ')') op3[i] = 0;

        if (strcmp(mnem, "addi") == 0) inst = enc_I(0x13, 0, get_reg(op1), get_reg(op2), parse_imm(op3));
        else if (strcmp(mnem, "addiw") == 0) inst = enc_I(0x1b, 0, get_reg(op1), get_reg(op2), parse_imm(op3));
        else if (strcmp(mnem, "slli") == 0) inst = enc_I(0x13, 1, get_reg(op1), get_reg(op2), parse_imm(op3));  // slli
        else if (strcmp(mnem, "srli") == 0) inst = enc_I(0x13, 5, get_reg(op1), get_reg(op2), parse_imm(op3));  // srli
        else if (strcmp(mnem, "srai") == 0) inst = enc_I(0x13, 5, get_reg(op1), get_reg(op2), parse_imm(op3) | 0x1000);  // srai
        else if (strcmp(mnem, "slliw") == 0) inst = enc_I(0x1b, 1, get_reg(op1), get_reg(op2), parse_imm(op3));  // slliw
        else if (strcmp(mnem, "srliw") == 0) inst = enc_I(0x1b, 5, get_reg(op1), get_reg(op2), parse_imm(op3));  // srliw
        else if (strcmp(mnem, "sraiw") == 0) inst = enc_I(0x1b, 5, get_reg(op1), get_reg(op2), parse_imm(op3) | 0x1000);  // sraiw
        else if (strcmp(mnem, "lw") == 0) inst = enc_I(0x03, 2, get_reg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "ld") == 0) inst = enc_I(0x03, 3, get_reg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "sw") == 0) inst = enc_S(0x23, 2, get_reg(op2), get_reg(op1), mem_imm);
        else if (strcmp(mnem, "sd") == 0) inst = enc_S(0x23, 3, get_reg(op2), get_reg(op1), mem_imm);
        else if (strcmp(mnem, "add") == 0) inst = enc_R(0x33, 0, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "sub") == 0) inst = enc_R(0x33, 0, 0x20, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "mul") == 0) inst = enc_R(0x33, 0, 1, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "sll") == 0) inst = enc_R(0x33, 1, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "srl") == 0) inst = enc_R(0x33, 5, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "sra") == 0) inst = enc_R(0x33, 5, 0x20, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "addw") == 0) inst = enc_R(0x3b, 0, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "subw") == 0) inst = enc_R(0x3b, 0, 0x20, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "sllw") == 0) inst = enc_R(0x3b, 1, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "srlw") == 0) inst = enc_R(0x3b, 5, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "sraw") == 0) inst = enc_R(0x3b, 5, 0x20, get_reg(op1), get_reg(op2), get_reg(op3));
        
        // RV64I 乘法指令
        else if (strcmp(mnem, "mul") == 0) inst = enc_R(0x33, 0, 1, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "mulh") == 0) inst = enc_R(0x33, 0, 2, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "mulhsu") == 0) inst = enc_R(0x33, 0, 3, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "mulhu") == 0) inst = enc_R(0x33, 0, 4, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "mulw") == 0) inst = enc_R(0x3b, 0, 1, get_reg(op1), get_reg(op2), get_reg(op3));
        
        // RV64I 除法指令
        else if (strcmp(mnem, "div") == 0) inst = enc_R(0x33, 0, 5, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "divu") == 0) inst = enc_R(0x33, 0, 6, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "divw") == 0) inst = enc_R(0x3b, 0, 5, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "divuw") == 0) inst = enc_R(0x3b, 0, 6, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "rem") == 0) inst = enc_R(0x33, 0, 7, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "remu") == 0) inst = enc_R(0x33, 0, 8, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "remw") == 0) inst = enc_R(0x3b, 0, 7, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "remuw") == 0) inst = enc_R(0x3b, 0, 8, get_reg(op1), get_reg(op2), get_reg(op3));
        
        // RV64I 邏輯指令
        else if (strcmp(mnem, "and") == 0) inst = enc_R(0x33, 7, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "or") == 0) inst = enc_R(0x33, 6, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "xor") == 0) inst = enc_R(0x33, 4, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        
        // RV64I 比較指令
        else if (strcmp(mnem, "slt") == 0) inst = enc_R(0x33, 2, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "sltu") == 0) inst = enc_R(0x33, 3, 0, get_reg(op1), get_reg(op2), get_reg(op3));
        else if (strcmp(mnem, "seqz") == 0) inst = enc_I(0x13, 3, get_reg(op1), get_reg(op2), 1); // sltiu rd, rs, 1
        else if (strcmp(mnem, "snez") == 0) inst = enc_R(0x33, 3, 0, get_reg(op1), 0, get_reg(op2)); // sltu rd, x0, rs
        else if (strcmp(mnem, "not") == 0) inst = enc_I(0x13, 4, get_reg(op1), get_reg(op2), -1); // xori rd, rs, -1
        else if (strcmp(mnem, "neg") == 0) inst = enc_R(0x33, 0, 0x20, get_reg(op1), 0, get_reg(op2)); // sub rd, x0, rs
        else if (strcmp(mnem, "nop") == 0) inst = enc_I(0x13, 0, 0, 0, 0); // addi x0, x0, 0
        else if (strcmp(mnem, "jr") == 0) inst = enc_I(0x67, 0, 0, get_reg(op1), 0); // jalr x0, 0(rs)
        
        // RV64I I-type 立即數版本
        else if (strcmp(mnem, "andi") == 0) inst = enc_I(0x13, 7, get_reg(op1), get_reg(op2), parse_imm(op3));
        else if (strcmp(mnem, "ori") == 0) inst = enc_I(0x13, 6, get_reg(op1), get_reg(op2), parse_imm(op3));
        else if (strcmp(mnem, "xori") == 0) inst = enc_I(0x13, 4, get_reg(op1), get_reg(op2), parse_imm(op3));
        else if (strcmp(mnem, "slti") == 0) inst = enc_I(0x13, 2, get_reg(op1), get_reg(op2), parse_imm(op3));
        else if (strcmp(mnem, "sltiu") == 0) inst = enc_I(0x13, 3, get_reg(op1), get_reg(op2), parse_imm(op3));
        
        // RV64I 載入指令 (lb, lbu, lh, lhu)
        else if (strcmp(mnem, "lb") == 0) inst = enc_I(0x03, 0, get_reg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "lbu") == 0) inst = enc_I(0x03, 4, get_reg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "lh") == 0) inst = enc_I(0x03, 1, get_reg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "lhu") == 0) inst = enc_I(0x03, 5, get_reg(op1), get_reg(op2), mem_imm);
        
        // RV64I 儲存指令 (sb, sh)
        else if (strcmp(mnem, "sb") == 0) inst = enc_S(0x23, 0, get_reg(op2), get_reg(op1), mem_imm);
        else if (strcmp(mnem, "sh") == 0) inst = enc_S(0x23, 1, get_reg(op2), get_reg(op1), mem_imm);
        
        // LUI 和 AUIPC
        else if (strcmp(mnem, "lui") == 0) inst = enc_U(0x37, get_reg(op1), parse_imm(op2));
        
        // ==================== RV64F 浮點數指令 ====================
        // FLW/FSD (載入/儲存)
        else if (strcmp(mnem, "flw") == 0) inst = enc_FP_I(0x07, 2, get_freg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "fsw") == 0) inst = enc_FP_S(0x27, 2, get_freg(op1), get_reg(op2), mem_imm);
        
        // FADD.S, FSUB.S, FMUL.S, FDIV.S
        else if (strcmp(mnem, "fadd.s") == 0) inst = enc_FP_R(0x43, 0, get_freg(op1), get_reg(op2), get_freg(op3), 0);
        else if (strcmp(mnem, "fsub.s") == 0) inst = enc_FP_R(0x43, 0, get_freg(op1), get_reg(op2), get_freg(op3), 4);
        else if (strcmp(mnem, "fmul.s") == 0) inst = enc_FP_R(0x43, 0, get_freg(op1), get_reg(op2), get_freg(op3), 8);
        else if (strcmp(mnem, "fdiv.s") == 0) inst = enc_FP_R(0x43, 0, get_freg(op1), get_reg(op2), get_freg(op3), 12);
        else if (strcmp(mnem, "fsqrt.s") == 0) inst = enc_FP_R(0x43, 0, get_freg(op1), 0, get_freg(op2), 0x2C);
        
        // 比較指令
        else if (strcmp(mnem, "feq.s") == 0) inst = enc_FP_R(0x53, 0x50, get_reg(op1), get_freg(op2), get_freg(op3), 2);
        else if (strcmp(mnem, "flt.s") == 0) inst = enc_FP_R(0x53, 0x50, get_reg(op1), get_freg(op2), get_freg(op3), 1);
        else if (strcmp(mnem, "fle.s") == 0) inst = enc_FP_R(0x53, 0x50, get_reg(op1), get_freg(op2), get_freg(op3), 0);
        
        // ==================== RV64D 浮點數指令 ====================
        // FLD/FSD (雙精度載入/儲存)
        else if (strcmp(mnem, "fld") == 0) inst = enc_FP_I(0x07, 3, get_freg(op1), get_reg(op2), mem_imm);
        else if (strcmp(mnem, "fsd") == 0) inst = enc_FP_S(0x27, 3, get_freg(op1), get_reg(op2), mem_imm);
        
        // FADD.D, FSUB.D, FMUL.D, FDIV.D
        else if (strcmp(mnem, "fadd.d") == 0) inst = enc_FP_R(0x47, 0, get_freg(op1), get_freg(op2), get_freg(op3), 0);
        else if (strcmp(mnem, "fsub.d") == 0) inst = enc_FP_R(0x47, 0, get_freg(op1), get_freg(op2), get_freg(op3), 4);
        else if (strcmp(mnem, "fmul.d") == 0) inst = enc_FP_R(0x47, 0, get_freg(op1), get_freg(op2), get_freg(op3), 8);
        else if (strcmp(mnem, "fdiv.d") == 0) inst = enc_FP_R(0x47, 0, get_freg(op1), get_freg(op2), get_freg(op3), 12);
        else if (strcmp(mnem, "fsqrt.d") == 0) inst = enc_FP_R(0x47, 0, get_freg(op1), 0, get_freg(op2), 0x2C);
        
        // 比較指令
        else if (strcmp(mnem, "feq.d") == 0) inst = enc_FP_R(0x53, 0x51, get_reg(op1), get_freg(op2), get_freg(op3), 2);
        else if (strcmp(mnem, "flt.d") == 0) inst = enc_FP_R(0x53, 0x51, get_reg(op1), get_freg(op2), get_freg(op3), 1);
        else if (strcmp(mnem, "fle.d") == 0) inst = enc_FP_R(0x53, 0x51, get_reg(op1), get_freg(op2), get_freg(op3), 0);
        
        // 轉換指令 - 修正參數順序並處理 rounding mode
        else if (strcmp(mnem, "fcvt.w.s") == 0) inst = enc_FP_R(0x43, 0x60, get_rounding_mode(op3), get_freg(op2), 0, get_reg(op1));  // FCVT.W.S
        else if (strcmp(mnem, "fcvt.wu.s") == 0) inst = enc_FP_R(0x43, 0x61, get_rounding_mode(op3), get_freg(op2), 0, get_reg(op1)); // FCVT.WU.S
        else if (strcmp(mnem, "fcvt.s.w") == 0) inst = enc_FP_R(0x43, 0x68, get_rounding_mode(op3), get_reg(op2), 0, get_freg(op1));  // FCVT.S.W
        else if (strcmp(mnem, "fcvt.s.wu") == 0) inst = enc_FP_R(0x43, 0x69, get_rounding_mode(op3), get_reg(op2), 0, get_freg(op1)); // FCVT.S.WU
        
        else if (strcmp(mnem, "fcvt.w.d") == 0) inst = enc_FP_R(0x53, 0x60, get_rounding_mode(op3), get_freg(op2), 0, get_reg(op1));  // FCVT.W.D
        else if (strcmp(mnem, "fcvt.wu.d") == 0) inst = enc_FP_R(0x53, 0x61, get_rounding_mode(op3), get_freg(op2), 0, get_reg(op1)); // FCVT.WU.D
        else if (strcmp(mnem, "fcvt.d.w") == 0) inst = enc_FP_R(0x53, 0x68, get_rounding_mode(op3), get_reg(op2), 0, get_freg(op1));  // FCVT.D.W
        else if (strcmp(mnem, "fcvt.d.wu") == 0) inst = enc_FP_R(0x53, 0x69, get_rounding_mode(op3), get_reg(op2), 0, get_freg(op1)); // FCVT.D.WU
        else if (strcmp(mnem, "fcvt.d.s") == 0) inst = enc_FP_R(0x53, 0x20, 0, get_freg(op2), 0, get_freg(op1)); // FCVT.D.S
        else if (strcmp(mnem, "fcvt.s.d") == 0) inst = enc_FP_R(0x53, 0x21, 0, get_freg(op2), 0, get_freg(op1)); // FCVT.S.D
        
        // 移動指令 - 修正參數順序
        else if (strcmp(mnem, "fmv.w.x") == 0) inst = enc_FP_R(0x43, 0x70, 0, get_reg(op2), 0, get_freg(op1));  // FMV.W.X (int to float)
        else if (strcmp(mnem, "fmv.x.w") == 0) inst = enc_FP_R(0x43, 0x78, 0, get_freg(op2), 0, get_reg(op1));  // FMV.X.W (float to int)
        
        else if (strcmp(mnem, "fmv.d.x") == 0) inst = enc_FP_R(0x53, 0x70, 0, get_reg(op2), 0, get_freg(op1));  // FMV.D.X (int to double)
        else if (strcmp(mnem, "fmv.x.d") == 0) inst = enc_FP_R(0x53, 0x78, 0, get_freg(op2), 0, get_reg(op1));  // FMV.X.D (double to int)
        
        // ==================== RV64A 原子指令 ====================
        // AMO 編碼格式: opcode(7) | funct5(5) | rs2(5) | rs1(5) | funct3(3) | rd(5) | opcode2(7)
        // opcode = 0x2F
        // 語法: amo<op>.d rd, rs2, (rs1) => op1=rd, op2=rs2, op3=rs1 (address in parentheses)
        // LR/SC: funct5=0x02 for LR, 0x03 for SC
        else if (strcmp(mnem, "lr.w") == 0) inst = (0x02 << 27) | (0 << 20) | (get_reg(op2) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F; // LR.W: funct5=2
        else if (strcmp(mnem, "lr.d") == 0) inst = (0x02 << 27) | (0 << 20) | (get_reg(op2) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F; // LR.D: funct5=2, funct3=3
        else if (strcmp(mnem, "sc.w") == 0) inst = (0x03 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F; // SC.W: funct5=3, funct3=2
        else if (strcmp(mnem, "sc.d") == 0) inst = (0x03 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F; // SC.D: funct5=3, funct3=3
        // 32-bit AMO (funct5 varies, funct3=2 for word)
        else if (strcmp(mnem, "amoadd.w") == 0) inst = (0x00 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoswap.w") == 0) inst = (0x01 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoxor.w") == 0) inst = (0x04 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoand.w") == 0) inst = (0x03 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoor.w") == 0) inst = (0x06 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amomin.w") == 0) inst = (0x05 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amomax.w") == 0) inst = (0x07 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amominu.w") == 0) inst = (0x09 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amomaxu.w") == 0) inst = (0x0A << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (2 << 12) | (get_reg(op1) << 7) | 0x2F;
        // 64-bit AMO (funct5=0x01 for AMOSWAP, funct3=3 for doubleword)
        else if (strcmp(mnem, "amoadd.d") == 0) inst = (0x00 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoswap.d") == 0) inst = (0x01 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoxor.d") == 0) inst = (0x04 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoand.d") == 0) inst = (0x03 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amoor.d") == 0) inst = (0x06 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amomin.d") == 0) inst = (0x05 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amomax.d") == 0) inst = (0x07 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amominu.d") == 0) inst = (0x09 << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        else if (strcmp(mnem, "amomaxu.d") == 0) inst = (0x0A << 27) | (get_reg(op2) << 20) | (get_reg(op3) << 15) | (3 << 12) | (get_reg(op1) << 7) | 0x2F;
        
        // ... 前面的 addi, lw, sw, mul 等等保留 ...
        else if (strcmp(mnem, "beq") == 0) inst = enc_B(0x63, 0, get_reg(op1), get_reg(op2), resolve_label(op3) - pc);
        else if (strcmp(mnem, "bne") == 0) inst = enc_B(0x63, 1, get_reg(op1), get_reg(op2), resolve_label(op3) - pc);
        else if (strcmp(mnem, "blt") == 0) inst = enc_B(0x63, 4, get_reg(op1), get_reg(op2), resolve_label(op3) - pc);
        else if (strcmp(mnem, "bge") == 0) inst = enc_B(0x63, 5, get_reg(op1), get_reg(op2), resolve_label(op3) - pc);
        
        // 支援編譯器常用的 Pseudo-instructions (偽指令)
        else if (strcmp(mnem, "ble") == 0) inst = enc_B(0x63, 5, get_reg(op2), get_reg(op1), resolve_label(op3) - pc); // ble 是顛倒的 bge
        else if (strcmp(mnem, "bgt") == 0) inst = enc_B(0x63, 4, get_reg(op2), get_reg(op1), resolve_label(op3) - pc); // bgt 是顛倒的 blt
        else if (strcmp(mnem, "beqz") == 0) inst = enc_B(0x63, 0, get_reg(op1), 0, resolve_label(op2) - pc);
        else if (strcmp(mnem, "bnez") == 0) inst = enc_B(0x63, 1, get_reg(op1), 0, resolve_label(op2) - pc);
        else if (strcmp(mnem, "sext.w")== 0) inst = enc_I(0x1B, 0, get_reg(op1), get_reg(op2), 0); // int 常見的 sign-extend
        // ... 下面保持原樣 ...
        else if (strcmp(mnem, "j") == 0) {
            int offset = resolve_label(op1) - pc;
            inst = enc_J(0x6f, 0, offset); // jal zero, offset
        }
        // Pseudo-instructions handling
        else if (strcmp(mnem, "li") == 0) inst = enc_I(0x13, 0, get_reg(op1), 0, parse_imm(op2)); // addi rd, zero, imm
        else if (strcmp(mnem, "mv") == 0) inst = enc_I(0x13, 0, get_reg(op1), get_reg(op2), 0); // addi rd, rs, 0
        else if (strcmp(mnem, "ret") == 0) inst = enc_I(0x67, 0, 0, get_reg("ra"), 0); // jalr zero, 0(ra)
        else if (strcmp(mnem, "call") == 0) {
            // call label -> auipc ra, hi20 ; jalr ra, lo12(ra)
            int offset = resolve_label(op1) - pc;
            int hi20 = (offset + 0x800) >> 12;
            int lo12 = offset - (hi20 << 12);
            uint32_t auipc = enc_U(0x17, get_reg("ra"), hi20);
            uint32_t jalr = enc_I(0x67, 0, get_reg("ra"), get_reg("ra"), lo12);
            memcpy(&text_section[pc], &auipc, 4);
            pc += 4;
            inst = jalr;
        }

        if (inst != 0) {
            memcpy(&text_section[pc], &inst, 4);
            pc += 4;
        }
    }
    text_size = pc;
}

// Function to write a basic ELF file containing our .text code
void write_elf(const char *filename) {
    FILE *f = fopen(filename, "wb");
    
    // Construct .strtab (String Table for Symbols)
    char strtab[4096];
    memset(strtab, 0, sizeof(strtab));
    int strtab_size = 1; // index 0 is always null byte
    
    // Construct .symtab (Symbol Table)
    Elf64_Sym syms[MAX_LABELS + 1];
    memset(syms, 0, sizeof(syms));
    int sym_count = 1; // index 0 is undefined symbol
    
    for (int i = 0; i < label_count; i++) {
        // Only emit symbols that don't start with '.' (ignore local compiler labels)
        if (labels[i].name[0] == '.') continue;
        
        int name_offset = strtab_size;
        strcpy(strtab + strtab_size, labels[i].name);
        strtab_size += strlen(labels[i].name) + 1;
        
        syms[sym_count].st_name = name_offset;
        syms[sym_count].st_info = ELF64_ST_INFO(STB_GLOBAL, STT_FUNC);
        syms[sym_count].st_shndx = 1; // .text section index
        syms[sym_count].st_value = labels[i].address;
        syms[sym_count].st_size = 0; // Don't care for now
        sym_count++;
    }
    
    int symtab_size = sym_count * sizeof(Elf64_Sym);
    
    Elf64_Ehdr ehdr;
    memset(&ehdr, 0, sizeof(ehdr));
    ehdr.e_ident[EI_MAG0] = ELFMAG0; ehdr.e_ident[EI_MAG1] = ELFMAG1;
    ehdr.e_ident[EI_MAG2] = ELFMAG2; ehdr.e_ident[EI_MAG3] = ELFMAG3;
    ehdr.e_ident[EI_CLASS] = ELFCLASS64;
    ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
    ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    ehdr.e_type = ET_REL;
    ehdr.e_machine = EM_RISCV;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_ehsize = sizeof(Elf64_Ehdr);
    ehdr.e_shentsize = sizeof(Elf64_Shdr);
    ehdr.e_shnum = 5; // Null, .text, .shstrtab, .symtab, .strtab
    ehdr.e_shstrndx = 2;

    const char shstrtab[] = "\0.text\0.shstrtab\0.symtab\0.strtab\0";
    
    Elf64_Shdr shdrs[5];
    memset(shdrs, 0, sizeof(shdrs));
    
    // .text section
    shdrs[1].sh_name = 1; // ".text"
    shdrs[1].sh_type = SHT_PROGBITS;
    shdrs[1].sh_flags = SHF_ALLOC | SHF_EXECINSTR;
    shdrs[1].sh_offset = sizeof(Elf64_Ehdr);
    shdrs[1].sh_size = text_size;
    shdrs[1].sh_addralign = 4;
    
    // .shstrtab section
    shdrs[2].sh_name = 7; // ".shstrtab"
    shdrs[2].sh_type = SHT_STRTAB;
    shdrs[2].sh_offset = sizeof(Elf64_Ehdr) + text_size;
    shdrs[2].sh_size = sizeof(shstrtab);
    shdrs[2].sh_addralign = 1;

    // .symtab section
    shdrs[3].sh_name = 17; // ".symtab"
    shdrs[3].sh_type = SHT_SYMTAB;
    shdrs[3].sh_link = 4; // points to .strtab
    shdrs[3].sh_info = sym_count; // one greater than the symbol table index of the last local symbol
    shdrs[3].sh_entsize = sizeof(Elf64_Sym);
    shdrs[3].sh_offset = shdrs[2].sh_offset + shdrs[2].sh_size;
    shdrs[3].sh_size = symtab_size;
    shdrs[3].sh_addralign = 8;
    
    // .strtab section
    shdrs[4].sh_name = 25; // ".strtab"
    shdrs[4].sh_type = SHT_STRTAB;
    shdrs[4].sh_offset = shdrs[3].sh_offset + shdrs[3].sh_size;
    shdrs[4].sh_size = strtab_size;
    shdrs[4].sh_addralign = 1;

    ehdr.e_shoff = shdrs[4].sh_offset + shdrs[4].sh_size;

    fwrite(&ehdr, 1, sizeof(ehdr), f);
    fwrite(text_section, 1, text_size, f);
    fwrite(shstrtab, 1, sizeof(shstrtab), f);
    fwrite(syms, 1, symtab_size, f);
    fwrite(strtab, 1, strtab_size, f);
    fwrite(shdrs, 1, sizeof(shdrs), f);
    
    fclose(f);
}

int main(int argc, char **argv) {
    if (argc == 4 && strcmp(argv[2], "-o")==1) {
        printf("Usage: %s <input.s> -o <output.o>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) return 1;

    static char lines[MAX_LINES][256];
    int line_count = 0;

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), f)) {
        // Strip comments and leading spaces
        char *p = buffer;
        while (isspace(*p)) p++;
        char *hash = strchr(p, '#');
        if (hash) *hash = '\0';
        if (strlen(p) > 0) {
            strcpy(lines[line_count++], p);
        }
    }
    fclose(f);

    assemble(lines, line_count);
    write_elf(argv[3]);
    printf("Successfully assembled %s into %s\n", argv[1], argv[3]);

    return 0;
}
