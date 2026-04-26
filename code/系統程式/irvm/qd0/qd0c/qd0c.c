/*
 * qd0c.c — DynIR 四元組 → LLVM IR 編譯器
 *
 * 使用方式：
 *   qd0c xxx.qd            => 產生 xxx.ll
 *   clang xxx.ll qd0lib.c -o xxx -lm
 *
 * 四元組格式（每行一條指令，空白分隔，_ 表示不使用欄位）：
 *   OP  ARG1  ARG2  RESULT
 *
 * 策略：
 *   所有動態值在 LLVM IR 層均以 %QdObj* 表示。
 *   算術/比較/屬性/呼叫等操作均對應到 qd0lib 的 C 函式。
 *   Control flow (JUMP/BRANCH_x/LABEL) maps to LLVM basic blocks.
 *   ARG_PUSH + CALL pass args via a local alloca array to qd_call().
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ================================================================== */
/*  常數設定                                                            */
/* ================================================================== */
#define MAX_LINE    4096
#define MAX_TOKENS     8
#define MAX_INSTRS 65536
#define MAX_REGS    4096
#define MAX_LABELS  2048
#define MAX_ARGS      64
#define MAX_STR     4096

/* ================================================================== */
/*  四元組                                                              */
/* ================================================================== */
typedef struct {
    char op[64];
    char arg1[512];
    char arg2[512];
    char result[256];
    int  lineno;
} Quad;

static Quad quads[MAX_INSTRS];
static int  nquads = 0;

/* ================================================================== */
/*  暫存器 / slot 表（qd 名 => LLVM alloca 名）                       */
/* ================================================================== */
typedef struct { char qd[256]; char ll[256]; int is_iter; } RegSlot;
static RegSlot reg_slots[MAX_REGS];
static int     nreg_slots = 0;

static const char *slot_get(const char *qd) {
    for (int i = 0; i < nreg_slots; i++)
        if (strcmp(reg_slots[i].qd, qd) == 0) return reg_slots[i].ll;
    return NULL;
}

static void slot_def(const char *qd, const char *ll, int is_iter) {
    for (int i = 0; i < nreg_slots; i++) {
        if (strcmp(reg_slots[i].qd, qd) == 0) {
            strncpy(reg_slots[i].ll, ll, 255);
            reg_slots[i].is_iter = is_iter;
            return;
        }
    }
    if (nreg_slots >= MAX_REGS) { fprintf(stderr,"slot overflow\n"); exit(1); }
    strncpy(reg_slots[nreg_slots].qd, qd, 255);
    strncpy(reg_slots[nreg_slots].ll, ll, 255);
    reg_slots[nreg_slots].is_iter = is_iter;
    nreg_slots++;
}

/* ================================================================== */
/*  字串常數池                                                          */
/* ================================================================== */
typedef struct { char raw[1024]; char gname[64]; int blen; } StrEntry;
static StrEntry str_pool[MAX_STR];
static int      nstr = 0;

/* 展開 C escape，回傳 byte 數（含 NUL） */
static int unescape(const char *src, char *dst, int dstsz) {
    int d = 0;
    for (const char *s = src; *s && d < dstsz-1; s++) {
        if (*s == '\\') {
            s++;
            switch (*s) {
                case 'n':  dst[d++]='\n'; break;
                case 't':  dst[d++]='\t'; break;
                case '\\': dst[d++]='\\'; break;
                case '"':  dst[d++]='"';  break;
                case '0':  dst[d++]='\0'; break;
                default:   dst[d++]=*s;
            }
        } else { dst[d++]=*s; }
    }
    dst[d++]='\0';
    return d;
}

/* intern 裸字串（不含引號），回傳 @.str.N */
static const char *intern_raw(const char *raw) {
    for (int i = 0; i < nstr; i++)
        if (strcmp(str_pool[i].raw, raw) == 0) return str_pool[i].gname;
    if (nstr >= MAX_STR) { fprintf(stderr,"string pool overflow\n"); exit(1); }
    strncpy(str_pool[nstr].raw, raw, 1023);
    snprintf(str_pool[nstr].gname, 63, "@.str.%d", nstr);
    char buf[1024];
    str_pool[nstr].blen = unescape(raw, buf, sizeof(buf));
    return str_pool[nstr++].gname;
}

/* intern 帶引號字串 token，回傳 @.str.N */
static const char *intern_quoted(const char *s) {
    int len = (int)strlen(s);
    char raw[1024] = {0};
    if (len >= 2) strncpy(raw, s+1, len-2 < 1023 ? len-2 : 1023);
    return intern_raw(raw);
}

