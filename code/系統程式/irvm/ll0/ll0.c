#include "ll0.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Function functions[MAX_FUNCTIONS];
int      n_functions = 0;

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

static Value parse_value_float(const char *tok){
    Value v; memset(&v,0,sizeof v);
    if(tok[0]=='%'){v.kind=VK_REG;strncpy(v.name,tok,MAX_NAME-1);return v;}
    v.kind = VK_FIMM;
    v.fimm = strtod(tok, NULL);
    v.fimm_is_int = (strchr(tok,'.')==NULL && strchr(tok,'e')==NULL && strchr(tok,'E')==NULL);
    return v;
}

static int parse_type_bits(const char *tok){
    if(tok[0]=='i'){
        int bits=atoi(tok+1);
        return bits>0?bits:0;
    }
    return 0;
}

static int type_size(const char *tok){
    if (!strcmp(tok, "i8")) return 1;
    if (!strcmp(tok, "i16")) return 2;
    if (!strcmp(tok, "i32")) return 4;
    if (!strcmp(tok, "i64")) return 8;
    if (!strcmp(tok, "ptr")) return 8;
    if (!strcmp(tok, "float")) return 8;
    if (!strcmp(tok, "double")) return 8;
    return 8;
}

/* parse "type [noundef] value" */
static char *parse_arg(char *p, Value *v){
    char t[MAX_NAME];
    p=skip_ws(p); p=read_token(p,t,sizeof t); /* type */
    p=skip_ws(p);
    if(*p==','||*p==')'||!*p){*v=parse_value(t);return p;}
    char t2[MAX_NAME]; p=read_token(p,t2,sizeof t2);
    p=skip_ws(p);
    int is_float = (!strcmp(t,"float") || !strcmp(t,"double"));
    if(!strcmp(t2,"noundef")){
        char t3[MAX_NAME];p=read_token(p,t3,sizeof t3);
        *v = is_float ? parse_value_float(t3) : parse_value(t3);
    } else {
        *v = is_float ? parse_value_float(t2) : parse_value(t2);
    }
    return p;
}

static Block *get_block(Function *fn, const char *name){
    for(int i=0;i<fn->n_blocks;i++) if(!strcmp(fn->blocks[i].name,name)) return &fn->blocks[i];
    Block *b=&fn->blocks[fn->n_blocks++];
    strncpy(b->name,name,MAX_NAME-1); b->n_instrs=0;
    fprintf(stderr, "DEBUG: Created block %s in function %s (n_blocks now %d)\n", name, fn->name, fn->n_blocks);
    return b;
}

