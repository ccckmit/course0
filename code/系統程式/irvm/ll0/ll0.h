#ifndef LL0_H
#define LL0_H

#include <stdio.h>
#include <stdint.h>

#define MAX_FUNCTIONS   64
#define MAX_BLOCKS      32
#define MAX_INSTRS      64
#define MAX_ARGS        16
#define MAX_LINE        1024
#define MAX_NAME        128
#define MAX_CASES       16

typedef enum {
    OP_ALLOCA, OP_STORE, OP_LOAD, OP_ICMP,
    OP_BR, OP_JMP, OP_ADD, OP_SUB, OP_MUL,
    OP_SEXT, OP_ZEXT, OP_TRUNC,
    OP_AND, OP_OR, OP_XOR, OP_SHL, OP_LSHR, OP_ASHR,
    OP_SDIV, OP_UDIV, OP_SREM, OP_UREM,
    OP_FADD, OP_FSUB, OP_FMUL, OP_FDIV, OP_FCMP, OP_FPTOSI,
    OP_GEP, OP_PTRTOINT, OP_INTTOPTR, OP_PHI, OP_SWITCH,
    OP_CALL, OP_RET,
} Opcode;

typedef enum {
    PRED_EQ, PRED_NE,
    PRED_SLT, PRED_SLE, PRED_SGT, PRED_SGE,
    PRED_ULT, PRED_ULE, PRED_UGT, PRED_UGE
} Pred;

enum {
    FP_OEQ = 1,
    FP_ONE,
    FP_OLT,
    FP_OLE,
    FP_OGT,
    FP_OGE
};
typedef enum { VK_IMM, VK_REG, VK_FIMM } VKind;

typedef struct {
    VKind kind;
    int32_t imm;
    double fimm;
    int    fimm_is_int;
    char name[MAX_NAME];
} Value;

typedef struct {
    Opcode op;
    char   dst[MAX_NAME];
    Value  src[3];
    Pred   pred;
    int    fkind; // 0=none, 1=float, 2=double
    int    fpred; // float predicate enum
    int    src_bits;
    int    dst_bits;
    int    phi_n;
    Value  phi_vals[2];
    char   phi_labels[2][MAX_NAME];
    int    switch_n;
    int64_t switch_vals[MAX_CASES];
    char   switch_labels[MAX_CASES][MAX_NAME];
    char   switch_default[MAX_NAME];
    int    gep_elem_size;
    int    alloca_size;
    int    alloca_is_ptr;
    char   true_label[MAX_NAME];
    char   false_label[MAX_NAME];
    char   jmp_label[MAX_NAME];
    char   callee[MAX_NAME];
    Value  call_args[MAX_ARGS];
    int    call_argc;
} Instr;

typedef struct {
    char  name[MAX_NAME];
    Instr instrs[MAX_INSTRS];
    int   n_instrs;
} Block;

typedef struct {
    char  name[MAX_NAME];
    char  arg_names[MAX_ARGS][MAX_NAME];
    int   n_args;
    Block blocks[MAX_BLOCKS];
    int   n_blocks;
} Function;

extern Function functions[MAX_FUNCTIONS];
extern int      n_functions;

void parse_ll(FILE *fp);

#endif // LL0_H