/* ================================================================== */
/*  輔助判斷                                                            */
/* ================================================================== */
static int is_blank(const char *s)   { return !s||!*s||strcmp(s,"_")==0; }
static int is_int_lit(const char *s) {
    if (!s||!*s) return 0;
    const char *p=s; if(*p=='-'||*p=='+')p++;
    if(!*p) return 0;
    while(*p){if(!isdigit((unsigned char)*p))return 0;p++;} return 1;
}
static int is_float_lit(const char *s) {
    if (!s||!*s) return 0;
    const char *p=s; if(*p=='-'||*p=='+')p++;
    int dot=0,e=0;
    while(*p){
        if(*p=='.'){ if(dot)return 0; dot=1; p++; }
        else if(*p=='e'||*p=='E'){ if(e)return 0; e=1; p++; if(*p=='-'||*p=='+')p++; }
        else if(isdigit((unsigned char)*p)) p++;
        else return 0;
    }
    return dot||e;
}
static int is_str_lit(const char *s)  { int l=(int)strlen(s); return l>=2&&s[0]=='"'&&s[l-1]=='"'; }
static int is_name(const char *s) {
    if(is_blank(s)||is_int_lit(s)||is_float_lit(s)||is_str_lit(s)) return 0;
    if(strcmp(s,"None")==0||strcmp(s,"True")==0||strcmp(s,"False")==0) return 0;
    return 1;
}

/* 安全 LLVM identifier（替換非字母數字為 _）*/
static char safe_buf[256];
static const char *safe_id(const char *s) {
    strncpy(safe_buf, s, 255);
    for (char *p=safe_buf;*p;p++)
        if(!isalnum((unsigned char)*p)&&*p!='_') *p='_';
    return safe_buf;
}

/* ================================================================== */
/*  全域計數器（產生唯一 LLVM 暫存器）                                 */
/* ================================================================== */
static int gc = 0;
#define NEXT() (gc++)

/* ================================================================== */
/*  解析                                                                */
/* ================================================================== */
static int tokenize(char *line, char toks[][512], int maxn) {
    int n=0; char *p=line;
    while (*p && n<maxn) {
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p||*p=='#') break;
        if (*p=='"') {
            char *q=toks[n]; *q++=*p++;
            while (*p && !(*p=='"'&&*(p-1)!='\\')) *q++=*p++;
            if (*p=='"') *q++=*p++;
            *q='\0'; n++;
        } else {
            char *q=toks[n];
            while (*p&&!isspace((unsigned char)*p)&&*p!='#') *q++=*p++;
            *q='\0'; n++;
        }
    }
    return n;
}

static void parse_qd(FILE *fin) {
    char line[MAX_LINE]; char toks[MAX_TOKENS][512]; int ln=0;
    while (fgets(line,sizeof(line),fin)) {
        ln++;
        line[strcspn(line,"\r\n")]='\0';
        int n=tokenize(line,toks,MAX_TOKENS);
        if (n==0) continue;
        if (nquads>=MAX_INSTRS){fprintf(stderr,"too many instructions\n");exit(1);}
        Quad *q=&quads[nquads++];
        strncpy(q->op,    n>0?toks[0]:"_", 63);
        strncpy(q->arg1,  n>1?toks[1]:"_",511);
        strncpy(q->arg2,  n>2?toks[2]:"_",511);
        strncpy(q->result,n>3?toks[3]:"_",255);
        q->lineno=ln;
    }
}

/* ================================================================== */
/*  第一遍：收集所有 slot 和字串常數                                   */
/* ================================================================== */
typedef struct { char name[256]; int is_iter; } SlotInfo;
static SlotInfo pre_slots[MAX_REGS];
static int      npre_slots = 0;

static void pre_add_slot(const char *nm, int is_iter) {
    if (is_blank(nm)||!is_name(nm)) return;
    for (int i=0;i<npre_slots;i++)
        if (strcmp(pre_slots[i].name,nm)==0){
            if(is_iter) pre_slots[i].is_iter=1;
            return;
        }
    if (npre_slots>=MAX_REGS){fprintf(stderr,"pre_slots overflow\n");exit(1);}
    strncpy(pre_slots[npre_slots].name,nm,255);
    pre_slots[npre_slots].is_iter=is_iter;
    npre_slots++;
}

static void pre_add_str(const char *tok) {
    if (is_str_lit(tok)) intern_quoted(tok);
    else if (is_name(tok)) { char q[300]; snprintf(q,sizeof(q),"\"%s\"",tok); intern_quoted(q); }
}

static void prescan(void) {
    for (int i=0;i<nquads;i++) {
        Quad *q=&quads[i];
        /* slots */
        if (!is_blank(q->result)) pre_add_slot(q->result, strcmp(q->op,"GET_ITER")==0);
        /* strings */
        pre_add_str(q->arg1);
        pre_add_str(q->arg2);
        pre_add_str(q->result);
        if (strcmp(q->op,"LOAD_ATTR")==0||strcmp(q->op,"LOAD_METHOD")==0||
            strcmp(q->op,"STORE_ATTR")==0)
            pre_add_str(q->arg2);
        if (strcmp(q->op,"LOAD_NAME")==0||strcmp(q->op,"LOAD_GLOBAL")==0||
            strcmp(q->op,"STORE")==0||strcmp(q->op,"STORE_GLOBAL")==0||
            strcmp(q->op,"LOAD_FAST")==0||strcmp(q->op,"STORE_FAST")==0)
            pre_add_str(q->arg1[0]?q->arg1:q->result);
    }
}

