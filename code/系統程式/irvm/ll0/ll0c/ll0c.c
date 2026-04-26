#include "../ll0.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROLOGUE_SIZE 1024

typedef struct {
    char name[MAX_NAME];
    int offset;
} VarMap;

static VarMap varmap[128];
static int n_vars = 0;
static int curr_offset = -24;

static void reset_varmap() {
    n_vars = 0;
    curr_offset = -24;
    if (curr_offset < -8192) {
        fprintf(stderr, "Warning: stack overflow detected\n");
        curr_offset = -8192;
    }
}

static int get_offset(const char *name) {
    for (int i = 0; i < n_vars; i++) {
        if (!strcmp(varmap[i].name, name)) return varmap[i].offset;
    }
    if (n_vars >= 128) {
        fprintf(stderr, "Too many variables\n");
        exit(1);
    }
    int off = curr_offset;
    strcpy(varmap[n_vars].name, name);
    varmap[n_vars].offset = off;
    n_vars++;
    curr_offset -= 8;
    return off;
}

static void load_value(FILE *out, Value *v, const char *reg) {
    if (v->kind == VK_IMM) {
        fprintf(out, "    li %s, %d\n", reg, v->imm);
    } else {
        int off = get_offset(v->name);
        fprintf(out, "    ld %s, %d(s0)\n", reg, off);
    }
}

static void emit_li32(FILE *out, const char *reg, int32_t imm) {
    if (imm >= -2048 && imm <= 2047) {
        fprintf(out, "    li %s, %d\n", reg, imm);
    } else {
        int32_t hi20 = (imm + 0x800) >> 12;
        int32_t lo12 = imm - (hi20 << 12);
        fprintf(out, "    lui %s, %d\n", reg, hi20);
        fprintf(out, "    addi %s, %s, %d\n", reg, reg, lo12);
    }
}

static void load_fvalue(FILE *out, Value *v, const char *freg, int fkind) {
    if (v->kind == VK_FIMM) {
        if (v->fimm_is_int) {
            int32_t imm = (int32_t)v->fimm;
            emit_li32(out, "t0", imm);
            if (fkind == 1) fprintf(out, "    fcvt.s.w %s, t0\n", freg);
            else fprintf(out, "    fcvt.d.w %s, t0\n", freg);
        } else {
            float fv = (float)v->fimm;
            union { float f; uint32_t u; } u;
            u.f = fv;
            emit_li32(out, "t0", (int32_t)u.u);
            fprintf(out, "    fmv.w.x %s, t0\n", freg);
            if (fkind != 1) {
                fprintf(out, "    fcvt.d.s %s, %s\n", freg, freg);
            }
        }
    } else {
        int off = get_offset(v->name);
        fprintf(out, "    fld %s, %d(s0)\n", freg, off);
    }
}

static int block_id(Function *fn, const char *name){
    for (int i = 0; i < fn->n_blocks; i++) {
        if (!strcmp(fn->blocks[i].name, name)) return i;
    }
    return -1;
}

