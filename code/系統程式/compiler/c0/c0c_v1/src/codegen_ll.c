/*
 * codegen_ll.c – LLVM IR generator for c0c (complete rewrite)
 *
 * Matches clang -O0 output conventions:
 *  • alloca block at function entry (numbered first after params)
 *  • body SSA regs numbered continuously after allocas
 *  • branch labels are plain numbers (matching reg counter)
 *  • retval alloca at %1 (no params) or %(nparam+1)
 *  • return: store to retval, br label %exit; exit: load, ret
 *  • pointer comparison: icmp ne ptr %p, null
 *
 * Two-pass per function:
 *   Pass 1 (alloca scan): walk body collecting all ND_DECL nodes,
 *           assign each an alloca register. Count total allocas.
 *   Pass 2 (body emit): emit instructions; regs start after allocas.
 */

#include "codegen_ll.h"
#include "ast.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── helpers ──────────────────────────────────────────────────────────────── */
#define MAX(a,b) ((a)>(b)?(a):(b))

/* ── Struct field registry ────────────────────────────────────────────────── */
#define MAX_SF   128
#define MAX_SMEM  64

typedef struct {
    char tag[64];
    struct { char name[64]; char ll[64]; } mem[MAX_SMEM];
    int  nmem;
} SFEntry;

static SFEntry sf_table[MAX_SF];
static int     sf_count = 0;

static void sf_register(const char *tag, Node *snode) {
    if (!snode->children) return;
    for (int i = 0; i < sf_count; i++)
        if (strcmp(sf_table[i].tag, tag) == 0) return;
    if (sf_count >= MAX_SF) return;
    SFEntry *e = &sf_table[sf_count++];
    strncpy(e->tag, tag, 63);
    int idx = 0;
    for (NodeList *c = snode->children; c && idx < MAX_SMEM; c=c->next, idx++) {
        Node *m = c->node;
        strncpy(e->mem[idx].name, m->sval ? m->sval : "", 63);
        if (!m->left) { strncpy(e->mem[idx].ll,"i32",63); continue; }
        switch (m->left->kind) {
            case ND_TYPE_PTR:   strncpy(e->mem[idx].ll,"ptr",63); break;
            case ND_TYPE_BASE: {
                const char *s = m->left->sval ? m->left->sval : "int";
                if      (!strcmp(s,"char"))                strncpy(e->mem[idx].ll,"i8",63);
                else if (!strcmp(s,"short"))               strncpy(e->mem[idx].ll,"i16",63);
                else if (!strcmp(s,"long")||!strcmp(s,"long long")) strncpy(e->mem[idx].ll,"i64",63);
                else if (!strcmp(s,"double"))              strncpy(e->mem[idx].ll,"double",63);
                else                                       strncpy(e->mem[idx].ll,"i32",63);
                break;
            }
            case ND_TYPE_STRUCT: case ND_TYPE_UNION:
                if (m->left->sval && *m->left->sval)
                    snprintf(e->mem[idx].ll, 64, "%%struct.%s", m->left->sval);
                else strncpy(e->mem[idx].ll,"i32",63);
                break;
            default: strncpy(e->mem[idx].ll,"i32",63);
        }
    }
    e->nmem = idx;
}

static int sf_field(const char *tag, const char *fname, char *out_ll, int sz) {
    for (int i = 0; i < sf_count; i++) {
        if (strcmp(sf_table[i].tag, tag)) continue;
        for (int j = 0; j < sf_table[i].nmem; j++) {
            if (!strcmp(sf_table[i].mem[j].name, fname)) {
                strncpy(out_ll, sf_table[i].mem[j].ll, (size_t)sz-1);
                return j;
            }
        }
    }
    strncpy(out_ll,"i32",(size_t)sz-1);
    return 0;
}

/* ── Type helpers ─────────────────────────────────────────────────────────── */
static void node_to_ll(Node *t, char *out, int sz) {
    if (!t) { strncpy(out,"i32",(size_t)sz-1); return; }
    switch (t->kind) {
        case ND_TYPE_PTR:
        case ND_TYPE_ARRAY:
            strncpy(out,"ptr",(size_t)sz-1); break;
        case ND_TYPE_BASE: {
            const char *s = t->sval ? t->sval : "int";
            if      (!strcmp(s,"void"))              strncpy(out,"void",(size_t)sz-1);
            else if (!strcmp(s,"char"))              strncpy(out,"i8",(size_t)sz-1);
            else if (!strcmp(s,"short"))             strncpy(out,"i16",(size_t)sz-1);
            else if (!strcmp(s,"long")||!strcmp(s,"long long")) strncpy(out,"i64",(size_t)sz-1);
            else if (!strcmp(s,"float"))             strncpy(out,"float",(size_t)sz-1);
            else if (!strcmp(s,"double"))            strncpy(out,"double",(size_t)sz-1);
            else                                     strncpy(out,"i32",(size_t)sz-1);
            break;
        }
        case ND_TYPE_STRUCT: case ND_TYPE_UNION:
            if (t->sval && *t->sval) snprintf(out,(size_t)sz,"%%struct.%s",t->sval);
            else strncpy(out,"i32",(size_t)sz-1);
            break;
        default: strncpy(out,"i32",(size_t)sz-1);
    }
    out[sz-1]='\0';
}

static int node_align(Node *t) {
    if (!t) return 4;
    switch (t->kind) {
        case ND_TYPE_PTR: case ND_TYPE_ARRAY: return 8;
        case ND_TYPE_BASE: {
            const char *s = t->sval ? t->sval : "int";
            if (!strcmp(s,"char"))  return 1;
            if (!strcmp(s,"short")) return 2;
            if (!strcmp(s,"long")||!strcmp(s,"long long")) return 8;
            if (!strcmp(s,"double")) return 8;
            return 4;
        }
        case ND_TYPE_STRUCT: case ND_TYPE_UNION: return 8;
        default: return 4;
    }
}

/* align for a given ll string */
static int ll_align(const char *ll) {
    if (!strcmp(ll,"ptr")||!strcmp(ll,"i64")||!strcmp(ll,"double")) return 8;
    if (!strcmp(ll,"i8"))  return 1;
    if (!strcmp(ll,"i16")) return 2;
    if (ll[0]=='%') return 8; /* struct */
    return 4;
}