/* ================================================================== */
/*  取得 i8* 指向字串常數（名稱，不含引號）                           */
/* ================================================================== */
static FILE *CF; /* current output file */

static char cs_buf[256];
static const char *emit_cstr(const char *raw) {
    const char *gn = intern_raw(raw);
    int idx=-1;
    for (int i=0;i<nstr;i++) if (strcmp(str_pool[i].gname,gn)==0){idx=i;break;}
    snprintf(cs_buf,sizeof(cs_buf),"%%_cs%d",NEXT());
    fprintf(CF,"  %s = getelementptr inbounds [%d x i8], [%d x i8]* %s, i32 0, i32 0\n",
        cs_buf, str_pool[idx].blen, str_pool[idx].blen, gn);
    return cs_buf;
}

/* ================================================================== */
/*  取得 %QdObj* 值（可能從字面值建構，或從 alloca slot 讀取）        */
/* ================================================================== */
static char vbuf[256];

static const char *emit_val(const char *tok) {
    snprintf(vbuf,sizeof(vbuf),"%%_v%d",NEXT());
    if (is_blank(tok)||strcmp(tok,"None")==0) {
        fprintf(CF,"  %s = call %%QdObj* @qd_none()\n",vbuf); return vbuf;
    }
    if (strcmp(tok,"True")==0) {
        fprintf(CF,"  %s = call %%QdObj* @qd_bool(i32 1)\n",vbuf); return vbuf;
    }
    if (strcmp(tok,"False")==0) {
        fprintf(CF,"  %s = call %%QdObj* @qd_bool(i32 0)\n",vbuf); return vbuf;
    }
    if (is_int_lit(tok)) {
        fprintf(CF,"  %s = call %%QdObj* @qd_int(i64 %s)\n",vbuf,tok); return vbuf;
    }
    if (is_float_lit(tok)) {
        fprintf(CF,"  %s = call %%QdObj* @qd_float(double %s)\n",vbuf,tok); return vbuf;
    }
    if (is_str_lit(tok)) {
        const char *gn = intern_quoted(tok);
        int idx=-1;
        for (int i=0;i<nstr;i++) if(strcmp(str_pool[i].gname,gn)==0){idx=i;break;}
        char gep[256]; snprintf(gep,sizeof(gep),"%%_gp%d",NEXT());
        fprintf(CF,"  %s = getelementptr inbounds [%d x i8], [%d x i8]* %s, i32 0, i32 0\n",
            gep,str_pool[idx].blen,str_pool[idx].blen,gn);
        fprintf(CF,"  %s = call %%QdObj* @qd_string(i8* %s)\n",vbuf,gep);
        return vbuf;
    }
    /* 變數名：從 slot 讀，若不存在則從環境讀 */
    const char *sl = slot_get(tok);
    if (sl) {
        fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n",vbuf,sl);
    } else {
        /* 環境讀取 */
        const char *cs = emit_cstr(tok);
        fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n",vbuf,cs);
    }
    return vbuf;
}

/* 取得 slot（已在 prescan 建立）*/
static const char *get_slot(const char *nm) {
    const char *s = slot_get(nm);
    if (!s) { fprintf(stderr,"undefined slot: %s\n",nm); exit(1); }
    return s;
}

/* ================================================================== */
/*  ARG buffer                                                          */
/* ================================================================== */
static char arg_vals[MAX_ARGS][256];
static int  arg_cnt = 0;

