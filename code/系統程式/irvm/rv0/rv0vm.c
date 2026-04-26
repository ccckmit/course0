#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <math.h>

#ifndef EM_RISCV
#define EM_RISCV 243
#endif

#ifndef R_RISCV_JAL
#define R_RISCV_JAL 17
#endif
#ifndef R_RISCV_CALL
#define R_RISCV_CALL 18
#endif
#ifndef R_RISCV_CALL_PLT
#define R_RISCV_CALL_PLT 19
#endif
#ifndef R_RISCV_RELAX
#define R_RISCV_RELAX 51
#endif
#ifndef R_RISCV_HI20
#define R_RISCV_HI20 2
#endif
#ifndef R_RISCV_LO12_I
#define R_RISCV_LO12_I 3
#endif
#ifndef R_RISCV_LO12_S
#define R_RISCV_LO12_S 4
#endif

#define RAM_SIZE (1024 * 1024) // 1MB 虛擬記憶體

uint8_t RAM[RAM_SIZE];
int64_t X[32] = {0}; // 32 個 64-bit 暫存器
double F[32] = {0};  // 32 個 64-bit 浮點暫存器 (可存放 double 或 float)
uint64_t PC = 0;

const char* reg_names[32] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void print_registers() {
    printf("\n--- Execution Halted. Register State ---\n");
    for (int i = 0; i < 32; i++) {
        printf("%-4s: %-10lld ", reg_names[i], (long long)X[i]);
        if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("----------------------------------------\n");
}

int main(int argc, char **argv) {
    // 預設 entry point為 0xFFFFFFFFFFFFFFFF (-1)，表示需要從 ELF 讀取
    uint64_t entry_point = 0xFFFFFFFFFFFFFFFF; 

    int opt;
    while ((opt = getopt(argc, argv, "e:")) != -1) {
        if (opt == 'e') entry_point = strtol(optarg, NULL, 0);
    }

    if (optind >= argc) {
        fprintf(stderr, "Usage: %s [-e entry_pc] <file.o>\n", argv[0]);
        return 1;
    }

    int fd = open(argv[optind], O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    struct stat st;
    fstat(fd, &st);
    uint8_t *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    // 簡單的 ELF 讀取，找出 .text 區段並載入記憶體 0x0 處
    Elf64_Ehdr *ehdr = (Elf64_Ehdr*)map;
    Elf64_Shdr *shdrs = (Elf64_Shdr*)(map + ehdr->e_shoff);
    char *shstrtab = (char*)(map + shdrs[ehdr->e_shstrndx].sh_offset);
    
    // 如果沒有指定 entry point，先保留，等符號表載入後再決定
    if (entry_point == 0xFFFFFFFFFFFFFFFF) {
        entry_point = 0;
    }
    
    int rela_text_idx = -1;
    int symtab_idx = -1;
    int strtab_idx = -1;
    uint64_t rodata_addr = 0x10000000; // Load .rodata at fixed address

    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char *name = shstrtab + shdrs[i].sh_name;
        if (strcmp(name, ".text") == 0) {
            memcpy(RAM, map + shdrs[i].sh_offset, shdrs[i].sh_size);
        } else if (strcmp(name, ".rodata") == 0) {
            memcpy(RAM + rodata_addr, map + shdrs[i].sh_offset, shdrs[i].sh_size);
        } else if (strcmp(name, ".rela.text") == 0) {
            rela_text_idx = i;
        } else if (shdrs[i].sh_type == SHT_SYMTAB) {
            symtab_idx = i;
            strtab_idx = shdrs[i].sh_link;
        }
    }

    // 若 entry 尚未指定，嘗試從 ELF entry 或符號表找到 main
    fprintf(stderr, "DEBUG: entry_point initially = %llu, elf_entry = %llu\n", (unsigned long long)entry_point, (unsigned long long)ehdr->e_entry);
    if (entry_point == 0) {
        uint64_t elf_entry = ehdr->e_entry;
        fprintf(stderr, "DEBUG: elf_entry = %llu, symtab_idx=%d, strtab_idx=%d\n", (unsigned long long)elf_entry, symtab_idx, strtab_idx);
        if (elf_entry != 0) {
            entry_point = elf_entry;
            fprintf(stderr, "DEBUG: using elf_entry, entry_point now = %llu\n", (unsigned long long)entry_point);
        } else if (symtab_idx != -1 && strtab_idx != -1) {
            Elf64_Sym *syms = (Elf64_Sym*)(map + shdrs[symtab_idx].sh_offset);
            int sym_count = shdrs[symtab_idx].sh_size / sizeof(Elf64_Sym);
            const char *strtab = (const char*)(map + shdrs[strtab_idx].sh_offset);
            fprintf(stderr, "DEBUG: sym_count = %d\n", sym_count);
            for (int i = 0; i < sym_count; i++) {
                const char *name = strtab + syms[i].st_name;
                fprintf(stderr, "DEBUG: symbol %d: name='%s', st_value=%llu\n", i, name ? name : "(null)", (unsigned long long)syms[i].st_value);
                if (name && strcmp(name, "main") == 0) {
                    entry_point = syms[i].st_value;
                    break;
                }
            }
        }
        // 預設設為 0x0
        if (entry_point == 0) {
            entry_point = 0x0;
        }
    }

    // 當有提供 Relocation Table 與 Symbol Table 時，我們把它們套用至記憶體上的 .text
    if (rela_text_idx != -1 && symtab_idx != -1) {
        Elf64_Rela *relas = (Elf64_Rela*)(map + shdrs[rela_text_idx].sh_offset);
        int num_relas = shdrs[rela_text_idx].sh_size / sizeof(Elf64_Rela);
        Elf64_Sym *syms = (Elf64_Sym*)(map + shdrs[symtab_idx].sh_offset);

        for (int i = 0; i < num_relas; i++) {
            uint32_t r_type = ELF64_R_TYPE(relas[i].r_info);
            uint32_t r_sym  = ELF64_R_SYM(relas[i].r_info);
            uint64_t offset = relas[i].r_offset; // relative to .text
            int64_t addend  = relas[i].r_addend;
            
            // 由於只執行單一 .o 檔案，假定所有目標都相對於 0x0
            uint64_t sym_val = syms[r_sym].st_value;
            
            if (r_type == R_RISCV_CALL || r_type == R_RISCV_CALL_PLT) {
                int64_t diff = (sym_val + addend) - offset;
                uint32_t *inst_auipc = (uint32_t*)(RAM + offset);
                uint32_t *inst_jalr  = (uint32_t*)(RAM + offset + 4);
                int32_t hi20 = (diff + 0x800) & 0xFFFFF000;
                int32_t lo12 = diff - hi20;
                *inst_auipc = (*inst_auipc & 0x00000FFF) | hi20;
                *inst_jalr  = (*inst_jalr  & 0x000FFFFF) | ((lo12 & 0xFFF) << 20);
            } else if (r_type == R_RISCV_JAL) {
                int64_t diff = (sym_val + addend) - offset;
                uint32_t *inst_jal = (uint32_t*)(RAM + offset);
                uint32_t imm20 = ((diff >> 20) & 1) << 31;
                uint32_t imm10_1 = ((diff >> 1) & 0x3FF) << 21;
                uint32_t imm11 = ((diff >> 11) & 1) << 20;
                uint32_t imm19_12 = ((diff >> 12) & 0xFF) << 12;
                *inst_jal = (*inst_jal & 0x00000FFF) | imm20 | imm10_1 | imm11 | imm19_12;
            } else if (r_type == 2) { // R_RISCV_HI20 (LUI)
                uint32_t *inst_lui = (uint32_t*)(RAM + offset);
                // Set the upper 20 bits to point to .rodata
                *inst_lui = (*inst_lui & 0xFFF) | ((rodata_addr + sym_val + addend) & 0xFFFFF000);
            } else if (r_type == 3) { // R_RISCV_LO12_I (load from LUI address)
                // The lui has already been patched, we just need to add the offset
                // This is handled by the instruction itself
            }
        }
    }
    munmap(map, st.st_size);
    close(fd);

    // 初始化虛擬機狀態
    PC = entry_point;
    X[2] = RAM_SIZE;      // sp: 堆疊指標設在記憶體最底部 (1MB 處)
    X[1] = 0xFFFFFFFE;    // ra: 當 main 結束執行 ret (jalr ra) 時跳至此，作為結束標記

    printf("Starting VM at PC = 0x%llx\n", (unsigned long long)PC);

    int steps = 0;
    // 執行迴圈：直到跳回我們設定的 MAGIC EXIT ADDRESS (0xFFFFFFFE) 為止
    while (steps < 100000) {
        X[0] = 0; // x0 永遠為 0

        // 檢查是否為結束條件（避免嘗試讀取無效記憶體）
        if (PC == 0xFFFFFFFE) {
            printf("Program exited successfully! a0 = %lld\n", (long long)X[10]);
            break;
        }

        if (PC >= RAM_SIZE || PC < 0) {
            printf("Exception: PC out of bounds (0x%llx)\n", (unsigned long long)PC);
            break;
        }

        uint32_t inst = *(uint32_t*)(RAM + PC);
        uint32_t opcode = inst & 0x7F;
        uint32_t rd     = (inst >> 7) & 0x1F;
        uint32_t f3     = (inst >> 12) & 0x7;
        uint32_t rs1    = (inst >> 15) & 0x1F;
        uint32_t rs2    = (inst >> 20) & 0x1F;
        uint32_t f7     = (inst >> 25) & 0x7F;
        
        // 提取各種格式的 Immediate (並做 32-bit Sign-Extension)
        int32_t imm_i = ((int32_t)inst) >> 20;
        
        uint64_t next_pc = PC + 4;
        
        switch (opcode) {
            case 0x13: // OP-IMM (addi, andi, ori, xori, slti, sltiu, slli)
                if (f3 == 0) X[rd] = X[rs1] + imm_i; // addi
                else if (f3 == 7) X[rd] = X[rs1] & imm_i; // andi
                else if (f3 == 6) X[rd] = X[rs1] | imm_i; // ori
                else if (f3 == 4) X[rd] = X[rs1] ^ imm_i; // xori
                else if (f3 == 2) X[rd] = ((int64_t)X[rs1] < (int64_t)imm_i) ? 1 : 0; // slti
                else if (f3 == 3) X[rd] = ((uint64_t)X[rs1] < (uint64_t)(uint32_t)imm_i) ? 1 : 0; // sltiu
                else if (f3 == 1) {
                    uint32_t shamt = (inst >> 20) & 0x3F;
                    X[rd] = X[rs1] << shamt;
                }
                break;
            case 0x1B: // OP-IMM-32 (addiw, slli, srli, srai)
                {
                    uint32_t shamt = (inst >> 20) & 0x3F; // 6-bit shift amount for RV64
                    if (f3 == 0) X[rd] = (int32_t)(X[rs1] + imm_i);       // addiw
                    else if (f3 == 1) X[rd] = X[rs1] << shamt; // SLLI (for RV64, handles both 32 and 64-bit)
                    else if (f3 == 5 && (imm_i & 0x1000)) X[rd] = (int32_t)X[rs1] >> (shamt & 0x1F); // sraiw
                    else if (f3 == 5) X[rd] = (uint32_t)(X[rs1] >> (shamt & 0x1F)); // srliw
                }
                break;
            case 0x33: // OP (add, mul, sub, sll, srl, sra, and, or, xor, slt, sltu, div, rem, mulh, mulhsu, mulhu)
                if (f3 == 0 && f7 == 0) X[rd] = X[rs1] + X[rs2];       // add
                else if (f3 == 0 && f7 == 1) X[rd] = X[rs1] * X[rs2]; // mul
                else if (f3 == 0 && f7 == 0x20) X[rd] = X[rs1] - X[rs2]; // sub
                else if (f3 == 1 && f7 == 0) X[rd] = X[rs1] << (X[rs2] & 0x3F); // sll
                else if (f3 == 5 && f7 == 0) X[rd] = X[rs1] >> (X[rs2] & 0x3F); // srl
                else if (f3 == 5 && f7 == 0x20) X[rd] = (int64_t)X[rs1] >> (X[rs2] & 0x3F); // sra
                else if (f3 == 7 && f7 == 0) X[rd] = X[rs1] & X[rs2]; // and
                else if (f3 == 6 && f7 == 0) X[rd] = X[rs1] | X[rs2]; // or
                else if (f3 == 4 && f7 == 0) X[rd] = X[rs1] ^ X[rs2]; // xor
                else if (f3 == 2 && f7 == 0) X[rd] = ((int64_t)X[rs1] < (int64_t)X[rs2]) ? 1 : 0; // slt
                else if (f3 == 3 && f7 == 0) X[rd] = ((uint64_t)X[rs1] < (uint64_t)X[rs2]) ? 1 : 0; // sltu
                else if (f3 == 0 && f7 == 5) X[rd] = (X[rs2] != 0) ? (X[rs1] / X[rs2]) : -1; // div
                else if (f3 == 0 && f7 == 6) X[rd] = (X[rs2] != 0) ? (X[rs1] / X[rs2]) : 0xFFFFFFFFFFFFFFFF; // divu
                else if (f3 == 0 && f7 == 7) X[rd] = (X[rs2] != 0) ? (X[rs1] % X[rs2]) : X[rs1]; // rem
                else if (f3 == 0 && f7 == 8) X[rd] = (X[rs2] != 0) ? (X[rs1] % X[rs2]) : X[rs1]; // remu
                else if (f3 == 0 && f7 == 2) { // mulh (signed * signed)
                    __int128 result = (__int128)(int64_t)X[rs1] * (__int128)(int64_t)X[rs2];
                    X[rd] = (int64_t)(result >> 64);
                } else if (f3 == 0 && f7 == 3) { // mulhsu (signed * unsigned)
                    __int128 result = (__int128)(int64_t)X[rs1] * (__int128)X[rs2];
                    X[rd] = (int64_t)(result >> 64);
                } else if (f3 == 0 && f7 == 4) { // mulhu (unsigned * unsigned)
                    __uint128_t result = (__uint128_t)X[rs1] * (__uint128_t)X[rs2];
                    X[rd] = (int64_t)(result >> 64);
                }
                break;
            case 0x3B: // OP-32 (addw, subw, mulw, divw, divuw, remw, remuw, sllw, srlw, sraw)
                if (f3 == 0 && f7 == 0) X[rd] = (int32_t)(X[rs1] + X[rs2]);       // addw
                else if (f3 == 0 && f7 == 0x20) X[rd] = (int32_t)(X[rs1] - X[rs2]); // subw
                else if (f3 == 0 && f7 == 1) X[rd] = (int32_t)(X[rs1] * X[rs2]); // mulw
                else if (f3 == 1 && f7 == 0) X[rd] = (int32_t)(X[rs1] << (X[rs2] & 0x1F)); // sllw
                else if (f3 == 5 && f7 == 0) X[rd] = (uint32_t)(X[rs1] >> (X[rs2] & 0x1F)); // srlw
                else if (f3 == 5 && f7 == 0x20) X[rd] = (int32_t)X[rs1] >> (X[rs2] & 0x1F); // sraw
                else if (f3 == 0 && f7 == 5) { // divw
                    int32_t a = (int32_t)X[rs1];
                    int32_t b = (int32_t)X[rs2];
                    X[rd] = (b != 0) ? (a / b) : -1;
                } else if (f3 == 0 && f7 == 6) { // divuw
                    uint32_t a = (uint32_t)X[rs1];
                    uint32_t b = (uint32_t)X[rs2];
                    X[rd] = (b != 0) ? (a / b) : 0xFFFFFFFF;
                } else if (f3 == 0 && f7 == 7) { // remw
                    int32_t a = (int32_t)X[rs1];
                    int32_t b = (int32_t)X[rs2];
                    X[rd] = (b != 0) ? (a % b) : a;
                } else if (f3 == 0 && f7 == 8) { // remuw
                    uint32_t a = (uint32_t)X[rs1];
                    uint32_t b = (uint32_t)X[rs2];
                    X[rd] = (b != 0) ? (a % b) : a;
                }
                break;
            case 0x67: // JALR (ret)
                PC = (X[rs1] + imm_i) & ~1ULL;
                X[rd] = next_pc;
                continue;
            case 0x6F: // JAL (j)
                {
                    int32_t imm_j = (((inst >> 31) & 1) << 20) | (((inst >> 12) & 0xFF) << 12) |
                                    (((inst >> 20) & 1) << 11) | (((inst >> 21) & 0x3FF) << 1);
                    imm_j = (imm_j << 11) >> 11;
                    PC = PC + imm_j;
                    X[rd] = next_pc;
                    continue;
                }
case 0x63: // BRANCH 完整版
                {
                    int32_t imm_b = (((inst >> 31) & 1) << 12) | (((inst >> 7) & 1) << 11) |
                                    (((inst >> 25) & 0x3F) << 5) | (((inst >> 8) & 0xF) << 1);
                    imm_b = (imm_b << 19) >> 19;
                    
                    int take = 0;
                    if (f3 == 0) take = (X[rs1] == X[rs2]);                       // beq
                    else if (f3 == 1) take = (X[rs1] != X[rs2]);                  // bne
                    else if (f3 == 4) take = ((int64_t)X[rs1] < (int64_t)X[rs2]); // blt
                    else if (f3 == 5) take = ((int64_t)X[rs1] >= (int64_t)X[rs2]);// bge
                    else if (f3 == 6) take = ((uint64_t)X[rs1] < (uint64_t)X[rs2]);// bltu
                    else if (f3 == 7) take = ((uint64_t)X[rs1] >= (uint64_t)X[rs2]);// bgeu
                    
                    if (take) {
                        PC = PC + imm_b;
                        continue;
                    }
                }
                break;
            case 0x17: // AUIPC (call)
                X[rd] = PC + (int32_t)(inst & 0xFFFFF000);
                break;
            case 0x37: // LUI
                X[rd] = (int32_t)(inst & 0xFFFFF000);
                break;
            case 0x03: // LOAD (lw, ld, lb, lbu, lh, lhu)
                {
                    uint64_t addr = X[rs1] + imm_i;
                    if (addr >= RAM_SIZE) { printf("Memory Read Fault\n"); goto end; }
                    if (f3 == 2) X[rd] = (int32_t)(*(uint32_t*)(RAM + addr)); // lw
                    else if (f3 == 3) X[rd] = *(int64_t*)(RAM + addr);        // ld
                    else if (f3 == 0) X[rd] = (int8_t)(*(int8_t*)(RAM + addr)); // lb
                    else if (f3 == 4) X[rd] = *(uint8_t*)(RAM + addr); // lbu
                    else if (f3 == 1) X[rd] = (int16_t)(*(uint16_t*)(RAM + addr)); // lh
                    else if (f3 == 5) X[rd] = *(uint16_t*)(RAM + addr); // lhu
                }
                break;
            case 0x23: // STORE (sw, sd, sb, sh)
                {
                    int32_t imm_s = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
                    imm_s = (imm_s << 20) >> 20;
                    uint64_t addr = X[rs1] + imm_s;
                    if (addr >= RAM_SIZE) { printf("Memory Write Fault\n"); goto end; }
                    if (f3 == 2) *(uint32_t*)(RAM + addr) = (uint32_t)X[rs2]; // sw
                    else if (f3 == 3) *(uint64_t*)(RAM + addr) = X[rs2];      // sd
                    else if (f3 == 0) *(uint8_t*)(RAM + addr) = (uint8_t)X[rs2]; // sb
                    else if (f3 == 1) *(uint16_t*)(RAM + addr) = (uint16_t)X[rs2]; // sh
                }
                break;
            
            // ==================== RV64F/RV64D 浮點數指令 ====================
            case 0x07: // LOAD-FP (flw, fld)
                {
                    uint64_t addr = X[rs1] + imm_i;
                    if (addr + 8 > RAM_SIZE) { printf("Memory Read Fault (FP)\n"); goto end; }
                    if (f3 == 2) {
                        // FLW - load 32-bit float, NaN-box it into 64 bits
                        uint32_t val = *(uint32_t*)(RAM + addr);
                        F[rd] = (double)*(float*)&val;
                    } else if (f3 == 3) {
                        // FLD - load 64-bit double
                        F[rd] = *(double*)(RAM + addr);
                    }
                }
                break;
            case 0x27: // STORE-FP (fsw, fsd)
                {
                    int32_t imm_s = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
                    imm_s = (imm_s << 20) >> 20;
                    uint64_t addr = X[rs1] + imm_s;
                    if (addr >= RAM_SIZE) { printf("Memory Write Fault (FP)\n"); goto end; }
                    if (f3 == 2) {
                        // FSW - store 32-bit float
                        float val = (float)F[rs2];
                        *(uint32_t*)(RAM + addr) = *(uint32_t*)&val;
                    } else if (f3 == 3) {
                        // FSD - store 64-bit double
                        if (addr + 8 > RAM_SIZE) { printf("Memory Write Fault (FP)\n"); goto end; }
                        *(double*)(RAM + addr) = F[rs2];
                    }
                }
                break;
            
            // ==================== RV64A 原子指令 ====================
            case 0x2F: // AMO (Atomic Memory Operations)
                {
                    uint32_t funct5 = (inst >> 27) & 0x1F;
                    uint32_t funct3 = (inst >> 12) & 0x7;
                    uint64_t addr = X[rs1];
                    
                    if (addr >= RAM_SIZE) { printf("Memory Fault (AMO)\n"); goto end; }
                    
                    if (funct3 == 2) { // 32-bit operations
                        int32_t old_val = *(int32_t*)(RAM + addr);
                        int32_t new_val;
                        
                        if (funct5 == 0x02) { // LR.W
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x03) { // SC.W
                            new_val = (int32_t)X[rs2];
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = 0; // success
                        } else if (funct5 == 0x00) { // AMOADD.W
                            new_val = old_val + (int32_t)X[rs2];
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x01) { // AMOSWAP.W
                            new_val = (int32_t)X[rs2];
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x04) { // AMOXOR.W
                            new_val = old_val ^ (int32_t)X[rs2];
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x03) { // AMOAND.W
                            new_val = old_val & (int32_t)X[rs2];
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x06) { // AMOOR.W
                            new_val = old_val | (int32_t)X[rs2];
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x05) { // AMOMIN.W
                            new_val = ((int32_t)X[rs2] < old_val) ? (int32_t)X[rs2] : old_val;
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x07) { // AMOMAX.W
                            new_val = ((int32_t)X[rs2] > old_val) ? (int32_t)X[rs2] : old_val;
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x09) { // AMOMINU.W
                            new_val = ((uint32_t)X[rs2] < (uint32_t)old_val) ? (int32_t)X[rs2] : old_val;
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        } else if (funct5 == 0x0A) { // AMOMAXU.W
                            new_val = ((uint32_t)X[rs2] > (uint32_t)old_val) ? (int32_t)X[rs2] : old_val;
                            *(int32_t*)(RAM + addr) = new_val;
                            X[rd] = (int64_t)old_val;
                        }
                    } else if (funct3 == 3) { // 64-bit operations
                        int64_t old_val = *(int64_t*)(RAM + addr);
                        int64_t new_val;
                        
                        if (funct5 == 0x02) { // LR.D
                            X[rd] = old_val;
                        } else if (funct5 == 0x03) { // SC.D
                            new_val = X[rs2];
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = 0; // success
                        } else if (funct5 == 0x00) { // AMOADD.D
                            new_val = old_val + X[rs2];
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x01) { // AMOSWAP.D
                            new_val = X[rs2];
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x04) { // AMOXOR.D
                            new_val = old_val ^ X[rs2];
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x03) { // AMOAND.D
                            new_val = old_val & X[rs2];
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x06) { // AMOOR.D
                            new_val = old_val | X[rs2];
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x05) { // AMOMIN.D
                            new_val = (X[rs2] < old_val) ? X[rs2] : old_val;
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x07) { // AMOMAX.D
                            new_val = (X[rs2] > old_val) ? X[rs2] : old_val;
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x09) { // AMOMINU.D
                            new_val = ((uint64_t)X[rs2] < (uint64_t)old_val) ? X[rs2] : old_val;
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        } else if (funct5 == 0x0A) { // AMOMAXU.D
                            new_val = ((uint64_t)X[rs2] > (uint64_t)old_val) ? X[rs2] : old_val;
                            *(int64_t*)(RAM + addr) = new_val;
                            X[rd] = old_val;
                        }
                    }
                }
                break;
            case 0x53: // OP-FP (double precision / single precision)
                {
                    // f7 = funct7, f3 = funct3, rs1 = src1, rs2 = src2, rd = dest
                    int rs3 = (inst >> 27) & 0x1F; // for madd/msub/nmadd/nmsub
                    
                    if (f7 == 0x00) { // FADD.D
                        F[rd] = F[rs1] + F[rs2];
                    } else if (f7 == 0x04) { // FSUB.D
                        F[rd] = F[rs1] - F[rs2];
                    } else if (f7 == 0x08) { // FMUL.D
                        F[rd] = F[rs1] * F[rs2];
                    } else if (f7 == 0x0C) { // FDIV.D
                        if (F[rs2] != 0.0) F[rd] = F[rs1] / F[rs2];
                        else F[rd] = 0.0;
                    } else if (f7 == 0x2C) { // FSQRT.D
                        F[rd] = sqrt(F[rs2]);
                    } else if ((f7 == 0x51 || f7 == 0x50) && (f3 >= 0 && f3 <= 2)) {
                        // f7=0x51 for double precision, f7=0x50 for single precision
                        if (f3 == 0) {
                            X[rd] = (F[rs1] <= F[rs2]) ? 1 : 0; // FLE.D/S
                        } else if (f3 == 1) {
                            X[rd] = (F[rs1] < F[rs2]) ? 1 : 0; // FLT.D/S
                        } else if (f3 == 2) {
                            X[rd] = (F[rs1] == F[rs2]) ? 1 : 0; // FEQ.D/S
                        }
                    } else if (f7 == 0x20 && f3 == 2) { // FEQ.D
                        X[rd] = (int32_t)F[rs1];
                    } else if (f7 == 0x61) { // FCVT.WU.D (any rounding mode)
                        X[rd] = (uint32_t)F[rs1];
                    } else if (f7 == 0x68 && rs2 == 0) { // FCVT.D.W
                        F[rd] = (double)(int32_t)X[rs1];
                    } else if (f7 == 0x69 && rs2 == 0) { // FCVT.D.WU
                        F[rd] = (double)(uint32_t)X[rs1];
                    } else if (f7 == 0x70 && rs2 == 0) { // FMV.X.D
                        X[rd] = *(int64_t*)&F[rs1];
                    } else if (f7 == 0x78 && rs2 == 0) { // FMV.D.X
                        F[rd] = *(double*)&X[rs1];
                    } else if (f7 == 0x20) { // FCVT.D.S
                        F[rd] = (double)(float)F[rs1];
                    } else if (f7 == 0x21) { // FCVT.S.D
                        F[rd] = (double)(float)F[rs1];
                    }
                }
                break;
            case 0x47: // OP-FP (double precision arithmetic)
                {
                    int rs3 = (inst >> 27) & 0x1F;
                    (void)rs3;
                    if (f7 == 0x00) { // FADD.D
                        F[rd] = F[rs1] + F[rs2];
                    } else if (f7 == 0x04) { // FSUB.D
                        F[rd] = F[rs1] - F[rs2];
                    } else if (f7 == 0x08) { // FMUL.D
                        F[rd] = F[rs1] * F[rs2];
                    } else if (f7 == 0x0C) { // FDIV.D
                        if (F[rs2] != 0.0) F[rd] = F[rs1] / F[rs2];
                        else F[rd] = 0.0;
                    } else if (f7 == 0x2C) { // FSQRT.D
                        F[rd] = sqrt(F[rs2]);
                    }
                }
                break;
            case 0x57: // OP-FP (double precision compare/convert/move)
                {
                    int rs3 = (inst >> 27) & 0x1F;
                    (void)rs3;
                    if (f7 == 0x20 && f3 == 2) { // FEQ.D
                        X[rd] = (F[rs1] == F[rs2]) ? 1 : 0;
                    } else if (f7 == 0x21 && f3 == 2) { // FLT.D
                        X[rd] = (F[rs1] < F[rs2]) ? 1 : 0;
                    } else if (f7 == 0x22 && f3 == 2) { // FLE.D
                        X[rd] = (F[rs1] <= F[rs2]) ? 1 : 0;
                    } else if (f7 == 0x60) { // FCVT.W.D
                        X[rd] = (int32_t)F[rs1];
                    } else if (f7 == 0x61) { // FCVT.WU.D
                        X[rd] = (uint32_t)F[rs1];
                    } else if (f7 == 0x68 && rs2 == 0) { // FCVT.D.W
                        F[rd] = (double)(int32_t)X[rs1];
                    } else if (f7 == 0x69 && rs2 == 0) { // FCVT.D.WU
                        F[rd] = (double)(uint32_t)X[rs1];
                    } else if (f7 == 0x70 && rs2 == 0) { // FMV.X.D
                        X[rd] = *(int64_t*)&F[rs1];
                    } else if (f7 == 0x78 && rs2 == 0) { // FMV.D.X
                        F[rd] = *(double*)&X[rs1];
                    } else if (f7 == 0x20) { // FCVT.D.S
                        F[rd] = (double)(float)F[rs1];
                    } else if (f7 == 0x21) { // FCVT.S.D
                        F[rd] = (double)(float)F[rs1];
                    }
                }
                break;
            case 0x43: // OP-FP single precision
                {
                    int rs3 = (inst >> 27) & 0x1F;
                    
                    float fs1 = (float)F[rs1];
                    float fs2 = (float)F[rs2];
                    float fd;
                    
                    if (f7 == 0x00) { // FADD.S
                        fd = fs1 + fs2;
                    } else if (f7 == 0x04) { // FSUB.S
                        fd = fs1 - fs2;
                    } else if (f7 == 0x08) { // FMUL.S
                        fd = fs1 * fs2;
                    } else if (f7 == 0x0C) { // FDIV.S
                        if (fs2 != 0.0f) fd = fs1 / fs2;
                        else fd = 0.0f;
                    } else if (f7 == 0x2C) { // FSQRT.S
                        fd = sqrtf(fs2);
                    } else if (f7 == 0x20 && f3 == 2) { // FEQ.S
                        X[rd] = (fs1 == fs2) ? 1 : 0;
                    } else if (f7 == 0x21 && f3 == 2) { // FLT.S
                        X[rd] = (fs1 < fs2) ? 1 : 0;
                    } else if (f7 == 0x22 && f3 == 2) { // FLE.S
                        X[rd] = (fs1 <= fs2) ? 1 : 0;
                    } else if (f7 == 0x60) { // FCVT.W.S (any rounding mode)
                        X[rd] = (int32_t)fs1;
                    } else if (f7 == 0x61) { // FCVT.WU.S (any rounding mode)
                        X[rd] = (uint32_t)fs1;
                    } else if (f7 == 0x68 && rs2 == 0) { // FCVT.S.W
                        fd = (float)(int32_t)X[rs1];
                    } else if (f7 == 0x69 && rs2 == 0) { // FCVT.S.WU
                        fd = (float)(uint32_t)X[rs1];
                    } else if (f7 == 0x70 && rs2 == 0) { // FMV.X.W
                        X[rd] = (int32_t)fs1;
                    } else if (f7 == 0x78 && rs2 == 0) { // FMV.W.X
                        fd = *(float*)&X[rs1];
                    }
                    
                    // Store the result as double for simplicity
                    F[rd] = (double)fd;
                }
                break;
            default:
                if (opcode == 0x73) { // ECALL
                    // a7 = syscall no, a0-a5 = args (Linux RV64)
                    long long syscall = X[17]; // a7
                    if (syscall == 64 || syscall == 1) { // write (64 on Linux, 1 legacy)
                        int fd = (int)X[10]; // a0
                        uint64_t addr = X[11]; // a1
                        size_t count = (size_t)X[12]; // a2
                        if (addr + count > RAM_SIZE) {
                            printf("Memory Read Fault (ECALL write)\n");
                            goto end;
                        }
                        if (fd == 1 || fd == 2) {
                            write(fd, RAM + addr, count);
                        }
                    } else if (syscall == 93 || syscall == 94 || syscall == 2) { // exit/exit_group
                        printf("Program exited with code %lld\n", (long long)X[10]);
                        goto end;
                    }
                    X[10] = 0; // return 0 by default
                    break;
                }
                printf("Fault: Unknown Opcode 0x%x at PC=0x%llx\n", opcode, (unsigned long long)PC);
                goto end;
        }
        PC = next_pc;
        X[0] = 0; // enforce x0 is always zero
        steps++;
    }

end:
    print_registers();
    printf("Executed %d instructions.\n", steps);
    return 0;
}
