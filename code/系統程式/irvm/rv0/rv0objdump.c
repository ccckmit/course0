#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

// Parse options
int opt_h = 0;
int opt_d = 0;

// RISC-V ABI Register names
const char* reg_names[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// RISC-V Floating-Point Register names
const char* freg_names[32] = {
    "ft0", "ft1", "ft2", "ft3", "ft4", "ft5", "ft6", "ft7",
    "fs0", "fs1", "fa0", "fa1", "fa2", "fa3", "fa4", "fa5",
    "fa6", "fa7", "fs2", "fs3", "fs4", "fs5", "fs6", "fs7",
    "fs8", "fs9", "fs10", "fs11", "ft12", "ft13", "ft14", "ft15"
};

// Helper to convert byte alignment to power of 2 for printing (like 2**2)
int get_align_pow(uint64_t align) {
    if (align <= 1) return 0;
    int pow = 0;
    while ((align & 1) == 0) {
        align >>= 1;
        pow++;
    }
    return pow;
}

// Basic RISC-V instruction decoder
void decode_riscv(uint32_t inst) {
    if (inst == 0x00000013) {
        printf("nop\n");
        return;
    }
    
    uint32_t opcode = inst & 0x7F;
    uint32_t rd     = (inst >> 7) & 0x1F;
    uint32_t funct3 = (inst >> 12) & 0x7;
    uint32_t rs1    = (inst >> 15) & 0x1F;
    uint32_t rs2    = (inst >> 20) & 0x1F;
    uint32_t funct7 = (inst >> 25) & 0x7F;
    
    // Immediate extraction (sign-extended)
    int32_t imm_i = ((int32_t)inst) >> 20; 
    
    switch (opcode) {
        case 0x13: // OP-IMM
            if (funct3 == 0) printf("addi\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else if (funct3 == 1) printf("slli\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i & 0x1F);
            else if (funct3 == 2) printf("slti\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else if (funct3 == 3) printf("sltiu\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else if (funct3 == 4) printf("xori\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else if (funct3 == 5 && (imm_i & 0x1000)) printf("srai\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i & 0x1F);
            else if (funct3 == 5) printf("srli\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i & 0x1F);
            else if (funct3 == 6) printf("ori\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else if (funct3 == 7) printf("andi\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else printf("op-imm\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            break;
        case 0x1B: // OP-IMM-32
            if (funct3 == 0) printf("addiw\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            else if (funct3 == 1) printf("slliw\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i & 0x1F);
            else if (funct3 == 5 && (imm_i & 0x1000)) printf("sraiw\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i & 0x1F);
            else if (funct3 == 5) printf("srliw\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i & 0x1F);
            else printf("op-imm-32\t%s, %s, %d\n", reg_names[rd], reg_names[rs1], imm_i);
            break;
        case 0x33: // OP
            if (funct3 == 0 && funct7 == 0) printf("add \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 0x20) printf("sub \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 1) printf("mul \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 2) printf("mulh\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 3) printf("mulhsu\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 4) printf("mulhu\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 5) printf("div \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 6) printf("divu\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 7) printf("rem \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 8) printf("remu\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 1 && funct7 == 0) printf("sll \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 2 && funct7 == 0) printf("slt \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 3 && funct7 == 0) printf("sltu\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 4 && funct7 == 0) printf("xor \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 5 && funct7 == 0) printf("srl \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 5 && funct7 == 0x20) printf("sra \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 6 && funct7 == 0) printf("or  \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 7 && funct7 == 0) printf("and \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else printf("op  \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            break;
        case 0x3B: // OP-32
            if (funct3 == 0 && funct7 == 0) printf("addw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 0x20) printf("subw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 1) printf("mulw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 5) printf("divw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 6) printf("divuw\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 7) printf("remw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 0 && funct7 == 8) printf("remuw\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 1 && funct7 == 0) printf("sllw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 5 && funct7 == 0) printf("srlw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else if (funct3 == 5 && funct7 == 0x20) printf("sraw \t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            else printf("op-32\t%s, %s, %s\n", reg_names[rd], reg_names[rs1], reg_names[rs2]);
            break;
        case 0x03: // LOAD
            if (funct3 == 0) printf("lb  \t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            else if (funct3 == 1) printf("lh  \t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            else if (funct3 == 2) printf("lw  \t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            else if (funct3 == 3) printf("ld  \t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            else if (funct3 == 4) printf("lbu \t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            else if (funct3 == 5) printf("lhu \t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            else printf("load\t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            break;
        case 0x23: // STORE
            {
                int32_t imm_s = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
                imm_s = (imm_s << 20) >> 20; // sign extension
                if (funct3 == 0) printf("sb  \t%s, %d(%s)\n", reg_names[rs2], imm_s, reg_names[rs1]);
                else if (funct3 == 1) printf("sh  \t%s, %d(%s)\n", reg_names[rs2], imm_s, reg_names[rs1]);
                else if (funct3 == 2) printf("sw  \t%s, %d(%s)\n", reg_names[rs2], imm_s, reg_names[rs1]);
                else if (funct3 == 3) printf("sd  \t%s, %d(%s)\n", reg_names[rs2], imm_s, reg_names[rs1]);
                else printf("store\t%s, %d(%s)\n", reg_names[rs2], imm_s, reg_names[rs1]);
            }
            break;
        case 0x63: // BRANCH
            {
                int32_t imm_b = (((inst >> 31) & 1) << 12) |
                                (((inst >> 7) & 1) << 11) |
                                (((inst >> 25) & 0x3F) << 5) |
                                (((inst >> 8) & 0xF) << 1);
                imm_b = (imm_b << 19) >> 19;
                printf("branch\t%s, %s, .+%d\n", reg_names[rs1], reg_names[rs2], imm_b);
            }
            break;
        case 0x6f: // JAL
            {
                int32_t imm_j = (((inst >> 31) & 1) << 20) |
                                (((inst >> 12) & 0xFF) << 12) |
                                (((inst >> 20) & 1) << 11) |
                                (((inst >> 21) & 0x3FF) << 1);
                imm_j = (imm_j << 11) >> 11;
                printf("jal \t%s, .+%d\n", reg_names[rd], imm_j);
            }
            break;
        case 0x67: // JALR
            printf("jalr\t%s, %d(%s)\n", reg_names[rd], imm_i, reg_names[rs1]);
            break;
        case 0x37: // LUI
            printf("lui \t%s, %d\n", reg_names[rd], (inst >> 12) & 0xFFFFF);
            break;
        case 0x17: // AUIPC
            printf("auipc\t%s, %d\n", reg_names[rd], (inst >> 12) & 0xFFFFF);
            break;
        // ==================== RV64A 原子指令 ====================
        case 0x2F: // AMO
            {
                uint32_t funct5 = (inst >> 27) & 0x1F;
                uint32_t funct3 = (inst >> 12) & 0x7;
                if (funct3 == 2) { // 32-bit
                    if (funct5 == 0x02) printf("lr.w\t%s, (%s)\n", reg_names[rd], reg_names[rs1]);
                    else if (funct5 == 0x03) printf("sc.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x00) printf("amoadd.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x01) printf("amoswap.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x04) printf("amoxor.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x03) printf("amoand.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x06) printf("amoor.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x05) printf("amomin.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x07) printf("amomax.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x09) printf("amominu.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x0A) printf("amomaxu.w\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else printf("amo32\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                } else if (funct3 == 3) { // 64-bit
                    if (funct5 == 0x02) printf("lr.d\t%s, (%s)\n", reg_names[rd], reg_names[rs1]);
                    else if (funct5 == 0x03) printf("sc.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x00) printf("amoadd.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x01) printf("amoswap.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x04) printf("amoxor.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x03) printf("amoand.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x06) printf("amoor.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x05) printf("amomin.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x07) printf("amomax.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x09) printf("amominu.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else if (funct5 == 0x0A) printf("amomaxu.d\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                    else printf("amo64\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                } else {
                    printf("amo\t%s, %s, (%s)\n", reg_names[rd], reg_names[rs2], reg_names[rs1]);
                }
            }
            break;
        // ==================== RV64F/RV64D 浮點數指令 ====================
        case 0x07: // LOAD-FP
            if (funct3 == 2) printf("flw \t%s, %d(%s)\n", freg_names[rd], imm_i, reg_names[rs1]);
            else if (funct3 == 3) printf("fld \t%s, %d(%s)\n", freg_names[rd], imm_i, reg_names[rs1]);
            else printf("load-fp\t%s, %d(%s)\n", freg_names[rd], imm_i, reg_names[rs1]);
            break;
        case 0x27: // STORE-FP
            {
                int32_t imm_s = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
                imm_s = (imm_s << 20) >> 20;
                if (funct3 == 2) printf("fsw \t%s, %d(%s)\n", freg_names[rs2], imm_s, reg_names[rs1]);
                else if (funct3 == 3) printf("fsd \t%s, %d(%s)\n", freg_names[rs2], imm_s, reg_names[rs1]);
                else printf("store-fp\t%s, %d(%s)\n", freg_names[rs2], imm_s, reg_names[rs1]);
            }
            break;
        case 0x53: // OP-FP (double precision)
            {
                int rs3 = (inst >> 27) & 0x1F;
                if (funct7 == 0x00) printf("fadd.d\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x04) printf("fsub.d\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x08) printf("fmul.d\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x0C) printf("fdiv.d\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x2C) printf("fsqrt.d\t%s, %s\n", freg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x20 && funct3 == 2) printf("feq.d \t%s, %s, %s\n", reg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x21 && funct3 == 2) printf("flt.d \t%s, %s, %s\n", reg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x22 && funct3 == 2) printf("fle.d \t%s, %s, %s\n", reg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x60 && rs2 == 0) printf("fcvt.w.d\t%s, %s\n", reg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x61 && rs2 == 0) printf("fcvt.wu.d\t%s, %s\n", reg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x68 && rs2 == 0) printf("fcvt.d.w\t%s, %s\n", freg_names[rd], reg_names[rs1]);
                else if (funct7 == 0x69 && rs2 == 0) printf("fcvt.d.wu\t%s, %s\n", freg_names[rd], reg_names[rs1]);
                else if (funct7 == 0x70 && rs2 == 0) printf("fmv.x.d\t%s, %s\n", reg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x78 && rs2 == 0) printf("fmv.d.x\t%s, %s\n", freg_names[rd], reg_names[rs1]);
                else if (funct7 == 0x20) printf("fcvt.d.s\t%s, %s\n", freg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x21) printf("fcvt.s.d\t%s, %s\n", freg_names[rd], freg_names[rs1]);
                else printf("op-fp\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
            }
            break;
        case 0x43: // OP-FP (single precision)
            {
                int rs3 = (inst >> 27) & 0x1F;
                if (funct7 == 0x00) printf("fadd.s\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x04) printf("fsub.s\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x08) printf("fmul.s\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x0C) printf("fdiv.s\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x2C) printf("fsqrt.s\t%s, %s\n", freg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x20 && funct3 == 2) printf("feq.s \t%s, %s, %s\n", reg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x21 && funct3 == 2) printf("flt.s \t%s, %s, %s\n", reg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x22 && funct3 == 2) printf("fle.s \t%s, %s, %s\n", reg_names[rd], freg_names[rs1], freg_names[rs2]);
                else if (funct7 == 0x60 && rs2 == 0) printf("fcvt.w.s\t%s, %s\n", reg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x61 && rs2 == 0) printf("fcvt.wu.s\t%s, %s\n", reg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x68 && rs2 == 0) printf("fcvt.s.w\t%s, %s\n", freg_names[rd], reg_names[rs1]);
                else if (funct7 == 0x69 && rs2 == 0) printf("fcvt.s.wu\t%s, %s\n", freg_names[rd], reg_names[rs1]);
                else if (funct7 == 0x70 && rs2 == 0) printf("fmv.x.w\t%s, %s\n", reg_names[rd], freg_names[rs1]);
                else if (funct7 == 0x78 && rs2 == 0) printf("fmv.w.x\t%s, %s\n", freg_names[rd], reg_names[rs1]);
                else printf("op-fp-s\t%s, %s, %s\n", freg_names[rd], freg_names[rs1], freg_names[rs2]);
            }
            break;
        default:
            printf(".word\t0x%08x\n", inst);
    }
}

// Macro to generate processing functions for both 32-bit and 64-bit ELF
#define PROCESS_ELF(bits) \
void process_elf##bits(uint8_t *map, size_t size) { \
    Elf##bits##_Ehdr *ehdr = (Elf##bits##_Ehdr *)map; \
    if (ehdr->e_shoff + ehdr->e_shentsize * ehdr->e_shnum > size) { \
        fprintf(stderr, "ELF section headers out of bounds\n"); \
        return; \
    } \
    Elf##bits##_Shdr *shdrs = (Elf##bits##_Shdr *)(map + ehdr->e_shoff); \
    char *shstrtab = (char *)(map + shdrs[ehdr->e_shstrndx].sh_offset); \
    \
    if (opt_h) { \
        printf("Sections:\n"); \
        printf("Idx Name                 Size      VMA       LMA       File off  Algn\n"); \
        for (int i = 0; i < ehdr->e_shnum; i++) { \
            char *name = shstrtab + shdrs[i].sh_name; \
            printf("%3d %-20s %08lx  %08lx  %08lx  %08lx  2**%d\n", \
                   i, name, (unsigned long)shdrs[i].sh_size, \
                   (unsigned long)shdrs[i].sh_addr, (unsigned long)shdrs[i].sh_addr, \
                   (unsigned long)shdrs[i].sh_offset, get_align_pow(shdrs[i].sh_addralign)); \
        } \
    } \
    \
    if (opt_d) { \
        Elf##bits##_Shdr *symtab_shdr = NULL; \
        char *symstrtab = NULL; \
        Elf##bits##_Sym *syms = NULL; \
        int num_syms = 0; \
        for (int i = 0; i < ehdr->e_shnum; i++) { \
            if (shdrs[i].sh_type == SHT_SYMTAB) { \
                symtab_shdr = &shdrs[i]; \
                symstrtab = (char *)(map + shdrs[symtab_shdr->sh_link].sh_offset); \
                syms = (Elf##bits##_Sym *)(map + symtab_shdr->sh_offset); \
                num_syms = symtab_shdr->sh_size / sizeof(Elf##bits##_Sym); \
                break; \
            } \
        } \
        for (int i = 0; i < ehdr->e_shnum; i++) { \
            if (shdrs[i].sh_flags & SHF_EXECINSTR) { \
                char *name = shstrtab + shdrs[i].sh_name; \
                printf("\nDisassembly of section %s:\n\n", name); \
                \
                uint8_t *code = map + shdrs[i].sh_offset; \
                int printed_section_name = 0; \
                for (uint64_t offset = 0; offset < shdrs[i].sh_size; offset += 4) { \
                    uint64_t addr = shdrs[i].sh_addr + offset; \
                    const char *sym_name = NULL; \
                    if (syms) { \
                        for (int s = 0; s < num_syms; s++) { \
                            if (syms[s].st_shndx == i && syms[s].st_value == addr) { \
                                int type = ELF##bits##_ST_TYPE(syms[s].st_info); \
                                if (type == STT_SECTION || type == STT_FILE) continue; \
                                if (syms[s].st_name != 0) { \
                                    const char *tname = symstrtab + syms[s].st_name; \
                                    if (tname[0] == '.' && tname[1] == 'L') continue; \
                                    sym_name = tname; \
                                    if (type == STT_FUNC) break; \
                                } \
                            } \
                        } \
                    } \
                    if (sym_name) { \
                        if (offset > 0) printf("\n"); \
                        if (bits == 64) \
                            printf("%016lx <%s>:\n", (unsigned long)addr, sym_name); \
                        else \
                            printf("%08lx <%s>:\n", (unsigned long)addr, sym_name); \
                        printed_section_name = 1; \
                    } else if (offset == 0 && !printed_section_name) { \
                        if (bits == 64) \
                            printf("%016lx <%s>:\n", (unsigned long)addr, name); \
                        else \
                            printf("%08lx <%s>:\n", (unsigned long)addr, name); \
                    } \
                    \
                    if (offset + 4 > shdrs[i].sh_size) break; \
                    uint32_t inst; \
                    memcpy(&inst, code + offset, sizeof(inst)); \
                    printf("%8lx:\t%08x \t", (unsigned long)offset, inst); \
                    decode_riscv(inst); \
                } \
            } \
        } \
    } \
}

PROCESS_ELF(32)
PROCESS_ELF(64)

int main(int argc, char **argv) {
    int opt;
    // Parse arguments
    while ((opt = getopt(argc, argv, "hd")) != -1) {
        switch (opt) {
            case 'h': opt_h = 1; break;
            case 'd': opt_d = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-h] [-d] <file>\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "Expected file argument after options\n");
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[optind];
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        perror("fstat");
        exit(EXIT_FAILURE);
    }

    if (st.st_size < EI_NIDENT) {
        fprintf(stderr, "File too small\n");
        exit(EXIT_FAILURE);
    }

    // Map the file into memory to easily parse it
    uint8_t *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Validate the ELF magic bytes (0x7F, 'E', 'L', 'F')
    if (map[EI_MAG0] != ELFMAG0 || map[EI_MAG1] != ELFMAG1 || 
        map[EI_MAG2] != ELFMAG2 || map[EI_MAG3] != ELFMAG3) {
        fprintf(stderr, "Not a valid ELF file\n");
        exit(EXIT_FAILURE);
    }

    // Direct the parser based on architecture class (32-bit vs 64-bit)
    int class = map[EI_CLASS];
    if (class == ELFCLASS32) {
        process_elf32(map, st.st_size);
    } else if (class == ELFCLASS64) {
        process_elf64(map, st.st_size);
    } else {
        fprintf(stderr, "Unsupported ELF class\n");
        exit(EXIT_FAILURE);
    }

    munmap(map, st.st_size);
    close(fd);
    return 0;
}