/* ================================================================== */
/*  主代碼生成                                                          */
/* ================================================================== */
static void emit_code(void) {
    int term = 0; /* 上條指令是否為 terminator */

    for (int i=0;i<nquads;i++) {
        Quad *q = &quads[i];
        const char *op = q->op;

        /* LABEL */
        if (strcmp(op,"LABEL")==0) {
            if (!term) fprintf(CF,"  br label %%%s\n",q->arg1);
            fprintf(CF,"\n%s:\n",q->arg1);
            term=0; continue;
        }

        /* NOP / COMMENT / DEBUG */
        if (strcmp(op,"NOP")==0) { term=0; continue; }
        if (strcmp(op,"COMMENT")==0||strcmp(op,"DEBUG_BREAKPOINT")==0) {
            fprintf(CF,"  ; %s %s %s %s\n",op,q->arg1,q->arg2,q->result);
            term=0; continue;
        }

        /* LOAD_CONST */
        if (strcmp(op,"LOAD_CONST")==0) {
            const char *v  = emit_val(q->arg1);
            const char *sl = get_slot(q->result);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",v,sl);
            term=0; continue;
        }

        /* LOAD_NAME / LOAD_GLOBAL */
        if (strcmp(op,"LOAD_NAME")==0||strcmp(op,"LOAD_GLOBAL")==0) {
            const char *cs = emit_cstr(q->arg1);
            char t[256]; snprintf(t,sizeof(t),"%%_ln%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n",t,cs);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
            term=0; continue;
        }

        /* LOAD_FAST */
        if (strcmp(op,"LOAD_FAST")==0) {
            const char *v = emit_val(q->arg1);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",v,get_slot(q->result));
            term=0; continue;
        }

        /* STORE */
        if (strcmp(op,"STORE")==0||strcmp(op,"STORE_FAST")==0) {
            const char *v  = emit_val(q->arg1);
            const char *sl = get_slot(q->result);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",v,sl);
            if (strcmp(op,"STORE")==0) {
                const char *cs = emit_cstr(q->result);
                fprintf(CF,"  call void @qd_env_set(i8* %s, %%QdObj* %s)\n",cs,v);
            }
            term=0; continue;
        }

        /* STORE_GLOBAL */
        if (strcmp(op,"STORE_GLOBAL")==0) {
            const char *v  = emit_val(q->arg1);
            const char *cs = emit_cstr(q->result);
            fprintf(CF,"  call void @qd_env_set(i8* %s, %%QdObj* %s)\n",cs,v);
            term=0; continue;
        }

        /* LOAD_ATTR / LOAD_METHOD */
        if (strcmp(op,"LOAD_ATTR")==0||strcmp(op,"LOAD_METHOD")==0) {
            const char *obj = emit_val(q->arg1);
            const char *cs  = emit_cstr(q->arg2);
            char t[256]; snprintf(t,sizeof(t),"%%_la%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_getattr(%%QdObj* %s, i8* %s)\n",t,obj,cs);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
            term=0; continue;
        }

        /* STORE_ATTR */
        if (strcmp(op,"STORE_ATTR")==0) {
            const char *obj = emit_val(q->arg1);
            const char *cs  = emit_cstr(q->arg2);
            const char *val = emit_val(q->result);
            fprintf(CF,"  call void @qd_setattr(%%QdObj* %s, i8* %s, %%QdObj* %s)\n",obj,cs,val);
            term=0; continue;
        }

        /* LOAD_SUBSCR */
        if (strcmp(op,"LOAD_SUBSCR")==0) {
            const char *obj = emit_val(q->arg1);
            const char *key = emit_val(q->arg2);
            char t[256]; snprintf(t,sizeof(t),"%%_ls%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_getitem(%%QdObj* %s, %%QdObj* %s)\n",t,obj,key);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
            term=0; continue;
        }

        /* STORE_SUBSCR */
        if (strcmp(op,"STORE_SUBSCR")==0) {
            const char *obj = emit_val(q->arg1);
            const char *key = emit_val(q->arg2);
            const char *val = emit_val(q->result);
            fprintf(CF,"  call void @qd_setitem(%%QdObj* %s, %%QdObj* %s, %%QdObj* %s)\n",obj,key,val);
            term=0; continue;
        }

        /* 二元算術 */
        {
            const char *fn=NULL;
            if      (strcmp(op,"ADD")==0||strcmp(op,"CONCAT")==0) fn="qd_add";
            else if (strcmp(op,"SUB")==0)      fn="qd_sub";
            else if (strcmp(op,"MUL")==0)      fn="qd_mul";
            else if (strcmp(op,"DIV")==0)      fn="qd_div";
            else if (strcmp(op,"FLOORDIV")==0) fn="qd_floordiv";
            else if (strcmp(op,"MOD")==0)      fn="qd_mod";
            else if (strcmp(op,"POW")==0)      fn="qd_pow";
            else if (strcmp(op,"BITAND")==0)   fn="qd_bitand";
            else if (strcmp(op,"BITOR")==0)    fn="qd_bitor";
            if (fn) {
                // Fix: emit_val uses static buffer, so we need to load both values BEFORE calling the function
                // Generate variable names for both args first
                int id1 = gc++;
                int id2 = gc++;
                char a_buf[64], b_buf[64];
                snprintf(a_buf, sizeof(a_buf), "%%_v%d", id1);
                snprintf(b_buf, sizeof(b_buf), "%%_v%d", id2);
                
                // Now emit the loads for both args using emit_val which will use id1 and id2
                // But emit_val also increments gc, so we need to call it in a way that works
                // Actually, let's just emit the loads manually based on the argument names
                const char *sl1 = slot_get(q->arg1);
                const char *sl2 = slot_get(q->arg2);
                if (sl1) {
                    fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n", a_buf, sl1);
                } else {
                    const char *cs1 = emit_cstr(q->arg1);
                    fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n", a_buf, cs1);
                }
                if (sl2) {
                    fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n", b_buf, sl2);
                } else {
                    const char *cs2 = emit_cstr(q->arg2);
                    fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n", b_buf, cs2);
                }
                
                char t[256]; snprintf(t,sizeof(t),"%%_ar%d",NEXT());
                fprintf(CF,"  %s = call %%QdObj* @%s(%%QdObj* %s, %%QdObj* %s)\n",t,fn,a_buf,b_buf);
                fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
                term=0; continue;
            }
        }

        /* 一元運算 */
        if (strcmp(op,"NEG")==0||strcmp(op,"NOT")==0||strcmp(op,"UNARY")==0) {
            const char *fn="qd_neg";
            const char *oprand=q->arg1;
            if (strcmp(op,"NOT")==0) fn="qd_not";
            if (strcmp(op,"UNARY")==0) {
                fn=(strcmp(q->arg1,"NOT")==0)?"qd_not":"qd_neg";
                oprand=q->arg2;
            }
            const char *a=emit_val(oprand);
            char t[256]; snprintf(t,sizeof(t),"%%_un%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @%s(%%QdObj* %s)\n",t,fn,a);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
            term=0; continue;
        }

        /* 比較 */
        {
            const char *fn=NULL;
            if      (strcmp(op,"CMP_EQ")==0) fn="qd_cmp_eq";
            else if (strcmp(op,"CMP_NE")==0) fn="qd_cmp_ne";
            else if (strcmp(op,"CMP_LT")==0) fn="qd_cmp_lt";
            else if (strcmp(op,"CMP_LE")==0) fn="qd_cmp_le";
            else if (strcmp(op,"CMP_GT")==0) fn="qd_cmp_gt";
            else if (strcmp(op,"CMP_GE")==0) fn="qd_cmp_ge";
            else if (strcmp(op,"CMP_IS")==0) fn="qd_cmp_is";
            if (fn) {
                // Fix: emit_val uses static buffer
                int id1 = gc++;
                int id2 = gc++;
                char a_buf[64], b_buf[64];
                snprintf(a_buf, sizeof(a_buf), "%%_v%d", id1);
                snprintf(b_buf, sizeof(b_buf), "%%_v%d", id2);
                
                const char *sl1 = slot_get(q->arg1);
                const char *sl2 = slot_get(q->arg2);
                if (sl1) {
                    fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n", a_buf, sl1);
                } else {
                    const char *cs1 = emit_cstr(q->arg1);
                    fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n", a_buf, cs1);
                }
                if (sl2) {
                    fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n", b_buf, sl2);
                } else {
                    const char *cs2 = emit_cstr(q->arg2);
                    fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n", b_buf, cs2);
                }
                
                char t[256]; snprintf(t,sizeof(t),"%%_cm%d",NEXT());
                fprintf(CF,"  %s = call %%QdObj* @%s(%%QdObj* %s, %%QdObj* %s)\n",t,fn,a_buf,b_buf);
                fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
                term=0; continue;
            }
            if (strcmp(op,"CMP_IS_NOT")==0) {
                // Fix: emit_val uses static buffer
                int id1 = gc++;
                int id2 = gc++;
                char a_buf[64], b_buf[64];
                snprintf(a_buf, sizeof(a_buf), "%%_v%d", id1);
                snprintf(b_buf, sizeof(b_buf), "%%_v%d", id2);
                
                const char *sl1 = slot_get(q->arg1);
                const char *sl2 = slot_get(q->arg2);
                if (sl1) {
                    fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n", a_buf, sl1);
                } else {
                    const char *cs1 = emit_cstr(q->arg1);
                    fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n", a_buf, cs1);
                }
                if (sl2) {
                    fprintf(CF,"  %s = load %%QdObj*, %%QdObj** %s\n", b_buf, sl2);
                } else {
                    const char *cs2 = emit_cstr(q->arg2);
                    fprintf(CF,"  %s = call %%QdObj* @qd_env_get(i8* %s)\n", b_buf, cs2);
                }
                
                char t1[256],t2[256];
                snprintf(t1,sizeof(t1),"%%_ci%d",NEXT());
                snprintf(t2,sizeof(t2),"%%_cn%d",NEXT());
                fprintf(CF,"  %s = call %%QdObj* @qd_cmp_is(%%QdObj* %s, %%QdObj* %s)\n",t1,a_buf,b_buf);
                fprintf(CF,"  %s = call %%QdObj* @qd_not(%%QdObj* %s)\n",t2,t1);
                fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t2,get_slot(q->result));
                term=0; continue;
            }
        }

        /* JUMP */
        if (strcmp(op,"JUMP")==0) {
            fprintf(CF,"  br label %%%s\n",q->arg1); term=1; continue;
        }

        /* BRANCH_IF_TRUE / BRANCH_IF_FALSE */
        if (strcmp(op,"BRANCH_IF_TRUE")==0||strcmp(op,"BRANCH_IF_FALSE")==0) {
            const char *cond=emit_val(q->arg1);
            char tv[256],ic[256]; int fn=NEXT(),fc=NEXT();
            snprintf(tv,sizeof(tv),"%%_bt%d",fn);
            snprintf(ic,sizeof(ic),"%%_bc%d",fc);
            char fall[64]; snprintf(fall,sizeof(fall),"_fall%d",NEXT());
            fprintf(CF,"  %s = call i32 @qd_truthy(%%QdObj* %s)\n",tv,cond);
            fprintf(CF,"  %s = icmp ne i32 %s, 0\n",ic,tv);
            if (strcmp(op,"BRANCH_IF_TRUE")==0)
                fprintf(CF,"  br i1 %s, label %%%s, label %%%s\n",ic,q->result,fall);
            else
                fprintf(CF,"  br i1 %s, label %%%s, label %%%s\n",ic,fall,q->result);
            fprintf(CF,"\n%s:\n",fall);
            term=0; continue;
        }

        /* BRANCH_IF_NONE */
        if (strcmp(op,"BRANCH_IF_NONE")==0) {
            const char *val=emit_val(q->arg1);
            char n1[256],n2[256]; snprintf(n1,sizeof(n1),"%%_bn%d",NEXT()); snprintf(n2,sizeof(n2),"%%_bnc%d",NEXT());
            char fall[64]; snprintf(fall,sizeof(fall),"_fall%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_none()\n",n1);
            fprintf(CF,"  %s = icmp eq %%QdObj* %s, %s\n",n2,val,n1);
            fprintf(CF,"  br i1 %s, label %%%s, label %%%s\n",n2,q->result,fall);
            fprintf(CF,"\n%s:\n",fall);
            term=0; continue;
        }

        /* BRANCH_IF_EXHAUSTED */
        if (strcmp(op,"BRANCH_IF_EXHAUSTED")==0) {
            const char *isl=get_slot(q->arg1);
            char iv[256],dn[256],cc[256];
            snprintf(iv,sizeof(iv),"%%_ie%d",NEXT()); snprintf(dn,sizeof(dn),"%%_id%d",NEXT()); snprintf(cc,sizeof(cc),"%%_ic%d",NEXT());
            char fall[64]; snprintf(fall,sizeof(fall),"_fall%d",NEXT());
            fprintf(CF,"  %s = load %%QdIter*, %%QdIter** %s\n",iv,isl);
            fprintf(CF,"  %s = call i32 @qd_iter_done(%%QdIter* %s)\n",dn,iv);
            fprintf(CF,"  %s = icmp ne i32 %s, 0\n",cc,dn);
            fprintf(CF,"  br i1 %s, label %%%s, label %%%s\n",cc,q->result,fall);
            fprintf(CF,"\n%s:\n",fall);
            term=0; continue;
        }

        /* GET_ITER */
        if (strcmp(op,"GET_ITER")==0) {
            const char *obj=emit_val(q->arg1);
            const char *sl=get_slot(q->result); /* is_iter slot */
            char t[256]; snprintf(t,sizeof(t),"%%_gi%d",NEXT());
            fprintf(CF,"  %s = call %%QdIter* @qd_iter_new(%%QdObj* %s)\n",t,obj);
            fprintf(CF,"  store %%QdIter* %s, %%QdIter** %s\n",t,sl);
            term=0; continue;
        }

        /* ITER_NEXT */
        if (strcmp(op,"ITER_NEXT")==0) {
            const char *isl=get_slot(q->arg1);
            char iv[256],nxt[256];
            snprintf(iv,sizeof(iv),"%%_inl%d",NEXT()); snprintf(nxt,sizeof(nxt),"%%_inx%d",NEXT());
            fprintf(CF,"  %s = load %%QdIter*, %%QdIter** %s\n",iv,isl);
            fprintf(CF,"  %s = call %%QdObj* @qd_iter_next(%%QdIter* %s)\n",nxt,iv);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",nxt,get_slot(q->result));
            term=0; continue;
        }

        /* ARG_PUSH */
        if (strcmp(op,"ARG_PUSH")==0) {
            int idx=atoi(q->arg2);
            if (idx<0||idx>=MAX_ARGS){fprintf(stderr,"arg index out of range\n");exit(1);}
            const char *v=emit_val(q->arg1);
            strncpy(arg_vals[idx],v,255);
            if (idx>=arg_cnt) arg_cnt=idx+1;
            term=0; continue;
        }

        /* CALL */
        if (strcmp(op,"CALL")==0) {
            const char *callee=emit_val(q->arg1);
            int argc=atoi(q->arg2);
            /* alloca 引數陣列 */
            char arr[256]; snprintf(arr,sizeof(arr),"%%_argv%d",NEXT());
            if (argc>0) {
                fprintf(CF,"  %s = alloca %%QdObj*, i32 %d\n",arr,argc);
                for (int k=0;k<argc;k++) {
                    char ep[256]; snprintf(ep,sizeof(ep),"%%_ap%d",NEXT());
                    fprintf(CF,"  %s = getelementptr %%QdObj*, %%QdObj** %s, i32 %d\n",ep,arr,k);
                    fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",arg_vals[k],ep);
                }
            }
            char ret[256]; snprintf(ret,sizeof(ret),"%%_cr%d",NEXT());
            if (argc>0)
                fprintf(CF,"  %s = call %%QdObj* @qd_call(%%QdObj* %s, %%QdObj** %s, i32 %d)\n",ret,callee,arr,argc);
            else
                fprintf(CF,"  %s = call %%QdObj* @qd_call(%%QdObj* %s, %%QdObj** null, i32 0)\n",ret,callee);
            if (!is_blank(q->result))
                fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",ret,get_slot(q->result));
            arg_cnt=0;
            term=0; continue;
        }

        /* RETURN */
        if (strcmp(op,"RETURN")==0) {
            const char *v=emit_val(q->arg1);
            fprintf(CF,"  ret %%QdObj* %s\n",v); term=1; continue;
        }
        if (strcmp(op,"RETURN_NONE")==0) {
            char t[256]; snprintf(t,sizeof(t),"%%_rn%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_none()\n",t);
            fprintf(CF,"  ret %%QdObj* %s\n",t); term=1; continue;
        }

        /* BUILD_LIST / BUILD_MAP */
        if (strcmp(op,"BUILD_LIST")==0) {
            char t[256]; snprintf(t,sizeof(t),"%%_bl%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_list_new()\n",t);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
            term=0; continue;
        }
        if (strcmp(op,"BUILD_MAP")==0) {
            char t[256]; snprintf(t,sizeof(t),"%%_bm%d",NEXT());
            fprintf(CF,"  %s = call %%QdObj* @qd_map_new()\n",t);
            fprintf(CF,"  store %%QdObj* %s, %%QdObj** %s\n",t,get_slot(q->result));
            term=0; continue;
        }

        /* LIST_APPEND */
        if (strcmp(op,"LIST_APPEND")==0) {
            const char *lst=emit_val(q->arg1), *item=emit_val(q->arg2);
            fprintf(CF,"  call void @qd_list_append(%%QdObj* %s, %%QdObj* %s)\n",lst,item);
            term=0; continue;
        }

        /* TRY_BEGIN / TRY_END (簡化：不產生 landingpad) */
        if (strcmp(op,"TRY_BEGIN")==0||strcmp(op,"TRY_END")==0) {
            fprintf(CF,"  ; %s (simplified)\n",op); term=0; continue;
        }

        /* RAISE */
        if (strcmp(op,"RAISE")==0) {
            const char *exc=emit_val(q->arg1);
            fprintf(CF,"  call void @qd_print(%%QdObj* %s)\n",exc);
            fprintf(CF,"  call void @exit(i32 1)\n");
            fprintf(CF,"  unreachable\n"); term=1; continue;
        }

        /* 未知指令 */
        fprintf(CF,"  ; [TODO: %s %s %s %s]\n",op,q->arg1,q->arg2,q->result);
        term=0;
    }
}

