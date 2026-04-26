/*
 * ll0i.c - A simple LLVM IR interpreter
 * Supports a subset of LLVM IR sufficient to run fact.ll
 *
 * Compile: cc -o ll0i ll0i.c ../ll0.c
 * Usage:   ./ll0i <file.ll>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../ll0.h"

/* ── Frame (register file + stack memory) ── */
#define RF_SIZE  512
#define MEM_SIZE 4096

typedef struct Reg {
    char    name[MAX_NAME];
    int32_t val;
    int     is_ptr;
    int     mem_idx;
} Reg;

typedef struct Frame {
    Reg          regs[RF_SIZE];
    int          n_regs;
    int32_t      mem[MEM_SIZE];
    int          mem_top;
    struct Frame *caller;
} Frame;

static Frame *frame_new(Frame *caller) {
    Frame *f = calloc(1, sizeof(Frame));
    f->caller = caller;
    return f;
}
static void frame_free(Frame *f) { free(f); }

static Reg *reg_find(Frame *f, const char *name) {
    for (int i = 0; i < f->n_regs; i++)
        if (!strcmp(f->regs[i].name, name)) return &f->regs[i];
    return NULL;
}
static Reg *reg_set(Frame *f, const char *name, int32_t val) {
    Reg *r = reg_find(f, name);
    if (!r) { r = &f->regs[f->n_regs++]; strncpy(r->name,name,MAX_NAME-1); r->is_ptr=0; }
    r->val = val;
    return r;
}
static int32_t reg_get(Frame *f, const char *name) {
    Reg *r = reg_find(f,name);
    if (!r) { fprintf(stderr,"undefined register: %s\n",name); exit(1); }
    return r->val;
}
static int mem_alloc(Frame *f) {
    int idx = f->mem_top++;
    if (idx>=MEM_SIZE) { fprintf(stderr,"out of memory\n"); exit(1); }
    f->mem[idx]=0;
    return idx;
}
static void mem_store(Frame *f, const char *pn, int32_t val) {
    Reg *r = reg_find(f,pn);
    if (!r||!r->is_ptr) { fprintf(stderr,"store to non-ptr: %s\n",pn); exit(1); }
    f->mem[r->mem_idx] = val;
}
static int32_t mem_load(Frame *f, const char *pn) {
    Reg *r = reg_find(f,pn);
    if (!r||!r->is_ptr) { fprintf(stderr,"load from non-ptr: %s\n",pn); exit(1); }
    return f->mem[r->mem_idx];
}
static int32_t resolve(Frame *f, Value *v) {
    return (v->kind==VK_IMM) ? v->imm : reg_get(f,v->name);
}

/* ── Interpreter ── */
static int32_t exec_function(Function *fn, int32_t *argv, int argc, Frame *parent) {
    Frame *f = frame_new(parent);
    for (int i=0;i<argc&&i<fn->n_args;i++)
        reg_set(f, fn->arg_names[i], argv[i]);

    int bi = 0;
    while (1) {
        if (bi<0||bi>=fn->n_blocks){fprintf(stderr,"bad block idx\n");exit(1);}
        Block *blk = &fn->blocks[bi];

        for (int ii=0; ii<blk->n_instrs; ii++) {
            Instr *ins = &blk->instrs[ii];
            switch(ins->op) {

            case OP_ALLOCA: {
                int idx = mem_alloc(f);
                Reg *r = &f->regs[f->n_regs++];
                strncpy(r->name,ins->dst,MAX_NAME-1);
                r->is_ptr=1; r->mem_idx=idx; r->val=idx;
                break;
            }
            case OP_STORE:
                mem_store(f, ins->src[1].name, resolve(f,&ins->src[0]));
                break;
            case OP_LOAD:
                reg_set(f, ins->dst, mem_load(f,ins->src[0].name));
                break;
            case OP_ICMP: {
                int32_t a=resolve(f,&ins->src[0]), b=resolve(f,&ins->src[1]), r;
                switch(ins->pred){
                    case PRED_EQ:  r=(a==b);break; case PRED_NE:  r=(a!=b);break;
                    case PRED_SLT: r=(a< b);break; case PRED_SLE: r=(a<=b);break;
                    case PRED_SGT: r=(a> b);break; case PRED_SGE: r=(a>=b);break;
                    default: r=0;
                }
                reg_set(f,ins->dst,r);
                break;
            }
            case OP_ADD: reg_set(f,ins->dst,resolve(f,&ins->src[0])+resolve(f,&ins->src[1])); break;
            case OP_SUB: reg_set(f,ins->dst,resolve(f,&ins->src[0])-resolve(f,&ins->src[1])); break;
            case OP_MUL: reg_set(f,ins->dst,resolve(f,&ins->src[0])*resolve(f,&ins->src[1])); break;

            case OP_BR: {
                int32_t cond=resolve(f,&ins->src[0]);
                const char *tgt=cond?ins->true_label:ins->false_label;
                int found=-1;
                for(int k=0;k<fn->n_blocks;k++) if(!strcmp(fn->blocks[k].name,tgt)){found=k;break;}
                if(found<0){fprintf(stderr,"block not found: %s\n",tgt);exit(1);}
                bi=found; goto next_block;
            }
            case OP_JMP: {
                int found=-1;
                for(int k=0;k<fn->n_blocks;k++) if(!strcmp(fn->blocks[k].name,ins->jmp_label)){found=k;break;}
                if(found<0){fprintf(stderr,"block not found: %s\n",ins->jmp_label);exit(1);}
                bi=found; goto next_block;
            }
            case OP_CALL: {
                int32_t ca[MAX_ARGS];
                for(int k=0;k<ins->call_argc;k++) ca[k]=resolve(f,&ins->call_args[k]);
                Function *callee=NULL;
                for(int k=0;k<n_functions;k++) if(!strcmp(functions[k].name,ins->callee)){callee=&functions[k];break;}
                if(!callee){fprintf(stderr,"function not found: %s\n",ins->callee);exit(1);}
                int32_t ret=exec_function(callee,ca,ins->call_argc,f);
                if(ins->dst[0]) reg_set(f,ins->dst,ret);
                break;
            }
            case OP_RET: {
                int32_t retval=0;
                if(ins->src[0].kind==VK_IMM) retval=ins->src[0].imm;
                else if(ins->src[0].name[0]) retval=reg_get(f,ins->src[0].name);
                frame_free(f);
                return retval;
            }
            }
        }
        fprintf(stderr,"block %s has no terminator\n",blk->name); exit(1);
        next_block:;
    }
}

int main(int argc, char *argv[]){
    if(argc<2){fprintf(stderr,"Usage: %s <file.ll>\n",argv[0]);return 1;}
    FILE *fp=fopen(argv[1],"r");
    if(!fp){perror(argv[1]);return 1;}
    parse_ll(fp); fclose(fp);

    Function *main_fn=NULL;
    for(int i=0;i<n_functions;i++)
        if(!strcmp(functions[i].name,"main")){main_fn=&functions[i];break;}
    if(!main_fn){fprintf(stderr,"no @main found\n");return 1;}

    int32_t ret=exec_function(main_fn,NULL,0,NULL);
    printf("Return value: %d\n",ret);
    return 0;
}