static void compile_function(FILE *out, Function *fn) {
    fprintf(stderr, "DEBUG: compile_function called for %s\n", fn->name);
    reset_varmap();
    fprintf(stderr, "DEBUG: reset_varmap done\n");

    fprintf(out, "\n.global %s\n", fn->name);
    fprintf(out, "%s:\n", fn->name);
    fprintf(out, "    addi sp, sp, -%d\n", PROLOGUE_SIZE);
    fprintf(out, "    sd ra, %d(sp)\n", PROLOGUE_SIZE - 8);
    fprintf(out, "    sd s0, %d(sp)\n", PROLOGUE_SIZE - 16);
    fprintf(out, "    addi s0, sp, %d\n", PROLOGUE_SIZE);

    fprintf(stderr, "Compiling function %s with %d args, %d blocks\n", fn->name, fn->n_args, fn->n_blocks);

    for (int i = 0; i < fn->n_args; i++) {
        int off = get_offset(fn->arg_names[i]);
        fprintf(out, "    sd a%d, %d(s0)\n", i, off);
    }

    for (int b = 0; b < fn->n_blocks; b++) {
        Block *blk = &fn->blocks[b];
        fprintf(out, ".L_%s_%s:\n", fn->name, blk->name);

        for (int i = 0; i < blk->n_instrs; i++) {
            Instr *ins = &blk->instrs[i];
            
            switch (ins->op) {
                case OP_ALLOCA: {
                    int ptr_off = get_offset(ins->dst);
                    int mem_off = curr_offset;
                    int size = ins->alloca_size > 0 ? ins->alloca_size : 8;
                    curr_offset -= size;
                    fprintf(out, "    addi t0, s0, %d\n", mem_off);
                    fprintf(out, "    sd t0, %d(s0)\n", ptr_off);
                    break;
                }
                case OP_STORE: {
                    if (ins->fkind) {
                        load_fvalue(out, &ins->src[0], "ft0", ins->fkind);
                        load_value(out, &ins->src[1], "t1");
                        fprintf(out, "    fsd ft0, 0(t1)\n");
                    } else {
                        load_value(out, &ins->src[0], "t0");
                        load_value(out, &ins->src[1], "t1");
                        fprintf(out, "    sd t0, 0(t1)\n");
                    }
                    break;
                }
                case OP_LOAD: {
                    if (ins->fkind) {
                        load_value(out, &ins->src[0], "t1");
                        fprintf(out, "    fld ft0, 0(t1)\n");
                        int dst_off = get_offset(ins->dst);
                        fprintf(out, "    fsd ft0, %d(s0)\n", dst_off);
                    } else {
                        load_value(out, &ins->src[0], "t1");
                        fprintf(out, "    ld t0, 0(t1)\n");
                        int dst_off = get_offset(ins->dst);
                        fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    }
                    break;
                }
                case OP_ICMP: {
                    load_value(out, &ins->src[0], "t0");
                    load_value(out, &ins->src[1], "t1");
                    switch (ins->pred) {
                        case PRED_EQ:  fprintf(out, "    sub t0, t0, t1\n    seqz t0, t0\n"); break;
                        case PRED_NE:  fprintf(out, "    sub t0, t0, t1\n    snez t0, t0\n"); break;
                        case PRED_SLT: fprintf(out, "    slt t0, t0, t1\n"); break;
                        case PRED_SGT: fprintf(out, "    slt t0, t1, t0\n"); break;
                        case PRED_SLE: fprintf(out, "    slt t0, t1, t0\n    xori t0, t0, 1\n"); break;
                        case PRED_SGE: fprintf(out, "    slt t0, t0, t1\n    xori t0, t0, 1\n"); break;
                        case PRED_ULT: fprintf(out, "    sltu t0, t0, t1\n"); break;
                        case PRED_UGT: fprintf(out, "    sltu t0, t1, t0\n"); break;
                        case PRED_ULE: fprintf(out, "    sltu t0, t1, t0\n    xori t0, t0, 1\n"); break;
                        case PRED_UGE: fprintf(out, "    sltu t0, t0, t1\n    xori t0, t0, 1\n"); break;
                    }
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_ADD:
                case OP_SUB:
                case OP_MUL:
                case OP_SDIV:
                case OP_UDIV:
                case OP_SREM:
                case OP_UREM: {
                    load_value(out, &ins->src[0], "t0");
                    load_value(out, &ins->src[1], "t1");
                    if (ins->op == OP_ADD) fprintf(out, "    add t0, t0, t1\n");
                    else if (ins->op == OP_SUB) fprintf(out, "    sub t0, t0, t1\n");
                    else if (ins->op == OP_MUL) fprintf(out, "    mul t0, t0, t1\n");
                    else if (ins->op == OP_SDIV) fprintf(out, "    div t0, t0, t1\n");
                    else if (ins->op == OP_UDIV) fprintf(out, "    divu t0, t0, t1\n");
                    else if (ins->op == OP_SREM) fprintf(out, "    rem t0, t0, t1\n");
                    else if (ins->op == OP_UREM) fprintf(out, "    remu t0, t0, t1\n");
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_AND:
                case OP_OR:
                case OP_XOR:
                case OP_SHL:
                case OP_LSHR:
                case OP_ASHR: {
                    load_value(out, &ins->src[0], "t0");
                    load_value(out, &ins->src[1], "t1");
                    if (ins->op == OP_AND) fprintf(out, "    and t0, t0, t1\n");
                    else if (ins->op == OP_OR) fprintf(out, "    or t0, t0, t1\n");
                    else if (ins->op == OP_XOR) fprintf(out, "    xor t0, t0, t1\n");
                    else if (ins->op == OP_SHL) fprintf(out, "    sll t0, t0, t1\n");
                    else if (ins->op == OP_LSHR) fprintf(out, "    srl t0, t0, t1\n");
                    else if (ins->op == OP_ASHR) fprintf(out, "    sra t0, t0, t1\n");
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_FADD:
                case OP_FSUB:
                case OP_FMUL:
                case OP_FDIV: {
                    load_fvalue(out, &ins->src[0], "ft0", ins->fkind);
                    load_fvalue(out, &ins->src[1], "ft1", ins->fkind);
                    if (ins->op == OP_FADD) fprintf(out, "    fadd.d ft0, ft0, ft1\n");
                    else if (ins->op == OP_FSUB) fprintf(out, "    fsub.d ft0, ft0, ft1\n");
                    else if (ins->op == OP_FMUL) fprintf(out, "    fmul.d ft0, ft0, ft1\n");
                    else if (ins->op == OP_FDIV) fprintf(out, "    fdiv.d ft0, ft0, ft1\n");
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    fsd ft0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_FCMP: {
                    load_fvalue(out, &ins->src[0], "ft0", ins->fkind);
                    load_fvalue(out, &ins->src[1], "ft1", ins->fkind);
                    switch (ins->fpred) {
                        case FP_OEQ:
                            fprintf(out, "    feq.d t0, ft0, ft1\n");
                            break;
                        case FP_ONE:
                            fprintf(out, "    feq.d t0, ft0, ft1\n");
                            fprintf(out, "    xori t0, t0, 1\n");
                            break;
                        case FP_OLT:
                            fprintf(out, "    flt.d t0, ft0, ft1\n");
                            break;
                        case FP_OLE:
                            fprintf(out, "    fle.d t0, ft0, ft1\n");
                            break;
                        case FP_OGT:
                            fprintf(out, "    flt.d t0, ft1, ft0\n");
                            break;
                        case FP_OGE:
                            fprintf(out, "    fle.d t0, ft1, ft0\n");
                            break;
                    }
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_FPTOSI: {
                    load_fvalue(out, &ins->src[0], "ft0", ins->fkind);
                    fprintf(out, "    fcvt.w.d t0, ft0, rtz\n");
                    fprintf(out, "    sext.w t0, t0\n");
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_SEXT:
                case OP_ZEXT:
                case OP_TRUNC: {
                    load_value(out, &ins->src[0], "t0");
                    if (ins->op == OP_SEXT) {
                        if (ins->src_bits == 32 && ins->dst_bits == 64) {
                            fprintf(out, "    sext.w t0, t0\n");
                        } else if (ins->src_bits == 1 && ins->dst_bits == 64) {
                            fprintf(out, "    andi t0, t0, 1\n");
                            fprintf(out, "    neg t0, t0\n");
                        }
                    } else if (ins->op == OP_ZEXT) {
                        if (ins->src_bits == 1 && ins->dst_bits == 64) {
                            fprintf(out, "    andi t0, t0, 1\n");
                        } else if (ins->src_bits == 32 && ins->dst_bits == 64) {
                            fprintf(out, "    slli t0, t0, 32\n");
                            fprintf(out, "    srli t0, t0, 32\n");
                        }
                    } else { /* OP_TRUNC */
                        if (ins->dst_bits == 1) {
                            fprintf(out, "    andi t0, t0, 1\n");
                        } else if (ins->dst_bits == 32) {
                            fprintf(out, "    sext.w t0, t0\n");
                        }
                    }
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_BR: {
                    load_value(out, &ins->src[0], "t0");
                    int lbl_off = get_offset("__last_block");
                    fprintf(out, "    li t2, %d\n", b);
                    fprintf(out, "    sd t2, %d(s0)\n", lbl_off);
                    fprintf(out, "    bnez t0, .L_%s_%s\n", fn->name, ins->true_label);
                    fprintf(out, "    j .L_%s_%s\n", fn->name, ins->false_label);
                    break;
                }
                case OP_JMP: {
                    int lbl_off = get_offset("__last_block");
                    fprintf(out, "    li t2, %d\n", b);
                    fprintf(out, "    sd t2, %d(s0)\n", lbl_off);
                    fprintf(out, "    j .L_%s_%s\n", fn->name, ins->jmp_label);
                    break;
                }
                case OP_CALL: {
                    if (!strcmp(ins->callee, "putchar") && ins->call_argc >= 1) {
                        load_value(out, &ins->call_args[0], "t0");
                        int off = get_offset("__putc");
                        fprintf(out, "    sd t0, %d(s0)\n", off);
                        fprintf(out, "    li a0, 1\n");
                        fprintf(out, "    addi a1, s0, %d\n", off);
                        fprintf(out, "    li a2, 1\n");
                        fprintf(out, "    li a7, 64\n");
                        fprintf(out, "    ecall\n");
                        fprintf(out, "    mv a0, t0\n");
                    } else if (!strcmp(ins->callee, "printf")) {
                        fprintf(out, "    li a0, 0\n");
                    } else {
                        for (int a = 0; a < ins->call_argc; a++) {
                            load_value(out, &ins->call_args[a], "t0");
                            fprintf(out, "    mv a%d, t0\n", a);
                        }
                        fprintf(out, "    call %s\n", ins->callee);
                    }
                    if (ins->dst[0]) {
                        int dst_off = get_offset(ins->dst);
                        fprintf(out, "    sd a0, %d(s0)\n", dst_off);
                    }
                    break;
                }
                case OP_RET: {
                    if (ins->src[0].kind == VK_IMM || ins->src[0].name[0]) {
                        load_value(out, &ins->src[0], "a0");
                    }
                    int lbl_off = get_offset("__last_block");
                    fprintf(out, "    li t2, %d\n", b);
                    fprintf(out, "    sd t2, %d(s0)\n", lbl_off);
                    fprintf(out, "    j %s_epilogue\n", fn->name);
                    break;
                }
                case OP_GEP: {
                    load_value(out, &ins->src[0], "t0");
                    load_value(out, &ins->src[1], "t1");
                    if (ins->gep_elem_size != 1) {
                        fprintf(out, "    li t2, %d\n", ins->gep_elem_size);
                        fprintf(out, "    mul t1, t1, t2\n");
                    }
                    fprintf(out, "    add t0, t0, t1\n");
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_PTRTOINT:
                case OP_INTTOPTR: {
                    load_value(out, &ins->src[0], "t0");
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_PHI: {
                    int lbl_off = get_offset("__last_block");
                    fprintf(out, "    ld t2, %d(s0)\n", lbl_off);
                    int l0 = block_id(fn, ins->phi_labels[0]);
                    int l1 = block_id(fn, ins->phi_labels[1]);
                    fprintf(out, "    li t3, %d\n", l0);
                    fprintf(out, "    bne t2, t3, .L_%s_%s_phi_else_%d\n", fn->name, blk->name, i);
                    load_value(out, &ins->phi_vals[0], "t0");
                    fprintf(out, "    j .L_%s_%s_phi_end_%d\n", fn->name, blk->name, i);
                    fprintf(out, ".L_%s_%s_phi_else_%d:\n", fn->name, blk->name, i);
                    load_value(out, &ins->phi_vals[1], "t0");
                    fprintf(out, ".L_%s_%s_phi_end_%d:\n", fn->name, blk->name, i);
                    int dst_off = get_offset(ins->dst);
                    fprintf(out, "    sd t0, %d(s0)\n", dst_off);
                    break;
                }
                case OP_SWITCH: {
                    load_value(out, &ins->src[0], "t0");
                    for (int c = 0; c < ins->switch_n; c++) {
                        fprintf(out, "    li t1, %lld\n", (long long)ins->switch_vals[c]);
                        fprintf(out, "    sub t1, t0, t1\n");
                        fprintf(out, "    beqz t1, .L_%s_%s\n", fn->name, ins->switch_labels[c]);
                    }
                    fprintf(out, "    j .L_%s_%s\n", fn->name, ins->switch_default);
                    break;
                }
            }
        }
    }

    fprintf(out, "%s_epilogue:\n", fn->name);
    fprintf(out, "    ld ra, %d(sp)\n", PROLOGUE_SIZE - 8);
    fprintf(out, "    ld s0, %d(sp)\n", PROLOGUE_SIZE - 16);
    fprintf(out, "    addi sp, sp, %d\n", PROLOGUE_SIZE);
    fprintf(out, "    ret\n");
}

int main(int argc, char **argv) {
    char *in_filename = NULL;
    char *out_filename = NULL;

    // 解析命令列參數
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                out_filename = argv[++i];
            } else {
                fprintf(stderr, "Error: Missing output file after -o\n");
                return 1;
            }
        } else {
            in_filename = argv[i];
        }
    }

    if (!in_filename || !out_filename) {
        fprintf(stderr, "Usage: %s <file.ll> -o <file.s>\n", argv[0]);
        return 1;
    }

    FILE *in_fp = fopen(in_filename, "r");
    if (!in_fp) {
        perror(in_filename);
        return 1;
    }
    parse_ll(in_fp);
    fclose(in_fp);
    fprintf(stderr, "DEBUG: Parse complete, n_functions=%d\n", n_functions);

    FILE *out_fp = fopen(out_filename, "w");
    if (!out_fp) {
        perror(out_filename);
        return 1;
    }

    fprintf(out_fp, ".text\n");
    fprintf(stderr, "DEBUG: Starting function compilation loop, n_functions=%d\n", n_functions);
    for (int i = 0; i < n_functions; i++) {
        fprintf(stderr, "DEBUG: About to compile function %d: %s\n", i, functions[i].name);
        compile_function(out_fp, &functions[i]);
    }
    
    fclose(out_fp);
    return 0;
}