/* ── Symbol table ─────────────────────────────────────────────────────────── */
#define SBKT 256
typedef struct Sym {
    char name[128];
    char ll[64];         /* value type (pointee) */
    char ptr_target[64]; /* if ll=="ptr", what it points to (e.g. "%struct.Node") */
    int  ptr_reg;        /* alloca/address register */
    int  is_global;
    char gname[128];
    struct Sym *next;
} Sym;

typedef struct Scope { Sym *bkt[SBKT]; struct Scope *up; } Scope;

static unsigned shash(const char *s) {
    unsigned h=5381; while(*s) h=h*33^(unsigned char)*s++; return h%SBKT;
}

static Sym *sym_new(const char *n, const char *ll, int pr) {
    Sym *s=calloc(1,sizeof(Sym));
    strncpy(s->name,n,127); strncpy(s->ll,ll,63); s->ptr_reg=pr;
    return s;
}

static Scope *scope_push(Scope *up) {
    Scope *sc=calloc(1,sizeof(Scope)); sc->up=up; return sc;
}
static Scope *scope_pop(Scope *sc) {
    Scope *up=sc->up;
    for(int i=0;i<SBKT;i++){Sym*s=sc->bkt[i];while(s){Sym*nx=s->next;free(s);s=nx;}}
    free(sc); return up;
}
static void scope_def(Scope *sc, const char *n, const char *ll, int pr) {
    Sym *s=sym_new(n,ll,pr); unsigned h=shash(n);
    s->next=sc->bkt[h]; sc->bkt[h]=s;
}
static void scope_def_global(Scope *sc, const char *n, const char *ll, const char *gn) {
    Sym *s=sym_new(n,ll,0); s->is_global=1;
    strncpy(s->gname,gn,127); unsigned h=shash(n);
    s->next=sc->bkt[h]; sc->bkt[h]=s;
}
static Sym *scope_find(Scope *sc, const char *n) {
    for(;sc;sc=sc->up){Sym*s=sc->bkt[shash(n)];while(s){if(!strcmp(s->name,n))return s;s=s->next;}}
    return NULL;
}

/* ── String literal pool ─────────────────────────────────────────────────── */
#define MAXSTR 256
typedef struct { char *tok; int len; } SLit;
static SLit slits[MAXSTR];
static int  nslits=0;

static int parse_cstr(const char *tok, char *out, int sz) {
    int n=0; const char *p=tok;
    if(*p=='"') p++;
    while(*p && *p!='"' && n<sz-1){
        if(*p=='\\'){p++;
            switch(*p){
                case 'n': out[n++]='\n';break; case 't': out[n++]='\t';break;
                case 'r': out[n++]='\r';break; case '0': out[n++]=0;break;
                case '\\':out[n++]='\\';break; case '"': out[n++]='"';break;
                default:  out[n++]=*p;
            }
            if(*p)p++;
        } else out[n++]=*p++;
    }
    out[n++]=0; return n;
}

static int slit_intern(const char *tok) {
    for(int i=0;i<nslits;i++) if(!strcmp(slits[i].tok,tok)) return i;
    if(nslits>=MAXSTR) return 0;
    char buf[4096]; int len=parse_cstr(tok,buf,sizeof(buf));
    slits[nslits].tok=malloc(strlen(tok)+1); strcpy(slits[nslits].tok,tok);
    slits[nslits].len=len;
    return nslits++;
}

/* ── Codegen context ──────────────────────────────────────────────────────── */
struct Codegen {
    FILE       *out;
    const char *src;
    const char *target;
    Scope      *scope;
    int         reg;          /* next SSA register */
    int         lbl;          /* label counter for named labels */
    char        ret_ll[64];
    int         ret_alloca;
    char        exit_lbl_name[32];
    /* strbuf */
    char       *buf;
    int         bpos, bcap;
    int         to_buf;
};

static void cg_grow(Codegen *cg, int n) {
    if (cg->bpos+n >= cg->bcap) {
        cg->bcap = MAX(cg->bcap*2, cg->bpos+n+4096);
        cg->buf  = realloc(cg->buf,(size_t)cg->bcap);
    }
}

static void vemit(Codegen *cg, const char *fmt, va_list ap) {
    if (!cg->to_buf) { vfprintf(cg->out,fmt,ap); return; }
    char tmp[2048];
    int n=vsnprintf(tmp,sizeof(tmp),fmt,ap);
    if(n<0) n=0;
    if(n>=(int)sizeof(tmp)) n=(int)sizeof(tmp)-1;
    cg_grow(cg,n+1);
    memcpy(cg->buf+cg->bpos,tmp,(size_t)n);
    cg->bpos+=n; cg->buf[cg->bpos]=0;
}

static void W(Codegen *cg, const char *fmt, ...) {
    va_list ap; va_start(ap,fmt); vemit(cg,fmt,ap); va_end(ap);
}

static int R(Codegen *cg) { return cg->reg++; }

/* Emit a named basic-block label.
   LLVM counts each named block as consuming one value slot,
   so we increment reg to keep our numbering aligned. */
static void W_label(Codegen *cg, const char *name) {
    cg->reg++; /* named label occupies one slot */
    W(cg, "\n%s:\n", name);
}

/* ── alloca scan pass ─────────────────────────────────────────────────────── */
/*
 * Walk the body compound, assigning each local ND_DECL a register.
 * Returns total number of allocas emitted.
 * Also defines symbols so the body pass can look them up.
 */