/* ── Main parser ── */
void parse_ll(FILE *fp){
    char line[MAX_LINE];
    Function *cur_fn=NULL;
    Block    *cur_blk=NULL;
    Instr    *pending_switch=NULL;
    int line_num = 0;

    while(fgets(line,sizeof line,fp)){
        line_num++;
        if (line_num % 20 == 0) fprintf(stderr, "Parsing line %d...\n", line_num);
        rstrip(line);
        char *p=line; p=skip_ws(p);
        if(!*p||*p==';') continue;
        if (pending_switch) {
            if (strchr(p, ']')) { pending_switch = NULL; continue; }
            if (!*p) continue;
            char ct[MAX_NAME]; p=read_token(p,ct,sizeof ct);
            if (!ct[0]) continue;
            char cv[MAX_NAME]; p=read_token(p,cv,sizeof cv);
            p=skip_ws(p); if(*p==',') p++;
            char lt[MAX_NAME]; p=read_token(p,lt,sizeof lt);
            char ln[MAX_NAME]; p=read_token(p,ln,sizeof ln);
            if(ln[0]=='%') memmove(ln,ln+1,strlen(ln));
            if(pending_switch->switch_n < MAX_CASES){
                pending_switch->switch_vals[pending_switch->switch_n]=strtoll(cv,NULL,10);
                strncpy(pending_switch->switch_labels[pending_switch->switch_n], ln, MAX_NAME-1);
                pending_switch->switch_n++;
            }
            continue;
        }

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
            char t[MAX_NAME]; p=read_token(p,t,sizeof t);
            ins.alloca_size = 8;
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"store")){
            ins.op=OP_STORE; ins.dst[0]=0;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t); /* value type */
            p=skip_ws(p);
            char vt[MAX_NAME]; p=read_token(p,vt,sizeof vt);
            if(!strcmp(vt,"noundef")) p=read_token(p,vt,sizeof vt);
            if(!strcmp(t,"float") || !strcmp(t,"double")){
                ins.fkind = (!strcmp(t,"double")) ? 2 : 1;
                ins.src[0]=parse_value_float(vt);
            } else {
                ins.src[0]=parse_value(vt);
            }
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
            if(!strcmp(t,"float")) ins.fkind = 1;
            else if(!strcmp(t,"double")) ins.fkind = 2;
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
            else if(!strcmp(pr,"ult"))ins.pred=PRED_ULT;
            else if(!strcmp(pr,"ule"))ins.pred=PRED_ULE;
            else if(!strcmp(pr,"ugt"))ins.pred=PRED_UGT;
            else if(!strcmp(pr,"uge"))ins.pred=PRED_UGE;
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
        if(!strcmp(tok,"sext")||!strcmp(tok,"zext")||!strcmp(tok,"trunc")){
            if(!strcmp(tok,"sext")) ins.op=OP_SEXT;
            else if(!strcmp(tok,"zext")) ins.op=OP_ZEXT;
            else ins.op=OP_TRUNC;

            char st[MAX_NAME]; p=read_token(p,st,sizeof st); /* src type */
            char sv[MAX_NAME]; p=read_token(p,sv,sizeof sv); /* src value */
            char tokw[MAX_NAME]; p=read_token(p,tokw,sizeof tokw); /* to */
            char dt[MAX_NAME]; p=read_token(p,dt,sizeof dt); /* dst type */

            ins.src[0]=parse_value(sv);
            ins.src_bits=parse_type_bits(st);
            ins.dst_bits=parse_type_bits(dt);

            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"ptrtoint")||!strcmp(tok,"inttoptr")){
            ins.op = !strcmp(tok,"ptrtoint") ? OP_PTRTOINT : OP_INTTOPTR;
            char st[MAX_NAME]; p=read_token(p,st,sizeof st); /* src type */
            char sv[MAX_NAME]; p=read_token(p,sv,sizeof sv); /* src value */
            char tokw[MAX_NAME]; p=read_token(p,tokw,sizeof tokw); /* to */
            char dt[MAX_NAME]; p=read_token(p,dt,sizeof dt); /* dst type */
            (void)st; (void)dt; (void)tokw;
            ins.src[0]=parse_value(sv);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"getelementptr")){
            ins.op=OP_GEP;
            char et[MAX_NAME]; p=read_token(p,et,sizeof et); /* element type */
            ins.gep_elem_size = type_size(et);
            p=skip_ws(p); if(*p==',') p++;
            char pt[MAX_NAME]; p=read_token(p,pt,sizeof pt); /* ptr */
            p=skip_ws(p);
            char pv[MAX_NAME]; p=read_token(p,pv,sizeof pv);
            ins.src[0]=parse_value(pv);
            p=skip_ws(p); if(*p==',') p++;
            char it[MAX_NAME]; p=read_token(p,it,sizeof it); /* index type */
            p=skip_ws(p);
            char iv[MAX_NAME]; p=read_token(p,iv,sizeof iv);
            ins.src[1]=parse_value(iv);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"phi")){
            ins.op=OP_PHI;
            ins.phi_n = 0;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t); /* type */
            (void)t;
            while(*p){
                p=skip_ws(p);
                if(*p!='[') break;
                p++; // [
                char v[MAX_NAME]; p=read_token(p,v,sizeof v);
                ins.phi_vals[ins.phi_n]=parse_value(v);
                p=skip_ws(p); if(*p==',') p++;
                char l[MAX_NAME]; p=read_token(p,l,sizeof l);
                if(l[0]=='%') memmove(l,l+1,strlen(l));
                strncpy(ins.phi_labels[ins.phi_n], l, MAX_NAME-1);
                ins.phi_n++;
                p=strchr(p,']');
                if(!p) break;
                p++;
                p=skip_ws(p);
                if(*p==',') p++;
            }
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"switch")){
            ins.op=OP_SWITCH;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t); /* type */
            p=skip_ws(p);
            char v[MAX_NAME]; p=read_token(p,v,sizeof v);
            ins.src[0]=parse_value(v);
            p=skip_ws(p); if(*p==',') p++;
            char lbl[MAX_NAME]; p=read_token(p,lbl,sizeof lbl); /* label */
            char deflbl[MAX_NAME]; p=read_token(p,deflbl,sizeof deflbl);
            if(deflbl[0]=='%') memmove(deflbl,deflbl+1,strlen(deflbl));
            strncpy(ins.switch_default, deflbl, MAX_NAME-1);
            ins.switch_n = 0;
            Instr *dst = &cur_blk->instrs[cur_blk->n_instrs++];
            *dst = ins;
            pending_switch = dst;
            continue;
        }
        if(!strcmp(tok,"fadd")||!strcmp(tok,"fsub")||!strcmp(tok,"fmul")||!strcmp(tok,"fdiv")){
            if(!strcmp(tok,"fadd")) ins.op=OP_FADD;
            else if(!strcmp(tok,"fsub")) ins.op=OP_FSUB;
            else if(!strcmp(tok,"fmul")) ins.op=OP_FMUL;
            else ins.op=OP_FDIV;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t); /* type */
            ins.fkind = (!strcmp(t,"double")) ? 2 : 1;
            p=skip_ws(p);
            char a[MAX_NAME]; p=read_token(p,a,sizeof a); a[strcspn(a,",")]=0;
            ins.src[0]=parse_value_float(a);
            p=skip_ws(p); if(*p==',') p++;
            char b[MAX_NAME]; p=read_token(p,b,sizeof b);
            ins.src[1]=parse_value_float(b);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"fcmp")){
            ins.op=OP_FCMP;
            char pr[MAX_NAME]; p=read_token(p,pr,sizeof pr);
            if(!strcmp(pr,"oeq")) ins.fpred=FP_OEQ;
            else if(!strcmp(pr,"one")) ins.fpred=FP_ONE;
            else if(!strcmp(pr,"olt")) ins.fpred=FP_OLT;
            else if(!strcmp(pr,"ole")) ins.fpred=FP_OLE;
            else if(!strcmp(pr,"ogt")) ins.fpred=FP_OGT;
            else if(!strcmp(pr,"oge")) ins.fpred=FP_OGE;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t); /* type */
            ins.fkind = (!strcmp(t,"double")) ? 2 : 1;
            p=skip_ws(p);
            char a[MAX_NAME]; p=read_token(p,a,sizeof a); a[strcspn(a,",")]=0;
            ins.src[0]=parse_value_float(a);
            p=skip_ws(p); if(*p==',') p++;
            char b[MAX_NAME]; p=read_token(p,b,sizeof b);
            ins.src[1]=parse_value_float(b);
            cur_blk->instrs[cur_blk->n_instrs++]=ins;
            continue;
        }
        if(!strcmp(tok,"fptosi")){
            ins.op=OP_FPTOSI;
            char t[MAX_NAME]; p=read_token(p,t,sizeof t); /* src type */
            ins.fkind = (!strcmp(t,"double")) ? 2 : 1;
            p=skip_ws(p);
            char sv[MAX_NAME]; p=read_token(p,sv,sizeof sv);
            ins.src[0]=parse_value_float(sv);
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
        if(!strcmp(tok,"add")||!strcmp(tok,"sub")||!strcmp(tok,"mul")||
           !strcmp(tok,"sdiv")||!strcmp(tok,"udiv")||!strcmp(tok,"srem")||!strcmp(tok,"urem")){
            if(!strcmp(tok,"add")) ins.op=OP_ADD;
            else if(!strcmp(tok,"sub")) ins.op=OP_SUB;
            else if(!strcmp(tok,"mul")) ins.op=OP_MUL;
            else if(!strcmp(tok,"sdiv")) ins.op=OP_SDIV;
            else if(!strcmp(tok,"udiv")) ins.op=OP_UDIV;
            else if(!strcmp(tok,"srem")) ins.op=OP_SREM;
            else ins.op=OP_UREM;
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
        if(!strcmp(tok,"and")||!strcmp(tok,"or")||!strcmp(tok,"xor")||
           !strcmp(tok,"shl")||!strcmp(tok,"lshr")||!strcmp(tok,"ashr")){
            if(!strcmp(tok,"and")) ins.op=OP_AND;
            else if(!strcmp(tok,"or")) ins.op=OP_OR;
            else if(!strcmp(tok,"xor")) ins.op=OP_XOR;
            else if(!strcmp(tok,"shl")) ins.op=OP_SHL;
            else if(!strcmp(tok,"lshr")) ins.op=OP_LSHR;
            else ins.op=OP_ASHR;

            char t[MAX_NAME]; p=read_token(p,t,sizeof t);
            while(!strcmp(t,"nsw")||!strcmp(t,"nuw")||!strcmp(t,"exact")) {
                p=read_token(p,t,sizeof t);
            }
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