/* ================================================================== */
/*  輸出字串常數 globals                                               */
/* ================================================================== */
static void emit_str_globals(FILE *fout) {
    for (int i=0;i<nstr;i++) {
        char buf[1024]; int blen=unescape(str_pool[i].raw,buf,sizeof(buf));
        fprintf(fout,"%s = private unnamed_addr constant [%d x i8] c\"",str_pool[i].gname,blen);
        for (int j=0;j<blen;j++) {
            unsigned char c=(unsigned char)buf[j];
            if(c=='"'||c=='\\'||c<32||c>126) fprintf(fout,"\\%02X",c);
            else fputc(c,fout);
        }
        fprintf(fout,"\"\n");
    }
    fprintf(fout,"\n");
}

/* ================================================================== */
/*  輸出外部函式宣告                                                   */
/* ================================================================== */
static void emit_decls(FILE *fout) {
    fprintf(fout,
        "; ---- qd0lib 宣告 ----\n"
        "%%QdObj  = type opaque\n"
        "%%QdIter = type opaque\n\n"
        "declare %%QdObj*  @qd_none()\n"
        "declare %%QdObj*  @qd_bool(i32)\n"
        "declare %%QdObj*  @qd_int(i64)\n"
        "declare %%QdObj*  @qd_float(double)\n"
        "declare %%QdObj*  @qd_string(i8*)\n"
        "declare %%QdObj*  @qd_list_new()\n"
        "declare %%QdObj*  @qd_map_new()\n"
        "declare %%QdObj*  @qd_add(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_sub(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_mul(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_div(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_floordiv(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_mod(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_pow(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_neg(%%QdObj*)\n"
        "declare %%QdObj*  @qd_not(%%QdObj*)\n"
        "declare %%QdObj*  @qd_bitand(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_bitor(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_eq(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_ne(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_lt(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_le(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_gt(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_ge(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_cmp_is(%%QdObj*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_getattr(%%QdObj*, i8*)\n"
        "declare void      @qd_setattr(%%QdObj*, i8*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_getitem(%%QdObj*, %%QdObj*)\n"
        "declare void      @qd_setitem(%%QdObj*, %%QdObj*, %%QdObj*)\n"
        "declare void      @qd_list_append(%%QdObj*, %%QdObj*)\n"
        "declare %%QdIter* @qd_iter_new(%%QdObj*)\n"
        "declare %%QdObj*  @qd_iter_next(%%QdIter*)\n"
        "declare i32       @qd_iter_done(%%QdIter*)\n"
        "declare %%QdObj*  @qd_call(%%QdObj*, %%QdObj**, i32)\n"
        "declare i32       @qd_truthy(%%QdObj*)\n"
        "declare void      @qd_print(%%QdObj*)\n"
        "declare void      @qd_env_init()\n"
        "declare void      @qd_env_set(i8*, %%QdObj*)\n"
        "declare %%QdObj*  @qd_env_get(i8*)\n"
        "declare void      @exit(i32) noreturn\n\n"
    );
}