static void scan_decls(Codegen *cg, Node *n) {
    if (!n) return;
    switch (n->kind) {
        case ND_DECL: {
            char ll[64]; node_to_ll(n->left, ll, sizeof(ll));
            int al = n->left ? node_align(n->left) : 4;
            int r  = cg->reg++;
            /* array: alloca [N x elem] */
            if (n->left && n->left->kind == ND_TYPE_ARRAY) {
                char ell[64]; node_to_ll(n->left->left, ell, sizeof(ell));
                long long sz=5;
                if (n->left->right && n->left->right->kind==ND_INT_LIT)
                    sz=n->left->right->ival;
                W(cg,"  %%%d = alloca [%lld x %s], align %d\n",r,sz,ell,al);
                strncpy(ll,ell,sizeof(ll)-1);
            } else if (n->left && (n->left->kind==ND_TYPE_STRUCT||
                                   n->left->kind==ND_TYPE_UNION)) {
                char sll[64]; node_to_ll(n->left,sll,sizeof(sll));
                W(cg,"  %%%d = alloca %s, align %d\n",r,sll,al);
                strncpy(ll,sll,sizeof(ll)-1);
            } else {
                W(cg,"  %%%d = alloca %s, align %d\n",r,ll,al);
            }
            if (n->sval && *n->sval) {
                scope_def(cg->scope, n->sval, ll, r);
                /* if it's a pointer to a struct, record ptr_target */
                if (n->left && n->left->kind == ND_TYPE_PTR) {
                    Node *inner = n->left->left;
                    if (inner && (inner->kind==ND_TYPE_STRUCT||inner->kind==ND_TYPE_UNION)
                        && inner->sval) {
                        Sym *s = scope_find(cg->scope, n->sval);
                        if (s) snprintf(s->ptr_target, sizeof(s->ptr_target),
                                        "%%struct.%s", inner->sval);
                    }
                }
            }
            break;
        }
        case ND_COMPOUND:
            for (NodeList *c=n->children;c;c=c->next) scan_decls(cg,c->node);
            break;
        case ND_IF:
            scan_decls(cg,n->right); scan_decls(cg,n->extra); break;
        case ND_WHILE: case ND_DO_WHILE:
            scan_decls(cg,n->right); scan_decls(cg,n->left); break;
        case ND_FOR:
            if (n->left) scan_decls(cg,n->left);
            if (n->children) scan_decls(cg,n->children->node);
            break;
        case ND_SWITCH:
            scan_decls(cg,n->right); break;
        case ND_LABEL:
            scan_decls(cg,n->left); break;
        default: break;
    }
}

/* forward declarations */
static int emit_expr(Codegen *cg, Node *n);
static int emit_addr(Codegen *cg, Node *n, char *ll_out, int ll_sz);

static int emit_addr_s(Codegen *cg, Node *n,
                        char *type_out, int type_sz,
                        char *addr_out, int addr_sz) {
    int r = emit_addr(cg, n, type_out, type_sz);
    if (r == -1) {
        strncpy(addr_out, type_out, (size_t)addr_sz-1);
        if (n->kind == ND_IDENT) {
            Sym *s = scope_find(cg->scope, n->sval);
            if (s) strncpy(type_out, s->ll, (size_t)type_sz-1);
        }
    } else {
        snprintf(addr_out, (size_t)addr_sz, "%%%d", r);
    }
    return r;
}

static int emit_addr(Codegen *cg, Node *n, char *ll_out, int ll_sz) {
    if (!n) { strncpy(ll_out,"i32",(size_t)ll_sz-1); return 0; }

    if (n->kind==ND_IDENT) {
        Sym *s=scope_find(cg->scope,n->sval);
        if(!s){fprintf(stderr,"undef: %s\n",n->sval);
               strncpy(ll_out,"i32",(size_t)ll_sz-1); return 0;}
        strncpy(ll_out,s->ll,(size_t)ll_sz-1);
        if(s->is_global){
            /* global: encode "@gname" in ll_out, return -1 as sentinel */
            snprintf(ll_out,(size_t)ll_sz,"@%s",s->gname);
            return -1;
        }
        return s->ptr_reg;
    }

    if (n->kind==ND_UNARY_PRE && n->sval && !strcmp(n->sval,"*")) {
        /* *ptr → address is value of ptr */
        strncpy(ll_out,"i32",(size_t)ll_sz-1);
        return emit_expr(cg,n->left);
    }

    if (n->kind==ND_INDEX) {
        char base_ll[64]="i32";
        int arr_r;
        /* get array base address */
        if (n->left->kind==ND_IDENT) {
            Sym *s=scope_find(cg->scope,n->left->sval);
            if(s){
                strncpy(base_ll,s->ll,sizeof(base_ll)-1);
                arr_r=s->ptr_reg;
                /* If it's an array alloca ([N x T]), base_ll is elem type */
                /* We need to detect this case */
            } else { arr_r=0; }
        } else {
            arr_r=emit_expr(cg,n->left);
        }
        int idx_r=emit_expr(cg,n->right);
        /* sign-extend index to i64 */
        int idx64=R(cg);
        W(cg,"  %%%d = sext i32 %%%d to i64\n",idx64,idx_r);
        int r=R(cg);
        /* try to detect array alloca: look up symbol's alloca type */
        if (n->left->kind==ND_IDENT) {
            Sym *s=scope_find(cg->scope,n->left->sval);
            if(s){
                /* check if this sym was declared as array */
                /* We don't track this; heuristic: if ll is base element type */
                /* Actually, node_to_ll for arrays returns elem type now */
                /* Just use GEP with [? x T] - we don't know N here */
                /* Use i64 indexing: getelementptr T, ptr %reg, i64 %idx */
                W(cg,"  %%%d = getelementptr %s, ptr %%%d, i64 %%%d\n",
                  r,s->ll,arr_r,idx64);
                strncpy(ll_out,s->ll,(size_t)ll_sz-1);
                return r;
            }
        }
        W(cg,"  %%%d = getelementptr i32, ptr %%%d, i64 %%%d\n",r,arr_r,idx64);
        strncpy(ll_out,"i32",(size_t)ll_sz-1);
        return r;
    }

    /* member / arrow */
    if (n->kind==ND_MEMBER || n->kind==ND_ARROW) {
        int base_r;
        char struct_ll[64]="";

        if (n->kind==ND_MEMBER) {
            char tmp[64];
            int ar=emit_addr(cg,n->left,tmp,sizeof(tmp));
            /* find struct type */
            if(n->left->kind==ND_IDENT){
                Sym *s=scope_find(cg->scope,n->left->sval);
                if(s) strncpy(struct_ll,s->ll,sizeof(struct_ll)-1);
            }
            base_r=ar;
            if(ar==-1){
                /* global: load address */
                base_r=R(cg);
                W(cg,"  %%%d = load ptr, ptr %s, align 8\n",base_r,tmp);
                /* Actually for member access on global struct, address IS @name */
                /* We need the global's address, not its value */
                /* Re-think: for member, base is the struct address */
                /* For global struct g, &g == @g */
                /* emit_addr returns -1 with ll_out="@gname" */
                /* For struct member: GEP @gname, 0, field_idx */
                /* Recover gname from tmp */
                char sll[64];
                if(n->left->kind==ND_IDENT){
                    Sym *s2=scope_find(cg->scope,n->left->sval);
                    if(s2) strncpy(sll,s2->ll,sizeof(sll)-1);
                    else strncpy(sll,"i32",sizeof(sll)-1);
                }
                /* find tag */
                const char *tag=NULL;
                if(!strncmp(struct_ll,"%struct.",8)) tag=struct_ll+8;
                char fll[64]="i32"; int fidx=0;
                if(tag && n->sval) fidx=sf_field(tag,n->sval,fll,sizeof(fll));
                int gr=R(cg);
                W(cg,"  %%%d = getelementptr inbounds %s, ptr %s, i32 0, i32 %d\n",
                  gr,struct_ll,tmp,fidx);
                strncpy(ll_out,fll,(size_t)ll_sz-1);
                return gr;
            }
        } else {
            /* arrow: left is a pointer; load its value to get the struct ptr */
            int pr=emit_expr(cg,n->left);
            base_r=pr;
            /* infer struct type from ptr_target */
            if(n->left->kind==ND_IDENT){
                Sym *s=scope_find(cg->scope,n->left->sval);
                if(s && s->ptr_target[0])
                    strncpy(struct_ll,s->ptr_target,sizeof(struct_ll)-1);
                else if(s)
                    strncpy(struct_ll,s->ll,sizeof(struct_ll)-1);
            }
            /* fallback: scan sf_table by field name */
            if(!struct_ll[0] || !strcmp(struct_ll,"ptr")){
                for(int i=0;i<sf_count;i++){
                    for(int j=0;j<sf_table[i].nmem;j++){
                        if(n->sval && !strcmp(sf_table[i].mem[j].name,n->sval)){
                            snprintf(struct_ll,sizeof(struct_ll),
                                     "%%struct.%.54s",sf_table[i].tag);
                            goto found;
                        }
                    }
                }
                found:;
            }
        }

        const char *tag=NULL;
        if(!strncmp(struct_ll,"%struct.",8)) tag=struct_ll+8;
        else if(!strncmp(struct_ll,"%union.",7)) tag=struct_ll+7;

        char fll[64]="i32"; int fidx=0;
        if(tag && n->sval) fidx=sf_field(tag,n->sval,fll,sizeof(fll));

        int r=R(cg);
        W(cg,"  %%%d = getelementptr inbounds %s, ptr %%%d, i32 0, i32 %d\n",
          r,struct_ll,base_r,fidx);
        strncpy(ll_out,fll,(size_t)ll_sz-1);
        return r;
    }

    /* fallback */
    int r=emit_expr(cg,n); strncpy(ll_out,"i32",(size_t)ll_sz-1); return r;
}

