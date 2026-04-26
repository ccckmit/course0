/*
 * ll0i.c - A simple LLVM IR interpreter
 * Supports a subset of LLVM IR sufficient to run fact.ll
 *
 * Compile: cc -o ll0i ll0i.c
 * Usage:   ./ll0i <file.ll>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define MAX_FUNCTIONS   64
#define MAX_BLOCKS      128
#define MAX_INSTRS      512
#define MAX_ARGS        16
#define MAX_LINE        1024
#define MAX_NAME        128

typedef enum {
    OP_ALLOCA, OP_STORE, OP_LOAD, OP_ICMP,
    OP_BR, OP_JMP, OP_ADD, OP_SUB, OP_MUL,
    OP_CALL, OP_RET,
} Opcode;

typedef enum { PRED_EQ,PRED_NE,PRED_SLT,PRED_SLE,PRED_SGT,PRED_SGE } Pred;
typedef enum { VK_IMM, VK_REG } VKind;

typedef struct {
    VKind kind;
    int32_t imm;
    char name[MAX_NAME];
} Value;

typedef struct {
    Opcode op;
    char   dst[MAX_NAME];
    Value  src[3];
    Pred   pred;
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

static Function functions[MAX_FUNCTIONS];
static int      n_functions = 0;

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

/* ── Parser helpers ── */
static char *skip_ws(char *p){ while(*p==' '||*p=='\t') p++; return p; }

static char *read_token(char *p, char *buf, int sz) {
    p=skip_ws(p); int i=0;
    while(*p&&!isspace((unsigned char)*p)&&*p!=','&&*p!='('&&*p!=')'&&i<sz-1)
        buf[i++]=*p++;
    buf[i]=0; return p;
}

static void rstrip(char *s){
    int n=strlen(s);
    while(n>0&&(s[n-1]=='\n'||s[n-1]=='\r'||s[n-1]==' '||s[n-1]=='\t')) s[--n]=0;
}

static Value parse_value(const char *tok){
    Value v; memset(&v,0,sizeof v);
    if(tok[0]=='%'){v.kind=VK_REG;strncpy(v.name,tok,MAX_NAME-1);}
    else{char*e;long n=strtol(tok,&e,10);
         if(e!=tok){v.kind=VK_IMM;v.imm=(int32_t)n;}
         else{v.kind=VK_REG;strncpy(v.name,tok,MAX_NAME-1);}}
    return v;
}

/* parse "type [noundef] value" */
static char *parse_arg(char *p, Value *v){
    char t[MAX_NAME];
    p=skip_ws(p); p=read_token(p,t,sizeof t); /* type */
    p=skip_ws(p);
    if(*p==','||*p==')'||!*p){*v=parse_value(t);return p;}
    char t2[MAX_NAME]; p=read_token(p,t2,sizeof t2);
    p=skip_ws(p);
    if(!strcmp(t2,"noundef")){char t3[MAX_NAME];p=read_token(p,t3,sizeof t3);*v=parse_value(t3);}
    else *v=parse_value(t2);
    return p;
}

static Block *get_block(Function *fn, const char *name){
    for(int i=0;i<fn->n_blocks;i++) if(!strcmp(fn->blocks[i].name,name)) return &fn->blocks[i];
    Block *b=&fn->blocks[fn->n_blocks++];
    strncpy(b->name,name,MAX_NAME-1); b->n_instrs=0;
    return b;
}