/* ================================================================== */
/*  main                                                                */
/* ================================================================== */
static void usage(void) {
    fprintf(stderr,
        "qd0c — DynIR 四元組編譯器\n"
        "用法：  qd0c <file.qd>\n"
        "輸出：  <file>.ll\n"
        "連結：  clang <file>.ll qd0lib.c -o <prog> -lm\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 2) usage();
    const char *inp = argv[1];

    /* 建立輸出路徑 */
    char outp[4096];
    strncpy(outp, inp, sizeof(outp)-4);
    char *dot = strrchr(outp, '.');
    if (dot) *dot='\0';
    strcat(outp, ".ll");

    FILE *fin = fopen(inp, "r");
    if (!fin) { perror(inp); return 1; }

    /* 解析 */
    parse_qd(fin);
    fclose(fin);
    fprintf(stderr,"qd0c: %d instructions from %s\n", nquads, inp);

    /* 第一遍預掃描（slot + 字串） */
    prescan();

    /* ---- 把代碼生成寫到 tmpfile，這樣字串常數可以在最前面輸出 ---- */
    FILE *tmp = tmpfile();
    if (!tmp) { perror("tmpfile"); return 1; }
    CF = tmp;

    /* 輸出 alloca slots（函式入口） */
    fprintf(CF,"define %%QdObj* @main() {\nentry:\n");
    fprintf(CF,"  call void @qd_env_init()\n\n");

    /* alloca 所有 slot */
    for (int i=0;i<npre_slots;i++) {
        const char *sn = safe_id(pre_slots[i].name);
        if (pre_slots[i].is_iter) {
            fprintf(CF,"  %%%s_iterslot = alloca %%QdIter*\n", sn+(*sn=='%'?1:0));
            char sl[256]; snprintf(sl,sizeof(sl),"%%%s_iterslot",sn);
            slot_def(pre_slots[i].name, sl, 1);
        } else {
            fprintf(CF,"  %%%s_slot = alloca %%QdObj*\n", sn);
            char sl[256]; snprintf(sl,sizeof(sl),"%%%s_slot",sn);
            slot_def(pre_slots[i].name, sl, 0);
        }
    }
    fprintf(CF,"\n");

    /* 主體 */
    emit_code();

    /* 結尾 */
    fprintf(CF,"\n  ret %%QdObj* zeroinitializer\n}\n");

    /* ---- 組合最終輸出 ---- */
    FILE *fout = fopen(outp, "w");
    if (!fout) { perror(outp); return 1; }

    fprintf(fout,"; generated by qd0c from %s\n",inp);
    fprintf(fout,"source_filename = \"%s\"\n\n",inp);

    /* 宣告 */
    emit_decls(fout);

    /* 字串常數（此時 nstr 已完整） */
    emit_str_globals(fout);

    /* 函式主體（從 tmp 複製） */
    rewind(tmp);
    char buf[8192]; int n;
    while ((n=fread(buf,1,sizeof(buf),tmp))>0) fwrite(buf,1,n,fout);
    fclose(tmp);
    fclose(fout);

    fprintf(stderr,"qd0c: wrote %s\n", outp);
    fprintf(stderr,"      next: clang %s qd0lib.c -o program -lm\n", outp);
    return 0;
}