/* ── expression emission ──────────────────────────────────────────────────── */
static int emit_expr(Codegen *cg, Node *n) {
    if (!n) { int r=R(cg); W(cg,"  %%%d = add nsw i32 0, 0\n",r); return r; }

    switch(n->kind) {
        case ND_INT_LIT: {
            int r=R(cg);
            W(cg,"  %%%d = add nsw i32 0, %lld\n",r,n->ival); return r;
        }
        case ND_CHAR_LIT: {
            int val=0; const char *p=n->sval;
            if(p&&*p=='\'') p++;
            if(p&&*p=='\\'){p++;
                switch(*p){case 'n':val='\n';break;case 't':val='\t';break;
                    case 'r':val='\r';break;case '0':val=0;break;
                    default:val=(unsigned char)*p;}
            } else if(p) val=(unsigned char)*p;
            int r=R(cg); W(cg,"  %%%d = add nsw i32 0, %d\n",r,val); return r;
        }
        case ND_FLOAT_LIT: {
            int r=R(cg); W(cg,"  %%%d = fadd double 0.0, %s\n",r,n->sval); return r;
        }
        case ND_STR_LIT: {
            int idx=slit_intern(n->sval);
            char buf[4096]; int len=parse_cstr(n->sval,buf,sizeof(buf));
            int r=R(cg);
            if(idx==0) W(cg,"  %%%d = getelementptr [%d x i8], ptr @.str, i32 0, i32 0\n",r,len);
            else       W(cg,"  %%%d = getelementptr [%d x i8], ptr @.str.%d, i32 0, i32 0\n",r,len,idx);
            return r;
        }

        case ND_IDENT: {
            Sym *s=scope_find(cg->scope,n->sval);
            if(!s){fprintf(stderr,"undef: %s\n",n->sval);
                   int r=R(cg);W(cg,"  %%%d = add nsw i32 0, 0\n",r);return r;}
            int r=R(cg); int al=ll_align(s->ll);
            if(s->is_global)
                W(cg,"  %%%d = load %s, ptr @%s, align %d\n",r,s->ll,s->gname,al);
            else
                W(cg,"  %%%d = load %s, ptr %%%d, align %d\n",r,s->ll,s->ptr_reg,al);
            return r;
        }

        case ND_BINARY: {
            int l=emit_expr(cg,n->left);
            int rv=emit_expr(cg,n->right);
            int r=R(cg);
            const char *op=n->sval;
            if      (!strcmp(op,"+"))  W(cg,"  %%%d = add nsw i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"-"))  W(cg,"  %%%d = sub nsw i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"*"))  W(cg,"  %%%d = mul nsw i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"/"))  W(cg,"  %%%d = sdiv i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"%"))  W(cg,"  %%%d = srem i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"&"))  W(cg,"  %%%d = and i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"|"))  W(cg,"  %%%d = or i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"^"))  W(cg,"  %%%d = xor i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"<<")) W(cg,"  %%%d = shl i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,">>")) W(cg,"  %%%d = ashr i32 %%%d, %%%d\n",r,l,rv);
            else if (!strcmp(op,"==")) {
                W(cg,"  %%%d = icmp eq i32 %%%d, %%%d\n",r,l,rv);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            else if (!strcmp(op,"!=")) {
                W(cg,"  %%%d = icmp ne i32 %%%d, %%%d\n",r,l,rv);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            else if (!strcmp(op,"<")) {
                W(cg,"  %%%d = icmp slt i32 %%%d, %%%d\n",r,l,rv);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            else if (!strcmp(op,">")) {
                W(cg,"  %%%d = icmp sgt i32 %%%d, %%%d\n",r,l,rv);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            else if (!strcmp(op,"<=")) {
                W(cg,"  %%%d = icmp sle i32 %%%d, %%%d\n",r,l,rv);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            else if (!strcmp(op,">=")) {
                W(cg,"  %%%d = icmp sge i32 %%%d, %%%d\n",r,l,rv);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            else if (!strcmp(op,"&&")||!strcmp(op,"||")) {
                /* simplified: treat as bitwise */
                if(!strcmp(op,"&&")) W(cg,"  %%%d = and i32 %%%d, %%%d\n",r,l,rv);
                else                 W(cg,"  %%%d = or  i32 %%%d, %%%d\n",r,l,rv);
            }
            else W(cg,"  %%%d = add nsw i32 %%%d, %%%d\n",r,l,rv);
            return r;
        }

        case ND_UNARY_PRE: {
            const char *op=n->sval;
            if(!strcmp(op,"&")) {
                /* address-of: return the ptr reg */
                char ll[64]; int a=emit_addr(cg,n->left,ll,sizeof(ll));
                if(a==-1){
                    /* global: its address is a ptr constant, load into reg */
                    /* Actually @name IS a ptr; emit a GEP 0 to get a reg */
                    int r=R(cg);
                    W(cg,"  %%%d = getelementptr %s, ptr %s, i32 0\n",r,ll+1,ll);
                    /* ll here is "@name", not a type; fix: look up global type */
                    /* simplest: just return the alloca reg by storing @name in a temp */
                    /* Re-do: use a bitcast trick */
                    return r; /* approximate */
                }
                return a;
            }
            if(!strcmp(op,"*")) {
                int ptr=emit_expr(cg,n->left);
                int r=R(cg); W(cg,"  %%%d = load i32, ptr %%%d, align 4\n",r,ptr);
                return r;
            }
            if(!strcmp(op,"-")) {
                int v=emit_expr(cg,n->left);
                int r=R(cg); W(cg,"  %%%d = sub nsw i32 0, %%%d\n",r,v); return r;
            }
            if(!strcmp(op,"!")) {
                int v=emit_expr(cg,n->left);
                int r=R(cg); W(cg,"  %%%d = icmp eq i32 %%%d, 0\n",r,v);
                int r2=R(cg); W(cg,"  %%%d = zext i1 %%%d to i32\n",r2,r); return r2;
            }
            if(!strcmp(op,"++")||!strcmp(op,"--")) {
                char ll[64],astr[64];
                emit_addr_s(cg,n->left,ll,sizeof(ll),astr,sizeof(astr));
                int al=ll_align(ll);
                int old=R(cg);
                W(cg,"  %%%d = load %s, ptr %s, align %d\n",old,ll,astr,al);
                int nv=R(cg);
                if(!strcmp(op,"++")) W(cg,"  %%%d = add nsw i32 %%%d, 1\n",nv,old);
                else                 W(cg,"  %%%d = sub nsw i32 %%%d, 1\n",nv,old);
                W(cg,"  store %s %%%d, ptr %s, align %d\n",ll,nv,astr,al);
                return nv;
            }
            return emit_expr(cg,n->left);
        }

        case ND_UNARY_POST: {
            char ll[64],astr[64];
            emit_addr_s(cg,n->left,ll,sizeof(ll),astr,sizeof(astr));
            int al=ll_align(ll);
            int old=R(cg);
            W(cg,"  %%%d = load %s, ptr %s, align %d\n",old,ll,astr,al);
            if(!strcmp(n->sval,"++")||!strcmp(n->sval,"--")) {
                int nv=R(cg);
                if(!strcmp(n->sval,"++")) W(cg,"  %%%d = add nsw i32 %%%d, 1\n",nv,old);
                else                      W(cg,"  %%%d = sub nsw i32 %%%d, 1\n",nv,old);
                W(cg,"  store %s %%%d, ptr %s, align %d\n",ll,nv,astr,al);
            }
            return old;
        }

        case ND_ASSIGN: {
            const char *op=n->sval;
            char ll[64], astr[64];
            int a=emit_addr_s(cg,n->left,ll,sizeof(ll),astr,sizeof(astr));
            int al=ll_align(ll);
            int val;
            int val_is_null = 0; /* special case: storing ptr null */
            if(!strcmp(op,"=")) {
                /* If storing integer 0 into a ptr slot, emit null directly */
                if(!strcmp(ll,"ptr") && n->right &&
                   n->right->kind==ND_INT_LIT && n->right->ival==0) {
                    W(cg,"  store ptr null, ptr %s, align %d\n",astr,al);
                    (void)a;
                    /* return a dummy reg so callers have something */
                    val=R(cg);
                    W(cg,"  %%%d = add nsw i32 0, 0\n",val);
                    return val;
                }
                val=emit_expr(cg,n->right);
            } else {
                int old=R(cg);
                W(cg,"  %%%d = load %s, ptr %s, align %d\n",old,ll,astr,al);
                int rv=emit_expr(cg,n->right);
                val=R(cg);
                if      (!strcmp(op,"+=")) W(cg,"  %%%d = add nsw i32 %%%d, %%%d\n",val,old,rv);
                else if (!strcmp(op,"-=")) W(cg,"  %%%d = sub nsw i32 %%%d, %%%d\n",val,old,rv);
                else if (!strcmp(op,"*=")) W(cg,"  %%%d = mul nsw i32 %%%d, %%%d\n",val,old,rv);
                else if (!strcmp(op,"/=")) W(cg,"  %%%d = sdiv i32 %%%d, %%%d\n",val,old,rv);
                else                       W(cg,"  %%%d = add nsw i32 %%%d, %%%d\n",val,old,rv);
            }
            /* If storing a ptr-type alloca reg into ptr field, emit as ptr */
            const char *store_type = ll;
            (void)val_is_null;
            W(cg,"  store %s %%%d, ptr %s, align %d\n",store_type,val,astr,al);
            (void)a;
            return val;
        }

        case ND_CALL: {
            /* collect arg values and their types */
            int args[64]; char arg_types[64][64]; int na=0;
            for(NodeList *c=n->children;c&&na<64;c=c->next){
                Node *arg=c->node;
                args[na]=emit_expr(cg,arg);
                /* infer arg type: if it's an ident with ptr type, use ptr */
                strncpy(arg_types[na],"i32",63);
                if(arg->kind==ND_IDENT){
                    Sym *s=scope_find(cg->scope,arg->sval);
                    if(s && !strcmp(s->ll,"ptr")) strncpy(arg_types[na],"ptr",63);
                } else if(arg->kind==ND_UNARY_PRE && arg->sval
                          && !strcmp(arg->sval,"&")) {
                    strncpy(arg_types[na],"ptr",63);
                }
                na++;
            }
            const char *fn="unknown";
            if(n->left&&n->left->kind==ND_IDENT) fn=n->left->sval;
            /* determine return type: check if fn is in scope */
            char ret_t[64]; strncpy(ret_t,cg->ret_ll,sizeof(ret_t)-1);
            int r=R(cg);
            W(cg,"  %%%d = call %s @%s(",r,ret_t,fn);
            for(int i=0;i<na;i++){
                if(i) W(cg,", ");
                W(cg,"%s noundef %%%d",arg_types[i],args[i]);
            }
            W(cg,")\n");
            return r;
        }

        case ND_MEMBER: case ND_ARROW: {
            char ll[64];
            int a=emit_addr(cg,n,ll,sizeof(ll));
            int r=R(cg); int al=ll_align(ll);
            if(a==-1) W(cg,"  %%%d = load %s, ptr %s, align %d\n",r,ll,ll,al);
            else      W(cg,"  %%%d = load %s, ptr %%%d, align %d\n",r,ll,a,al);
            return r;
        }

        case ND_INDEX: {
            char ll[64];
            int a=emit_addr(cg,n,ll,sizeof(ll));
            int r=R(cg); int al=ll_align(ll);
            W(cg,"  %%%d = load %s, ptr %%%d, align %d\n",r,ll,a,al);
            return r;
        }

        case ND_CAST:
            return emit_expr(cg,n->right);

        case ND_SIZEOF_TYPE: case ND_SIZEOF_EXPR: {
            int r=R(cg); W(cg,"  %%%d = add nsw i32 0, 4\n",r); return r;
        }

        case ND_TERNARY: {
            int id=cg->lbl++;
            char lt[32],lf[32],le[32];
            snprintf(lt,32,"tern.true.%d",id);
            snprintf(lf,32,"tern.false.%d",id);
            snprintf(le,32,"tern.end.%d",id);
            int cr=emit_expr(cg,n->left);
            int cb=R(cg);
            W(cg,"  %%%d = icmp ne i32 %%%d, 0\n",cb,cr);
            W(cg,"  br i1 %%%d, label %%%s, label %%%s\n",cb,lt,lf);
            W_label(cg,lt);
            int tv=emit_expr(cg,n->right);
            W(cg,"  br label %%%s\n",le);
            W_label(cg,lf);
            int fv=emit_expr(cg,n->extra);
            W(cg,"  br label %%%s\n",le);
            W_label(cg,le);
            int r=R(cg);
            W(cg,"  %%%d = phi i32 [ %%%d, %%%s ], [ %%%d, %%%s ]\n",
              r,tv,lt,fv,lf);
            return r;
        }

        case ND_COMMA_EXPR:
            emit_expr(cg,n->left); return emit_expr(cg,n->right);

        default: {
            int r=R(cg); W(cg,"  %%%d = add nsw i32 0, 0\n",r); return r;
        }
    }
}

/* ── statement emission ───────────────────────────────────────────────────── */
static void emit_stmt(Codegen *cg, Node *n);

static void emit_cond_br(Codegen *cg, Node *cond,
                          const char *tl, const char *fl) {
    int cv; int is_i1=0;

    if (cond->kind==ND_IDENT) {
        Sym *s=scope_find(cg->scope,cond->sval);
        if(s && !strcmp(s->ll,"ptr")) {
            int lr=R(cg);
            int al=ll_align(s->ll);
            W(cg,"  %%%d = load ptr, ptr %%%d, align %d\n",lr,s->ptr_reg,al);
            cv=R(cg);
            W(cg,"  %%%d = icmp ne ptr %%%d, null\n",cv,lr);
            is_i1=1;
        }
    }
    if(!is_i1) {
        int raw=emit_expr(cg,cond);
        cv=R(cg);
        W(cg,"  %%%d = icmp ne i32 %%%d, 0\n",cv,raw);
    }
    W(cg,"  br i1 %%%d, label %%%s, label %%%s\n",cv,tl,fl);
}

static void emit_stmt(Codegen *cg, Node *n) {
    if(!n) return;
    switch(n->kind) {
        case ND_EMPTY_STMT: break;
        case ND_EXPR_STMT: emit_expr(cg,n->left); break;

        case ND_DECL: {
            /* symbol already defined in scan pass; emit initializer if any */
            if(!n->sval || !*n->sval) break;
            Sym *s=scope_find(cg->scope,n->sval);
            if(!s) break;
            int al=ll_align(s->ll);
            if(n->right) {
                int iv=emit_expr(cg,n->right);
                W(cg,"  store %s %%%d, ptr %%%d, align %d\n",s->ll,iv,s->ptr_reg,al);
            }
            break;
        }

        case ND_COMPOUND:
            for(NodeList *c=n->children;c;c=c->next) emit_stmt(cg,c->node);
            break;

        case ND_RETURN: {
            if(strcmp(cg->ret_ll,"void")==0) {
                W(cg,"  br label %%%s\n",cg->exit_lbl_name);
            } else {
                int v=emit_expr(cg,n->left);
                W(cg,"  store %s %%%d, ptr %%%d, align 4\n",
                  cg->ret_ll,v,cg->ret_alloca);
                W(cg,"  br label %%%s\n",cg->exit_lbl_name);
            }
            /* dead code marker: LLVM needs the next "block" to be reachable
               or terminated; emit an unreachable marker via a dummy label */
            W(cg,"\n");
            break;
        }

        case ND_IF: {
            int id = cg->lbl++;
            char sl_then[32], sl_else[32], sl_end[32];
            snprintf(sl_then, 32, "if.then.%d", id);
            snprintf(sl_else, 32, "if.else.%d", id);
            snprintf(sl_end,  32, "if.end.%d",  id);

            emit_cond_br(cg, n->left,
                         sl_then, n->extra ? sl_else : sl_end);

            W_label(cg, sl_then);
            emit_stmt(cg, n->right);
            W(cg,"  br label %%%s\n", sl_end);

            if(n->extra) {
                W_label(cg, sl_else);
                emit_stmt(cg, n->extra);
                W(cg,"  br label %%%s\n", sl_end);
            }
            W_label(cg, sl_end);
            break;
        }

        case ND_WHILE: {
            int id = cg->lbl++;
            char sc[32], sb[32], se[32];
            snprintf(sc, 32, "while.cond.%d", id);
            snprintf(sb, 32, "while.body.%d", id);
            snprintf(se, 32, "while.end.%d",  id);
            W(cg,"  br label %%%s\n", sc);
            W_label(cg, sc);
            emit_cond_br(cg, n->left, sb, se);
            W_label(cg, sb);
            emit_stmt(cg, n->right);
            W(cg,"  br label %%%s\n", sc);
            W_label(cg, se);
            break;
        }

        case ND_FOR: {
            int id = cg->lbl++;
            char sc[32], sb[32], ss[32], se[32];
            snprintf(sc, 32, "for.cond.%d", id);
            snprintf(sb, 32, "for.body.%d", id);
            snprintf(ss, 32, "for.step.%d", id);
            snprintf(se, 32, "for.end.%d",  id);
            if(n->left) {
                if(n->left->kind==ND_DECL) emit_stmt(cg,n->left);
                else emit_expr(cg,n->left);
            }
            W(cg,"  br label %%%s\n", sc);
            W_label(cg, sc);
            if(n->right) emit_cond_br(cg, n->right, sb, se);
            else         W(cg,"  br label %%%s\n", sb);
            W_label(cg, sb);
            if(n->children) emit_stmt(cg, n->children->node);
            W(cg,"  br label %%%s\n", ss);
            W_label(cg, ss);
            if(n->extra) emit_expr(cg, n->extra);
            W(cg,"  br label %%%s\n", sc);
            W_label(cg, se);
            break;
        }

        case ND_DO_WHILE: {
            int id = cg->lbl++;
            char sb[32], sc[32], se[32];
            snprintf(sb, 32, "do.body.%d", id);
            snprintf(sc, 32, "do.cond.%d", id);
            snprintf(se, 32, "do.end.%d",  id);
            W(cg,"  br label %%%s\n", sb);
            W_label(cg, sb);
            emit_stmt(cg, n->left);
            W(cg,"  br label %%%s\n", sc);
            W_label(cg, sc);
            emit_cond_br(cg, n->right, sb, se);
            W_label(cg, se);
            break;
        }

        case ND_LABEL:
            W_label(cg,n->sval);
            emit_stmt(cg,n->left);
            break;

        case ND_GOTO:
            W(cg,"  br label %%%s\n",n->sval);
            break;

        default:
            if(n->left) emit_expr(cg,n->left);
            break;
    }
}

/* ── function emission ────────────────────────────────────────────────────── */
static void emit_func(Codegen *cg, Node *fn) {
    /* determine return type */
    char ret[64]="i32";
    if(fn->left) node_to_ll(fn->left,ret,sizeof(ret));
    strncpy(cg->ret_ll,ret,sizeof(cg->ret_ll)-1);

    /* count params */
    int np=0;
    for(NodeList *c=fn->children;c;c=c->next)
        if(!c->node->is_ellipsis) np++;

    /* emit function signature */
    fprintf(cg->out,"define dso_local %s @%s(",ret,fn->sval?fn->sval:"f");
    int pi=0;
    for(NodeList *c=fn->children;c;c=c->next) {
        Node *par=c->node;
        if(par->is_ellipsis){if(pi)fprintf(cg->out,", ");fprintf(cg->out,"...");continue;}
        char pll[64]="i32";
        if(par->left) node_to_ll(par->left,pll,sizeof(pll));
        if(pi) fprintf(cg->out,", ");
        fprintf(cg->out,"%s noundef %%%d",pll,pi++);
    }
    fprintf(cg->out,") #0 {\n");

    /* --- alloca pass --- */
    /* reg numbering:
       params occupy %0..%np-1 (implicit, already named in signature)
       %np is the entry block number
       allocas start at %(np+1) for retval, then params, then locals */
    cg->reg = np + 1; /* skip param regs; +1 because %np is the entry "label" */
    /* Actually clang's entry block is unnamed (it IS %0 in SSA sense but the
       first instruction's number reveals the true start).
       In practice: alloca starts at %1 for no-param functions, or %np+1 */
    /* Let's match clang exactly: alloca starts at %np+1 if np>0, else %1 */
    if(np == 0) cg->reg = 1;
    else        cg->reg = np + 1;

    /* alloca buffer */
    cg->to_buf=1; cg->bpos=0;
    if(!cg->buf){ cg->bcap=65536; cg->buf=malloc((size_t)cg->bcap); }
    cg->buf[0]=0;

    /* retval alloca (always, for non-void) */
    int retval_reg = -1;
    if(strcmp(ret,"void")!=0) {
        retval_reg = cg->reg++;
        W(cg,"  %%%d = alloca %s, align 4\n",retval_reg,ret);
    }
    cg->ret_alloca = retval_reg;

    /* push scope for this function */
    cg->scope = scope_push(cg->scope);

    /* param allocas */
    pi=0;
    for(NodeList *c=fn->children;c;c=c->next) {
        Node *par=c->node;
        if(par->is_ellipsis) continue;
        char pll[64]="i32";
        if(par->left) node_to_ll(par->left,pll,sizeof(pll));
        int al=ll_align(pll);
        int pr=cg->reg++;
        W(cg,"  %%%d = alloca %s, align %d\n",pr,pll,al);
        if(par->sval && *par->sval) {
            scope_def(cg->scope,par->sval,pll,pr);
            /* set ptr_target for struct pointer params */
            if(par->left && par->left->kind==ND_TYPE_PTR) {
                Node *inner=par->left->left;
                if(inner && (inner->kind==ND_TYPE_STRUCT||inner->kind==ND_TYPE_UNION)
                   && inner->sval) {
                    Sym *s=scope_find(cg->scope,par->sval);
                    if(s) snprintf(s->ptr_target,sizeof(s->ptr_target),
                                   "%%struct.%s",inner->sval);
                }
            }
        }
        pi++;
    }

    /* local var allocas (scan body) */
    if(fn->right) scan_decls(cg, fn->right);

    /* --- body buffer (append to same buf after alloca) --- */
    /* retval init store */
    if(strcmp(ret,"void")!=0)
        W(cg,"  store %s 0, ptr %%%d, align 4\n",ret,retval_reg);

    /* param stores */
    pi=0;
    for(NodeList *c=fn->children;c;c=c->next) {
        Node *par=c->node;
        if(par->is_ellipsis) continue;
        char pll[64]="i32";
        if(par->left) node_to_ll(par->left,pll,sizeof(pll));
        int al=ll_align(pll);
        Sym *s=par->sval?scope_find(cg->scope,par->sval):NULL;
        if(s) W(cg,"  store %s %%%d, ptr %%%d, align %d\n",pll,pi,s->ptr_reg,al);
        pi++;
    }

    /* exit label is a NAMED label; it consumes one slot in LLVM's value table */
    strncpy(cg->exit_lbl_name, "return", sizeof(cg->exit_lbl_name)-1);

    /* body */
    int has_terminator = 0; /* track if last stmt was a br/ret */
    if(fn->right) {
        for(NodeList *c=fn->right->children;c;c=c->next)
            emit_stmt(cg,c->node);
    }

    /* fall-through br (only if body doesn't end with return) */
    /* Always emit it; LLVM will validate it's dead code */
    W(cg,"  br label %%%s\n", cg->exit_lbl_name);
    (void)has_terminator;

    /* exit block: named label counts as one value slot */
    cg->reg++; /* consume the slot for "return:" label */
    W_label(cg, cg->exit_lbl_name);
    if(strcmp(ret,"void")==0) {
        W(cg,"  ret void\n");
    } else {
        int lv=cg->reg++;
        W(cg,"  %%%d = load %s, ptr %%%d, align 4\n",lv,ret,retval_reg);
        W(cg,"  ret %s %%%d\n",ret,lv);
    }

    cg->scope = scope_pop(cg->scope);
    cg->to_buf=0;

    fwrite(cg->buf,(size_t)cg->bpos,1,cg->out);
    fprintf(cg->out,"}\n\n");
}

/* ── global variable ──────────────────────────────────────────────────────── */
static void emit_global_var(Codegen *cg, Node *n) {
    if(!n->sval||!*n->sval) return;
    char ll[64]="i32"; node_to_ll(n->left,ll,sizeof(ll));
    int al=n->left?node_align(n->left):4;
    const char *link = n->is_static ? "internal" : "dso_local global";
    if(n->is_extern){
        fprintf(cg->out,"@%s = external global %s, align %d\n\n",n->sval,ll,al);
    } else if(n->right && n->right->kind==ND_INT_LIT){
        fprintf(cg->out,"@%s = %s %s %lld, align %d\n\n",
                n->sval,link,ll,n->right->ival,al);
    } else {
        fprintf(cg->out,"@%s = %s %s zeroinitializer, align %d\n\n",
                n->sval,link,ll,al);
    }
    scope_def_global(cg->scope,n->sval,ll,n->sval);
}

/* ── prescan helper ───────────────────────────────────────────────────────── */
static char ps_emitted[MAX_SF][64];
static int  ps_ne = 0;

static void scan_type(FILE *out, Node *t) {
    if (!t) return;
    if ((t->kind==ND_TYPE_STRUCT||t->kind==ND_TYPE_UNION) && t->sval && t->children) {
        for (int i=0;i<ps_ne;i++) if (!strcmp(ps_emitted[i],t->sval)) return;
        if (ps_ne < MAX_SF) strncpy(ps_emitted[ps_ne++], t->sval, 63);
        sf_register(t->sval, t);
        fprintf(out,"%%struct.%s = type { ", t->sval);
        int first=1;
        for (NodeList *mc=t->children; mc; mc=mc->next) {
            Node *m=mc->node; char mll[64]; node_to_ll(m->left,mll,sizeof(mll));
            if (!first) fprintf(out,", ");
            fprintf(out,"%s",mll); first=0;
        }
        fprintf(out," }\n\n");
    }
}

/* ── prescan: collect struct types ────────────────────────────────────────── */
static void prescan(Codegen *cg, Node *root) {
    ps_ne = 0; /* reset per translation unit */
    for (NodeList *c=root->children; c; c=c->next) {
        Node *e=c->node;
        if (e->kind==ND_DECL) scan_type(cg->out, e->left);
        if (e->kind==ND_FUNC_DEF) {
            scan_type(cg->out, e->left);
            for (NodeList *pc=e->children; pc; pc=pc->next)
                if (pc->node->left) scan_type(cg->out, pc->node->left);
            if (e->right)
                for (NodeList *bc=e->right->children; bc; bc=bc->next) {
                    Node *s=bc->node;
                    if (s->kind==ND_DECL) scan_type(cg->out, s->left);
                }
        }
    }
}

/* ── top-level ────────────────────────────────────────────────────────────── */
void codegen_emit(Codegen *cg, Node *root) {
    assert(root && root->kind==ND_TRANSLATION_UNIT);

    fprintf(cg->out,"; ModuleID = '%s'\n",cg->src);
    fprintf(cg->out,"source_filename = \"%s\"\n",cg->src);
    const char *triple = cg->target ? cg->target : "x86_64-pc-linux-gnu";
    fprintf(cg->out,"target datalayout = \"e-m:e-p270:32:32-p271:32:32-p272:64:64"
            "-i64:64-i128:128-f80:128-n8:16:32:64-S128\"\n");
    fprintf(cg->out,"target triple = \"%s\"\n\n", triple);

    prescan(cg,root);

    cg->scope = scope_push(NULL);

    /* globals */
    for(NodeList *c=root->children;c;c=c->next){
        Node *e=c->node;
        if(e->kind==ND_DECL) emit_global_var(cg,e);
    }

    /* string literals – will be emitted after functions */

    /* functions */
    for(NodeList *c=root->children;c;c=c->next){
        Node *e=c->node;
        if(e->kind==ND_FUNC_DEF) emit_func(cg,e);
    }

    /* string literals */
    for(int i=0;i<nslits;i++){
        char buf[4096]; int len=parse_cstr(slits[i].tok,buf,sizeof(buf));
        if(i==0) fprintf(cg->out,"@.str = private unnamed_addr constant [%d x i8] c\"",len);
        else     fprintf(cg->out,"@.str.%d = private unnamed_addr constant [%d x i8] c\"",i,len);
        for(int j=0;j<len;j++){
            unsigned char ch=(unsigned char)buf[j];
            if(ch=='"'||ch=='\\'||ch<0x20||ch>=0x7f) fprintf(cg->out,"\\%02X",ch);
            else fprintf(cg->out,"%c",ch);
        }
        fprintf(cg->out,"\", align 1\n");
    }
    if(nslits) fprintf(cg->out,"\n");

    fprintf(cg->out,"attributes #0 = { noinline nounwind optnone uwtable }\n");

    cg->scope = scope_pop(cg->scope);
}

Codegen *codegen_new(FILE *out, const char *src, const char *target){
    Codegen *cg=calloc(1,sizeof(Codegen));
    cg->out=out; cg->src=src; cg->target=target;
    return cg;
}
void codegen_free(Codegen *cg){
    if(!cg) return;
    free(cg->buf);
    while(cg->scope) cg->scope=scope_pop(cg->scope);
    for(int i=0;i<nslits;i++) free(slits[i].tok);
    nslits=0;
    sf_count=0;
    free(cg);
}