/* ── Main parser ── */
static void parse_ll(FILE *fp){
    char line[MAX_LINE];
    Function *cur_fn=NULL;
    Block    *cur_blk=NULL;

    while(fgets(line,sizeof line,fp)){
        rstrip(line);
        char *p=line; p=skip_ws(p);
        if(!*p||*p==';') continue;

        if(!strncmp(p,"define ",7)){
            cur_fn=&functions[n_functions++];
            memset(cur_fn,0,sizeof *cur_fn);
            cur_blk=NULL;
            char *at=strchr(p,'@'); if(!at) continue; at++;
            char *par=strchr(at,'('); if(!par) continue;
            int len=par-at; if(len>=MAX_NAME)len=MAX_NAME-1;
            strncpy(cur_fn->name,at,len); cur_fn->name[len]=0;

            char abuf[MAX_LINE]; char *ae=strchr(par+1,')');
            if(!ae) continue;
            int alen=ae-(par+1); if(alen>=(int)sizeof(abuf))alen=sizeof(abuf)-1;
            strncpy(abuf,par+1,alen); abuf[alen]=0;
            char *ap=abuf;
            while(*ap){
                ap=skip_ws(ap); if(!*ap) break;
                char t1[MAX_NAME]; ap=read_token(ap,t1,sizeof t1); if(!t1[0]) break;
                ap=skip_ws(ap);
                char t2[MAX_NAME]; ap=read_token(ap,t2,sizeof t2);
                if(!strcmp(t2,"noundef")){char t3[MAX_NAME];ap=read_token(ap,t3,sizeof t3);
                    if(t3[0]=='%') strncpy(cur_fn->arg_names[cur_fn->n_args++],t3,MAX_NAME-1);}
                else{if(t2[0]=='%') strncpy(cur_fn->arg_names[cur_fn->n_args++],t2,MAX_NAME-1);}
                ap=skip_ws(ap); if(*ap==',') ap++;
            }
            continue;
        }

        if(*p=='}'){cur_fn=NULL;cur_blk=NULL;continue;}
        if(!cur_fn) continue;

        /* detect block label "N:" */
        {
            char ltok[MAX_NAME]; int li=0; char *lp=p;
            while(*lp&&*lp!=':'&&!isspace((unsigned char)*lp)&&li<MAX_NAME-1) ltok[li++]=*lp++;
            ltok[li]=0;
            if(*lp==':'&&li>0){cur_blk=get_block(cur_fn,ltok);continue;}
        }

        if(!cur_blk) cur_blk=get_block(cur_fn,"entry");

        Instr ins; memset(&ins,0,sizeof ins);
        char tok[MAX_NAME];

        if(*p=='%'){
            char *eq=strstr(p," = "); if(!eq) continue;
            int dlen=eq-p; if(dlen>=MAX_NAME)dlen=MAX_NAME-1;
            strncpy(ins.dst,p,dlen); ins.dst[dlen]=0;
            p=eq+3;
        }
        p=skip_ws(p); p=read_token(p,tok,sizeof tok);

        if(!strcmp(tok,"alloca")){
            ins.op=OP_ALLOCA;
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"store")){
            ins.op=OP_STORE; ins.dst[0]=0;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t);
            p=skip_ws(p);
            char vt[MAX_NAME]; p=read_token(p,vt,sizeof vt);
            if(!strcmp(vt,"noundef")) p=read_token(p,vt,sizeof vt);
            ins.src[0]=parse_value(vt);
            p=skip_ws(p); if(*p==',') p++;
            p=read_token(p,t,sizeof t); /* ptr */
            p=skip_ws(p);
            char pt[MAX_NAME]; p=read_token(p,pt,sizeof pt);
            ins.src[1]=parse_value(pt);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"load")){
            ins.op=OP_LOAD;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t);
            p=skip_ws(p); if(*p==',') p++;
            p=read_token(p,t,sizeof t); /* ptr */
            p=skip_ws(p);
            char pt[MAX_NAME]; p=read_token(p,pt,sizeof pt);
            pt[strcspn(pt,",")]=0;
            ins.src[0]=parse_value(pt);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"icmp")){
            ins.op=OP_ICMP;
            char pr[MAX_NAME]; p=read_token(p,pr,sizeof pr);
            if(!strcmp(pr,"eq"))ins.pred=PRED_EQ;
            else if(!strcmp(pr,"ne"))ins.pred=PRED_NE;
            else if(!strcmp(pr,"slt"))ins.pred=PRED_SLT;
            else if(!strcmp(pr,"sle"))ins.pred=PRED_SLE;
            else if(!strcmp(pr,"sgt"))ins.pred=PRED_SGT;
            else if(!strcmp(pr,"sge"))ins.pred=PRED_SGE;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t);
            p=skip_ws(p);
            char a[MAX_NAME]; p=read_token(p,a,sizeof a); a[strcspn(a,",")]=0;
            ins.src[0]=parse_value(a);
            p=skip_ws(p); if(*p==',') p++;
            char b[MAX_NAME]; p=read_token(p,b,sizeof b);
            ins.src[1]=parse_value(b);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"br")){
            p=skip_ws(p);
            char t2[MAX_NAME]; p=read_token(p,t2,sizeof t2);
            if(!strcmp(t2,"label")){
                ins.op=OP_JMP;
                char lt[MAX_NAME]; p=read_token(p,lt,sizeof lt);
                strncpy(ins.jmp_label, lt[0]=='%'?lt+1:lt, MAX_NAME-1);
            } else {
                ins.op=OP_BR;
                char ct[MAX_NAME]; p=read_token(p,ct,sizeof ct); ct[strcspn(ct,",")]=0;
                ins.src[0]=parse_value(ct);
                p=skip_ws(p); if(*p==',') p++;
                p=read_token(p,t2,sizeof t2); /* label */
                char lt[MAX_NAME]; p=read_token(p,lt,sizeof lt); lt[strcspn(lt,",")]=0;
                strncpy(ins.true_label,  lt[0]=='%'?lt+1:lt, MAX_NAME-1);
                p=skip_ws(p); if(*p==',') p++;
                p=read_token(p,t2,sizeof t2); /* label */
                char lf[MAX_NAME]; p=read_token(p,lf,sizeof lf);
                strncpy(ins.false_label, lf[0]=='%'?lf+1:lf, MAX_NAME-1);
            }
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"add")||!strcmp(tok,"sub")||!strcmp(tok,"mul")){
            ins.op= !strcmp(tok,"add")?OP_ADD:!strcmp(tok,"sub")?OP_SUB:OP_MUL;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t);
            if(!strcmp(t,"nsw")||!strcmp(t,"nuw")) p=read_token(p,t,sizeof t);
            p=skip_ws(p);
            char a[MAX_NAME]; p=read_token(p,a,sizeof a); a[strcspn(a,",")]=0;
            ins.src[0]=parse_value(a);
            p=skip_ws(p); if(*p==',') p++;
            char b[MAX_NAME]; p=read_token(p,b,sizeof b);
            ins.src[1]=parse_value(b);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"call")){
            ins.op=OP_CALL;
            char rt[MAX_NAME]; p=read_token(p,rt,sizeof rt);
            p=skip_ws(p);
            char cn[MAX_NAME]; p=read_token(p,cn,sizeof cn);
            char *par2=strchr(cn,'('); if(par2)*par2=0;
            strncpy(ins.callee, cn[0]=='@'?cn+1:cn, MAX_NAME-1);
            p=skip_ws(p); if(*p=='(') p++;
            ins.call_argc=0;
            while(*p&&*p!=')'){
                p=skip_ws(p); if(*p==')') break;
                Value v; p=parse_arg(p,&v);
                ins.call_args[ins.call_argc++]=v;
                p=skip_ws(p); if(*p==',') p++;
            }
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"ret")){
            ins.op=OP_RET;
            char rt[MAX_NAME]; p=read_token(p,rt,sizeof rt);
            p=skip_ws(p);
            if(*p&&strcmp(rt,"void")!=0){
                char vt[MAX_NAME]; p=read_token(p,vt,sizeof vt);
                ins.src[0]=parse_value(vt);
            }
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
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
