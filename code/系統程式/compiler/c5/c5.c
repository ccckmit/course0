// c5.c - C in four functions (ELF Phase 3: Pure POSIX I/O)
//
// Refactored to use standard POSIX open/read/write/close.
// Removed FILE and libc I/O wrappers. Fully self-compilable.

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>

struct symbol {
  long v_tk;
  long v_hash;
  char *v_name;
  long v_class;
  long v_type;
  long v_val;
  long v_hclass;
  long v_htype;
  long v_hval;
  long v_size;
};

// -------------------------------------------------------
// ELF 符號表結構
// -------------------------------------------------------
struct Elf64_Sym {
  long st_name;
  long st_info;
  long st_other;
  long st_shndx;
  long st_value;
  long st_size;
};

// -------------------------------------------------------
// Relocation 表結構 (自定義，供 c5tool link 使用)
// -------------------------------------------------------
struct Rela {
  long r_offset;   // text 中需要被填的 slot 位址 (以 long 為單位的 index)
  long r_symidx;   // symtab 中的符號 index（-1 = 內部 reloc，無對應符號）
  long r_type;     // RELA_DATA=1, RELA_FUNC=2, RELA_JMP=3
};

// r_type 說明：
//   RELA_DATA = 1 : text[r_offset] 需填入 data segment 的 byte offset（含跨 ELF 偏移）
//   RELA_FUNC = 2 : text[r_offset] 需填入 text segment 的 long index（含跨 ELF 偏移）
//   RELA_JMP  = 3 : text[r_offset] 需填入 text segment 的 long index（JMP/BZ/BNZ）
// r_symidx >= 0 : 外部符號，需查 symtab 找定義
// r_symidx == -1: 內部符號，r_offset 處的現有值是本 ELF 內的 index，需加 base
enum { RELA_DATA = 1, RELA_FUNC = 2, RELA_JMP = 3 };

char *data_base;
long *text_base;
struct Elf64_Sym *symtab_base;
char *strtab_base;
long symtab_count;
long strtab_ptr;

struct Rela *rela_base;
long rela_count;

char *p, *lp,
     *data;

long *e, *le,
    tk,
    ival,
    ty,
    loc,
    line,
    src,
    debug;

double fval;
struct symbol *sym, *id;

enum {
  Num = 128, NumF, Fun, Sys, Glo, GloArr, Loc, LocArr, Id, Member,
  Ext, ExtFun,
  Char, Double, Else, Enum, Float, For, Break, Continue, If, Int, Int64, Long, Return, Short, Sizeof, While, Struct, Extern,
  Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak, Arrow, Dot
};

// 移除了原始 C4 對 libc FILE 的相依性 (FOPN, FWRT, FCLS)，使用原生的 POSIX SYS Calls
enum { LLA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       LF  ,SF  ,IMMF,ITF ,ITFS,FTI ,FADD,FSUB,FMUL,FDIV,
       FEQ ,FNE ,FLT ,FGT ,FLE ,FGE ,PRTF_DBL,
       OPEN,READ,WRIT,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

enum { CHAR, SHORT, INT, LONG, FLOAT, DOUBLE, STRUCT_TY, PTR };
enum { Idsz = 10 };

double long_to_double(long v) { return *(double *)&v; }
long   double_to_long(double d) { return *(long *)&d; }

char *instr_name;
#define LAST_WITH_OPERAND ADJ

int is_float_ty(long t) { return (t == FLOAT || t == DOUBLE); }

void error(char *msg) {
  char *ep;
  printf("%ld: Error: %s\n", line, msg);
  ep = lp;
  while (*ep && *ep != '\n') { printf("%c", *ep); ep = ep + 1; }
  printf("\n");
  ep = lp;
  while (ep < p - 1) {
    if (*ep == '\t') printf("\t");
    else printf(" ");
    ep = ep + 1;
  }
  printf("^\n");
  exit(-1);
}

extern long add_symbol(struct symbol *s, long type, long value, long shndx);
extern void add_rela(long text_slot, long sym_idx, long type);
extern void write_elf(char *filename);


void next()
{
  char *pp;
  long op;
  double frac;
  long sign;
  long exp;
  long start_offset;

  while (tk = *p) {
    ++p;
    if (tk == '\n') {
      if (src) {
        printf("%ld: ", line);
        pp = lp;
        while (pp < p) { printf("%c", *pp); pp = pp + 1; }
        lp = p;
        while (le < e) {
          op = *++le;
          printf("%8.4s", instr_name + op * 5);
          if (op <= ADJ || op == IMMF) printf(" %ld\n", *++le); else printf("\n");
        }
      }
      lp = p;
      ++line;
    }
    else if (tk == '#') {
      while (*p != 0 && *p != '\n') ++p;
    }
    else if ((tk >= 'a' && tk <= 'z') || (tk >= 'A' && tk <= 'Z') || tk == '_') {
      pp = p - 1;
      while ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || (*p >= '0' && *p <= '9') || *p == '_')
        tk = (tk & 0x0000FFFF) * 147 + *p++;
      tk = ((tk & 0x00FFFFFF) << 6) + (p - pp);
      
      id = sym;
      while (id->v_tk) {
        if (tk == id->v_hash && !memcmp(id->v_name, pp, p - pp)) { tk = id->v_tk; return; }
        id = (struct symbol *)((long *)id + Idsz);
      }
      id->v_name = (char *)pp;
      id->v_hash = tk;
      tk = id->v_tk = Id;
      return;
    }
    else if (tk >= '0' && tk <= '9') {
      if (ival = tk - '0') { while (*p >= '0' && *p <= '9') ival = ival * 10 + *p++ - '0'; }
      else if (*p == 'x' || *p == 'X') {
        while ((tk = *++p) && ((tk >= '0' && tk <= '9') || (tk >= 'a' && tk <= 'f') || (tk >= 'A' && tk <= 'F')))
          ival = ival * 16 + (tk & 15) + (tk >= 'A' ? 9 : 0);
      }
      else { while (*p >= '0' && *p <= '7') ival = ival * 8 + *p++ - '0'; }

      if (*p == '.' || *p == 'e' || *p == 'E') {
        fval = (double)ival;
        if (*p == '.') {
          ++p;
          frac = 1.0;
          while (*p >= '0' && *p <= '9') {
            frac = frac * 0.1;
            fval = fval + (double)(*p - '0') * frac;
            ++p;
          }
        }
        if (*p == 'e' || *p == 'E') {
          ++p;
          sign = 1;
          if (*p == '-') { sign = -1; ++p; }
          else if (*p == '+') { ++p; }
          exp = 0;
          while (*p >= '0' && *p <= '9') {
            exp = exp * 10 + (*p - '0');
            ++p;
          }
          if (sign == 1) {
            while (exp > 0) { fval = fval * 10.0; exp = exp - 1; }
          } else {
            while (exp > 0) { fval = fval * 0.1; exp = exp - 1; }
          }
        }
        if (*p == 'f' || *p == 'F') ++p;
        tk = NumF;
      } else {
        if (*p == 'f' || *p == 'F') { ++p; fval = (double)ival; tk = NumF; }
        else tk = Num;
      }
      return;
    }
    else if (tk == '/') {
      if (*p == '/') {
        p = p + 1;
        while (*p != 0 && *p != '\n') p = p + 1;
      }
      else if (*p == '*') {
        p = p + 1;
        while (*p != 0) {
          if (*p == '*' && p[1] == '/') { p = p + 2; break; }
          if (*p == '\n') {
            if (src) {
              printf("%ld: ", line);
              pp = lp;
              while (pp <= p) { printf("%c", *pp); pp = pp + 1; }
            }
            lp = p + 1;
            line = line + 1;
          }
          p = p + 1;
        }
      }
      else { tk = Div; return; }
    }
    else if (tk == '\'' || tk == '"') {
      start_offset = (long)(data - data_base);
      while (*p != 0 && *p != tk) {
        if ((ival = *p++) == '\\') {
          if ((ival = *p++) == 'n') ival = '\n';
        }
        if (tk == '"') *data++ = ival;
      }
      ++p;
      if (tk == '"') {
        *data++ = 0;
        ival = start_offset;
      } else tk = Num;
      return;
    }
    else if (tk == '=') { if (*p == '=') { ++p; tk = Eq; } else tk = Assign; return; }
    else if (tk == '+') { if (*p == '+') { ++p; tk = Inc; } else tk = Add; return; }
    else if (tk == '-') { 
      if (*p == '-') { ++p; tk = Dec; } 
      else if (*p == '>') { ++p; tk = Arrow; }
      else tk = Sub; 
      return; 
    }
    else if (tk == '!') { if (*p == '=') { ++p; tk = Ne; } return; }
    else if (tk == '<') { if (*p == '=') { ++p; tk = Le; } else if (*p == '<') { ++p; tk = Shl; } else tk = Lt; return; }
    else if (tk == '>') { if (*p == '=') { ++p; tk = Ge; } else if (*p == '>') { ++p; tk = Shr; } else tk = Gt; return; }
    else if (tk == '|') { if (*p == '|') { ++p; tk = Lor; } else tk = Or; return; }
    else if (tk == '&') { if (*p == '&') { ++p; tk = Lan; } else tk = And; return; }
    else if (tk == '^') { tk = Xor; return; }
    else if (tk == '%') { tk = Mod; return; }
    else if (tk == '*') { tk = Mul; return; }
    else if (tk == '[') { tk = Brak; return; }
    else if (tk == '?') { tk = Cond; return; }
    else if (tk == '.') { tk = Dot; return; }
    else if (tk == '~' || tk == ';' || tk == '{' || tk == '}' || tk == '(' || tk == ')' || tk == ']' || tk == ',' || tk == ':') return;
  }
}

void expr(long lev)
{
  long t, *d, sz;
  struct symbol *s;
  long is_dot;

  if (!tk) error("unexpected eof in expression");
  else if (tk == Num) {
    *++e = IMM; *++e = ival; next();
    ty = INT;
  }
  else if (tk == NumF) {
    *++e = IMMF; *++e = double_to_long(fval); next();
    ty = DOUBLE;
  }
  else if (tk == '"') {
    *++e = IMM; *++e = ival;
    add_rela((long)(e - text_base), -1, RELA_DATA); // 字串在 data segment，需重定位
    next();
    while (tk == '"') next();
    data = (char *)(((long)data + sizeof(long)) & -sizeof(long)); ty = PTR;
  }
  else if (tk == Sizeof) {
    next(); if (tk == '(') next(); else error("open paren expected in sizeof");
    sz = sizeof(long);
    ty = INT;
    if (tk == Int || tk == Int64 || tk == Long || tk == Short) { next(); sz = sizeof(long); ty = INT; }
    else if (tk == Char) { next(); sz = sizeof(char); ty = CHAR; }
    else if (tk == Float) { next(); sz = sizeof(double); ty = FLOAT; }
    else if (tk == Double) { next(); sz = sizeof(double); ty = DOUBLE; }
    else if (tk == Struct) {
      next();
      if (tk == Id) { sz = id->v_size; next(); }
      ty = STRUCT_TY;
    }
    while (tk == Mul) { next(); ty = ty + PTR; }
    if (tk == ')') next(); else error("close paren expected in sizeof");
    *++e = IMM;
    if (ty >= PTR) *++e = sizeof(long);
    else *++e = sz;
    ty = INT;
  }
  else if (tk == Id) {
    s = id; next();
    if (tk == '(') {
      next();
      t = 0;
      while (tk != ')') { expr(Assign); *++e = PSH; ++t; if (tk == ',') next(); }
      next();
      if (s->v_class == Sys) *++e = s->v_val;
      else if (s->v_class == Fun) {
        *++e = JSR; *++e = s->v_val;
        add_rela((long)(e - text_base), -1, RELA_FUNC); // 內部呼叫，需加 text_base
      }
      else if (s->v_class == ExtFun) {
        // 外部函式呼叫：JSR + placeholder，記 relocation
        *++e = JSR; *++e = 0;
        add_rela((long)(e - text_base), s->v_val, RELA_FUNC);
      }
      else error("bad function call");
      if (t) { *++e = ADJ; *++e = t; }
      ty = s->v_type;
    }
    else if (s->v_class == Num) { *++e = IMM; *++e = s->v_val; ty = INT; }
    else {
      if (s->v_class == Loc || s->v_class == LocArr) {
        *++e = LLA; *++e = loc - s->v_val;
        ty = s->v_type;
        if (s->v_class != LocArr) { 
          if (ty == CHAR)       *++e = LC;
          else if (is_float_ty(ty))  *++e = LF;
          else if (ty == STRUCT_TY) ;
          else                       *++e = LI;
        }
      }
      else if (s->v_class == GloArr) {
        *++e = IMM; *++e = s->v_val;
        add_rela((long)(e - text_base), -1, RELA_DATA); // 內部 data 位址
        ty = s->v_type;
      }
      else if (s->v_class == Glo) {
        *++e = IMM; *++e = s->v_val;
        add_rela((long)(e - text_base), -1, RELA_DATA); // 內部 data 位址
        ty = s->v_type;
        if      (ty == CHAR)       *++e = LC;
        else if (is_float_ty(ty))  *++e = LF;
        else if (ty == STRUCT_TY)  ;
        else                       *++e = LI;
      }
      else if (s->v_class == Ext) {
        // 外部變數：IMM + placeholder，記 relocation，再 LI/LC/LF
        *++e = IMM; *++e = 0;
        add_rela((long)(e - text_base), s->v_val, RELA_DATA);
        ty = s->v_type;
        if      (ty == CHAR)       *++e = LC;
        else if (is_float_ty(ty))  *++e = LF;
        else if (ty == STRUCT_TY)  ;
        else                       *++e = LI;
      }
      else error("undefined variable");
    }
  }
  else if (tk == '(') {
    next();
    if (tk == Int || tk == Int64 || tk == Char || tk == Long || tk == Short ||
        tk == Float || tk == Double || tk == Struct) {
      if      (tk == Int || tk == Int64 || tk == Long || tk == Short) { next(); t = INT; }
      else if (tk == Char)   { next(); t = CHAR; }
      else if (tk == Float)  { next(); t = FLOAT; }
      else if (tk == Double) { next(); t = DOUBLE; }
      else if (tk == Struct) { next(); if (tk == Id) next(); t = STRUCT_TY; }
      while (tk == Mul) { next(); t = t + PTR; }
      if (tk == ')') next(); else error("bad cast");
      expr(Inc);
      if (is_float_ty(t) && !is_float_ty(ty)) { *++e = ITF; }  
      else if (!is_float_ty(t) && is_float_ty(ty)) { *++e = FTI; } 
      ty = t;
    }
    else {
      expr(Assign);
      if (tk == ')') next(); else error("close paren expected");
    }
  }
  else if (tk == Mul) {
    next(); expr(Inc);
    if (ty > INT) ty = ty - PTR; else error("bad dereference");
    if      (ty == CHAR)      *++e = LC;
    else if (is_float_ty(ty)) *++e = LF;
    else if (ty == STRUCT_TY) ;
    else                      *++e = LI;
  }
  else if (tk == And) {
    next(); expr(Inc);
    if (*e == LC || *e == LI || *e == LF) e = e - 1;
    else if (ty == STRUCT_TY) ;
    else error("bad address-of");
    ty = ty + PTR;
  }
  else if (tk == '!') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = 0; *++e = EQ; ty = INT; }
  else if (tk == '~') { next(); expr(Inc); *++e = PSH; *++e = IMM; *++e = -1; *++e = XOR; ty = INT; }
  else if (tk == Add) { next(); expr(Inc); ty = INT; }
  else if (tk == Sub) {
    next();
    if (tk == Num)  { *++e = IMM;  *++e = -ival;                      next(); ty = INT; }
    else if (tk == NumF) { *++e = IMMF; *++e = double_to_long(-fval); next(); ty = DOUBLE; }
    else {
      expr(Inc);
      if (is_float_ty(ty)) {
        *++e = PSH;
        *++e = IMMF; *++e = double_to_long(-1.0);
        *++e = FMUL;
      } else {
        *++e = PSH; *++e = IMM; *++e = -1; *++e = MUL;
        ty = INT;
      }
    }
  }
  else if (tk == Inc || tk == Dec) {
    t = tk; next(); expr(Inc);
    if      (*e == LC) { *e = PSH; *++e = LC; }
    else if (*e == LI) { *e = PSH; *++e = LI; }
    else if (*e == LF) { *e = PSH; *++e = LF; }
    else error("bad lvalue in pre-increment");
    *++e = PSH;
    *++e = IMM; *++e = (ty > PTR) ? sizeof(long) : sizeof(char);
    *++e = (t == Inc) ? ADD : SUB;
    *++e = (ty == CHAR) ? SC : (is_float_ty(ty) ? SF : SI);
  }
  else error("bad expression");
  
  while (tk >= lev) {
    t = ty;
    if (tk == Assign) {
      next();
      if (*e == LC || *e == LI || *e == LF) *e = PSH;
      else error("bad lvalue in assignment");
      expr(Assign);
      if (is_float_ty(t) && !is_float_ty(ty)) *++e = ITF;
      else if (!is_float_ty(t) && is_float_ty(ty)) *++e = FTI;
      *++e = (t == CHAR) ? SC : (is_float_ty(t) ? SF : SI);
      ty = t;
    }
    else if (tk == Cond) {
      next();
      *++e = BZ; d = ++e;
      expr(Assign);
      if (tk == ':') next(); else error("conditional missing colon");
      *d = (long)(e + 3 - text_base);
      add_rela((long)((long *)d - text_base), -1, RELA_JMP);  // BZ operand
      *++e = JMP; d = ++e;
      expr(Cond);
      *d = (long)(e + 1 - text_base);
      add_rela((long)((long *)d - text_base), -1, RELA_JMP);  // JMP operand
    }
    else if (tk == Lor) { next(); *++e = BNZ; d = ++e; expr(Lan); *d = (long)(e + 1 - text_base); add_rela((long)((long *)d - text_base), -1, RELA_JMP); ty = INT; }
    else if (tk == Lan) { next(); *++e = BZ;  d = ++e; expr(Or);  *d = (long)(e + 1 - text_base); add_rela((long)((long *)d - text_base), -1, RELA_JMP); ty = INT; }
    else if (tk == Or)  { next(); *++e = PSH; expr(Xor); *++e = OR;  ty = INT; }
    else if (tk == Xor) { next(); *++e = PSH; expr(And); *++e = XOR; ty = INT; }
    else if (tk == And) { next(); *++e = PSH; expr(Eq);  *++e = AND; ty = INT; }
    else if (tk == Eq) {
      next(); *++e = PSH; expr(Lt);
      if (is_float_ty(t) || is_float_ty(ty)) {
         if (!is_float_ty(ty)) *++e = ITF;
         if (!is_float_ty(t))  *++e = ITFS;
         *++e = FEQ;
      } else { *++e = EQ; }
      ty = INT;
    }
    else if (tk == Ne) {
      next(); *++e = PSH; expr(Lt);
      if (is_float_ty(t) || is_float_ty(ty)) {
         if (!is_float_ty(ty)) *++e = ITF;
         if (!is_float_ty(t))  *++e = ITFS;
         *++e = FNE;
      } else { *++e = NE; }
      ty = INT;
    }
    else if (tk == Lt) {
      next(); *++e = PSH; expr(Shl);
      if (is_float_ty(t) || is_float_ty(ty)) {
         if (!is_float_ty(ty)) *++e = ITF;
         if (!is_float_ty(t))  *++e = ITFS;
         *++e = FLT;
      } else { *++e = LT; }
      ty = INT;
    }
    else if (tk == Gt) {
      next(); *++e = PSH; expr(Shl);
      if (is_float_ty(t) || is_float_ty(ty)) {
         if (!is_float_ty(ty)) *++e = ITF;
         if (!is_float_ty(t))  *++e = ITFS;
         *++e = FGT;
      } else { *++e = GT; }
      ty = INT;
    }
    else if (tk == Le) {
      next(); *++e = PSH; expr(Shl);
      if (is_float_ty(t) || is_float_ty(ty)) {
         if (!is_float_ty(ty)) *++e = ITF;
         if (!is_float_ty(t))  *++e = ITFS;
         *++e = FLE;
      } else { *++e = LE; }
      ty = INT;
    }
    else if (tk == Ge) {
      next(); *++e = PSH; expr(Shl);
      if (is_float_ty(t) || is_float_ty(ty)) {
         if (!is_float_ty(ty)) *++e = ITF;
         if (!is_float_ty(t))  *++e = ITFS;
         *++e = FGE;
      } else { *++e = GE; }
      ty = INT;
    }
    else if (tk == Shl) { next(); *++e = PSH; expr(Add); *++e = SHL; ty = INT; }
    else if (tk == Shr) { next(); *++e = PSH; expr(Add); *++e = SHR; ty = INT; }
    else if (tk == Add) {
      next(); *++e = PSH; expr(Mul);
      if (is_float_ty(t) || is_float_ty(ty)) {
        if (!is_float_ty(ty)) *++e = ITF;
        if (!is_float_ty(t))  *++e = ITFS;
        *++e = FADD; ty = DOUBLE;
      } else {
        if (t > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(long); *++e = MUL; }
        *++e = ADD; ty = t;
      }
    }
    else if (tk == Sub) {
      next(); *++e = PSH; expr(Mul);
      if (is_float_ty(t) || is_float_ty(ty)) {
        if (!is_float_ty(ty)) *++e = ITF;
        if (!is_float_ty(t))  *++e = ITFS;
        *++e = FSUB; ty = DOUBLE;
      } else {
        if (t > PTR && t == ty) { *++e = SUB; *++e = PSH; *++e = IMM; *++e = sizeof(long); *++e = DIV; ty = INT; }
        else if (t > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(long); *++e = MUL; *++e = SUB; ty = t; }
        else { *++e = SUB; ty = t; }
      }
    }
    else if (tk == Mul) {
      next(); *++e = PSH; expr(Inc);
      if (is_float_ty(t) || is_float_ty(ty)) {
        if (!is_float_ty(ty)) *++e = ITF;
        if (!is_float_ty(t))  *++e = ITFS;
        *++e = FMUL; ty = DOUBLE;
      } else { *++e = MUL; ty = INT; }
    }
    else if (tk == Div) {
      next(); *++e = PSH; expr(Inc);
      if (is_float_ty(t) || is_float_ty(ty)) {
        if (!is_float_ty(ty)) *++e = ITF;
        if (!is_float_ty(t))  *++e = ITFS;
        *++e = FDIV; ty = DOUBLE;
      } else { *++e = DIV; ty = INT; }
    }
    else if (tk == Mod) { next(); *++e = PSH; expr(Inc); *++e = MOD; ty = INT; }
    else if (tk == Inc || tk == Dec) {
      if      (*e == LC) { *e = PSH; *++e = LC; }
      else if (*e == LI) { *e = PSH; *++e = LI; }
      else if (*e == LF) { *e = PSH; *++e = LF; }
      else error("bad lvalue in post-increment");
      *++e = PSH; *++e = IMM; *++e = (ty > PTR) ? sizeof(long) : sizeof(char);
      *++e = (tk == Inc) ? ADD : SUB;
      *++e = (ty == CHAR) ? SC : (is_float_ty(ty) ? SF : SI);
      *++e = PSH; *++e = IMM; *++e = (ty > PTR) ? sizeof(long) : sizeof(char);
      *++e = (tk == Inc) ? SUB : ADD;
      next();
    }
    else if (tk == Brak) {
      next(); *++e = PSH; expr(Assign);
      if (tk == ']') next(); else error("close bracket expected");
      if (t > PTR) { *++e = PSH; *++e = IMM; *++e = sizeof(long); *++e = MUL; }
      else if (t < PTR) error("pointer type expected");
      *++e = ADD;
      ty = t - PTR;
      if      (ty == CHAR)      *++e = LC;
      else if (is_float_ty(ty)) *++e = LF;
      else if (ty == STRUCT_TY) ;
      else                      *++e = LI;
    }
    else if (tk == Arrow || tk == Dot) {
      is_dot = (tk == Dot);
      next();
      if (tk != Id) error("bad struct member");
      if (id->v_class != Member) error("undefined struct member");
      if (is_dot) {
        if (*e == LC || *e == LI || *e == LF) {
          e = e - 1;
          if (le > e) le = e;
        }
        else if (t == STRUCT_TY) ;
        else error("bad lvalue in struct");
      }
      *++e = PSH; *++e = IMM; *++e = id->v_val; *++e = ADD;
      ty = id->v_type;
      if      (ty == CHAR)      *++e = LC;
      else if (is_float_ty(ty)) *++e = LF;
      else if (ty == STRUCT_TY) ;
      else                      *++e = LI;
      next();
    }
    else error("compiler error");
  }
}

void stmt()
{
  long *b, *c, *p2;
  long a, d;

  if (tk == If) {
    next();
    if (tk == '(') next(); else error("open paren expected");
    expr(Assign);
    if (tk == ')') next(); else error("close paren expected");
    *++e = BZ; b = ++e;
    stmt();
    if (tk == Else) {
      *b = (long)(e + 3 - text_base);
      add_rela((long)(b - text_base), -1, RELA_JMP);  // BZ operand
      *++e = JMP; b = ++e;
      next();
      stmt();
    }
    *b = (long)(e + 1 - text_base);
    add_rela((long)(b - text_base), -1, RELA_JMP);  // BZ or JMP operand
  }
  else if (tk == While) {
    next();
    a = (long)(e + 1 - text_base);
    if (tk == '(') next(); else error("open paren expected");
    expr(Assign);
    if (tk == ')') next(); else error("close paren expected");
    *++e = BZ; b = ++e;
    stmt();
    *++e = JMP; *++e = (long)a;
    add_rela((long)(e - text_base), -1, RELA_JMP);  // JMP back to loop top
    *b = (long)(e + 1 - text_base);
    add_rela((long)(b - text_base), -1, RELA_JMP);  // BZ exit operand
    p2 = b + 1;
    while (p2 < e) {
      if (*p2 == JMP) {
        ++p2;
        if (*p2 == -1) { *p2 = (long)(e + 1 - text_base); add_rela((long)(p2 - text_base), -1, RELA_JMP); }
        else if (*p2 == -2) { *p2 = (long)a; add_rela((long)(p2 - text_base), -1, RELA_JMP); }
      }
      ++p2;
    }
  }
  else if (tk == For) {
    next();
    if (tk == '(') next(); else error("open paren expected");
    if (tk != ';') expr(Assign);
    if (tk == ';') next(); else error("semicolon expected");
    a = (long)(e + 1 - text_base);
    b = 0;
    if (tk != ';') { expr(Assign); *++e = BZ; b = ++e; }
    if (tk == ';') next(); else error("semicolon expected");
    *++e = JMP; c = ++e;
    d = (long)(e + 1 - text_base);
    if (tk != ')') expr(Assign);
    *++e = JMP; *++e = (long)a;
    add_rela((long)(e - text_base), -1, RELA_JMP);   // JMP back to condition
    *c = (long)(e + 1 - text_base);
    add_rela((long)(c - text_base), -1, RELA_JMP);   // JMP to body
    if (tk == ')') next(); else error("close paren expected");
    stmt();
    *++e = JMP; *++e = (long)d;
    add_rela((long)(e - text_base), -1, RELA_JMP);   // JMP to increment
    if (b) { *b = (long)(e + 1 - text_base); add_rela((long)(b - text_base), -1, RELA_JMP); }
    p2 = c + 1;
    while (p2 < e) {
      if (*p2 == JMP) {
        ++p2;
        if (*p2 == -1) { *p2 = (long)(e + 1 - text_base); add_rela((long)(p2 - text_base), -1, RELA_JMP); }
        else if (*p2 == -2) { *p2 = (long)d; add_rela((long)(p2 - text_base), -1, RELA_JMP); }
      }
      ++p2;
    }
  }
  else if (tk == Break) {
    next();
    *++e = JMP; *++e = -1;   // placeholder, patched by enclosing while/for
    if (tk == ';') next(); else error("semicolon expected");
  }
  else if (tk == Continue) {
    next();
    *++e = JMP; *++e = -2;   // placeholder, patched by enclosing while/for
    if (tk == ';') next(); else error("semicolon expected");
  }
  else if (tk == Return) {
    next();
    if (tk != ';') expr(Assign);
    *++e = LEV;
    if (tk == ';') next(); else error("semicolon expected");
  }
  else if (tk == '{') {
    next();
    while (tk != '}') stmt();
    next();
  }
  else if (tk == ';') {
    next();
  }
  else {
    expr(Assign);
    if (tk == ';') next(); else error("semicolon expected");
  }
}

long prog() {
  long bt, bt_size, i, offset, mty, var_sz, words, struct_off, max_align, m_size, m_align, is_ptr;
  long *ent_ptr;
  struct symbol *struct_id;
  struct symbol *cur_id;
  long is_extern;
  long sym_idx;
  
  while (tk) {
    bt = INT; bt_size = sizeof(long);
    is_extern = 0;
    if (tk == Extern) { is_extern = 1; next(); }
    if      (tk == Int || tk == Int64 || tk == Long || tk == Short) { next(); bt = INT; bt_size = sizeof(long); }
    else if (tk == Char)   { next(); bt = CHAR; bt_size = sizeof(char); }
    else if (tk == Float)  { next(); bt = FLOAT; bt_size = sizeof(double); }
    else if (tk == Double) { next(); bt = DOUBLE; bt_size = sizeof(double); }
    else if (tk == Struct) {
      next();
      struct_id = 0;
      if (tk == Id) { struct_id = id; bt_size = id->v_size; next(); }
      if (tk == '{') {
        next();
        struct_off = 0;
        max_align = 1;
        while (tk != '}') {
          mty = INT; m_size = sizeof(long); m_align = sizeof(long);
          if      (tk == Int || tk == Int64 || tk == Long || tk == Short) { next(); mty = INT; m_size = sizeof(long); m_align = sizeof(long); }
          else if (tk == Char)   { next(); mty = CHAR; m_size = sizeof(char); m_align = sizeof(char); }
          else if (tk == Float)  { next(); mty = FLOAT; m_size = sizeof(double); m_align = sizeof(double); }
          else if (tk == Double) { next(); mty = DOUBLE; m_size = sizeof(double); m_align = sizeof(double); }
          else if (tk == Struct) {
            next();
            if (tk == Id) { m_size = id->v_size; m_align = sizeof(long); next(); }
            mty = STRUCT_TY;
          }
          is_ptr = 0;
          while (tk == Mul) { next(); mty = mty + PTR; is_ptr = 1; }
          if (is_ptr) { m_size = sizeof(long); m_align = sizeof(long); }
          if (tk != Id) error("bad struct member");

          if (struct_off % m_align != 0) struct_off = struct_off + (m_align - (struct_off % m_align));
          id->v_class = Member;
          id->v_val = struct_off;
          id->v_type = mty;

          next();
          if (tk == Brak) {
            next();
            if (tk != Num) error("struct array size must be constant");
            m_size = m_size * ival;
            id->v_type = mty + PTR;
            next();
            if (tk == ']') next();
          }
          struct_off = struct_off + m_size;
          if (m_align > max_align) max_align = m_align;
          if (tk == ';') next();
        }
        next();
        if (struct_off % max_align != 0) struct_off = struct_off + (max_align - (struct_off % max_align));
        if (struct_id) struct_id->v_size = struct_off;
        bt_size = struct_off;
      } else {
        if (struct_id) bt_size = struct_id->v_size;
      }
      bt = STRUCT_TY;
    }
    else if (tk == Enum) {
      next();
      if (tk != '{') next();
      if (tk == '{') {
        next();
        i = 0;
        while (tk != '}') {
          if (tk != Id) error("bad enum identifier");
          next();
          if (tk == Assign) {
            next();
            if (tk != Num) error("bad enum initializer");
            i = ival;
            next();
          }
          id->v_class = Num; id->v_type = INT; id->v_val = i++;
          if (tk == ',') next();
        }
        next();
      }
    }
    
    while (tk != ';' && tk != '}') {
      ty = bt;
      var_sz = bt_size;
      while (tk == Mul) { next(); ty = ty + PTR; var_sz = sizeof(long); }
      if (tk != Id) error("bad global declaration");
      if (id->v_class && !is_extern) error("duplicate global definition");
      cur_id = id;   // save before any next() can clobber id
      next();
      cur_id->v_type = ty;
      if (tk == '(') {
        // 函式：extern 宣告 vs 定義
        if (is_extern) {
          // extern 函式：只宣告，不產生 code，跳過參數列表
          while (tk != ')') next();
          next(); // consume ')'
          if (tk == '{') error("extern function cannot have body");
          // 登記為 ExtFun，v_val = symtab index（用 cur_id 避免 id 被污染）
          if (!cur_id->v_class) {
            cur_id->v_class = ExtFun;
            cur_id->v_type  = ty;
            sym_idx = add_symbol(cur_id, 0x12, 0, 0); // st_shndx=0 (SHN_UNDEF)
            cur_id->v_val = sym_idx;
          }
        } else {
        cur_id->v_class = Fun;
        cur_id->v_val = (long)(e + 1 - text_base); 
        add_symbol(cur_id, 0x12, cur_id->v_val, 1);
        
        next(); i = 0;
        while (tk != ')') {
          ty = INT;
          if      (tk == Int || tk == Int64 || tk == Long || tk == Short) { next(); ty = INT; }
          else if (tk == Char)   { next(); ty = CHAR; }
          else if (tk == Float)  { next(); ty = FLOAT; }
          else if (tk == Double) { next(); ty = DOUBLE; }
          else if (tk == Struct) { next(); if (tk == Id) next(); ty = STRUCT_TY; }
          while (tk == Mul) { next(); ty = ty + PTR; }
          if (tk != Id) error("bad parameter declaration");
          if (id->v_class == Loc) error("duplicate parameter definition");
          id->v_hclass = id->v_class; id->v_class = Loc;
          id->v_htype  = id->v_type;  id->v_type = ty;
          id->v_hval   = id->v_val;   id->v_val = i++;
          next();
          if (tk == ',') next();
        }
        next();
        if (tk != '{') error("bad function definition");
        loc = ++i;
        next();

	        *++e = ENT; ent_ptr = ++e; *ent_ptr = 0;

	        while (tk == Int || tk == Int64 || tk == Char || tk == Long || tk == Short ||
	               tk == Float || tk == Double || tk == Struct) {
	          if      (tk == Int || tk == Int64 || tk == Long || tk == Short) { next(); bt = INT; bt_size = sizeof(long); }
	          else if (tk == Char)   { next(); bt = CHAR; bt_size = sizeof(char); }
	          else if (tk == Float)  { next(); bt = FLOAT; bt_size = sizeof(double); }
	          else if (tk == Double) { next(); bt = DOUBLE; bt_size = sizeof(double); }
	          else if (tk == Struct) {
	            next();
	            struct_id = 0;
	            if (tk == Id) { struct_id = id; bt_size = id->v_size; next(); }
	            if (tk == '{') { while (tk != '}') next(); next(); }
	            bt = STRUCT_TY;
	          }
	          while (tk != ';') {
	            ty = bt;
	            var_sz = bt_size;
	            while (tk == Mul) { next(); ty = ty + PTR; var_sz = sizeof(long); }
	            words = (var_sz + sizeof(long) - 1) / sizeof(long);
	            if (tk != Id) error("bad local declaration");
	            if (id->v_class == Loc) error("duplicate local definition");
	            id->v_hclass = id->v_class; id->v_class = Loc;
	            id->v_htype  = id->v_type;  id->v_type = ty;
	            id->v_hval   = id->v_val;
	            next();
	            if (tk == Brak) {
	              id->v_class = LocArr;
	              next();
	              if (tk != Num) error("array size must be constant");
	              words = words * ival;
	              i = i + words;
	              id->v_val = i;
	              id->v_type = ty + PTR;
	              next();
	              if (tk == ']') next();
	            } else if (tk == Assign) {
	              i = i + words;
	              id->v_val = i;
	              next();
	              *++e = LLA; *++e = loc - id->v_val;
	              *++e = PSH;
	              expr(Assign);
	              if (is_float_ty(id->v_type) && !is_float_ty(ty)) *++e = ITF;
	              else if (!is_float_ty(id->v_type) && is_float_ty(ty)) *++e = FTI;
	              *++e = (id->v_type == CHAR) ? SC : (is_float_ty(id->v_type) ? SF : SI);
	            } else {
	              i = i + words;
	              id->v_val = i;
	            }
	            if (tk == ',') next();
	          }
	          next();
	        }
        
        *ent_ptr = i - loc;
        
        while (tk != '}') stmt();
        *++e = LEV;
        id = sym;
        while (id->v_tk) {
          if (id->v_class == Loc || id->v_class == LocArr) {
            id->v_class = id->v_hclass;
            id->v_type  = id->v_htype;
            id->v_val   = id->v_hval;
          }
          id = (struct symbol *)((long *)id + Idsz);
        }
        } // end else (non-extern function body)
      }
      else {
        // 全域變數（非函式）
        if (is_extern) {
          // extern 變數：不分配空間，登記 SHN_UNDEF
          if (!cur_id->v_class) {
            cur_id->v_class = Ext;
            cur_id->v_type  = ty;
            sym_idx = add_symbol(id, 0x11, 0, 0); // st_shndx=0
            cur_id->v_val = sym_idx;
          }
          // 跳過可能的陣列宣告 extern int arr[]
          if (tk == Brak) { while (tk != ']' && tk) next(); if (tk == ']') next(); }
        } else {
          // 非 extern 全域變數
          if (tk == Brak) {
	          cur_id->v_class = GloArr;
	          cur_id->v_val = (long)(data - data_base);
	          add_symbol(id, 0x11, cur_id->v_val, 2);
	          
	          next();
	          if (tk != Num) error("array size must be constant");
	          else {
	            data = data + ival * var_sz;
	            next();
	          }
	          if (tk == ']') next();
	          cur_id->v_type = ty + PTR;
          } else {
            cur_id->v_class = Glo;
	          cur_id->v_val = (long)(data - data_base); 
	          add_symbol(id, 0x11, cur_id->v_val, 2);
	          
	          data = data + var_sz;
          
            if (tk == Assign) {
              next();
              if (tk == Num) { *(long *)(data_base + cur_id->v_val) = ival; next(); }
              else if (tk == NumF) { *(double *)(data_base + cur_id->v_val) = fval; next(); }
              else if (tk == '"') { *(long *)(data_base + cur_id->v_val) = ival; next(); while(tk == '"') next(); }
              else if (tk == '-') {
                next();
                if (tk == Num) { *(long *)(data_base + cur_id->v_val) = -ival; next(); }
                else if (tk == NumF) { *(double *)(data_base + cur_id->v_val) = -fval; next(); }
                else error("bad global initialization");
              }
              else error("bad global initialization");
            }
          }
        }
      }
      if (tk == ',') next();
    }
    next();
  }
  return 0;
}

long to_addr(long addr, long d_base, long poolsz) {
    if (addr >= 0) {
        if (addr < poolsz) return d_base + addr;
    }
    return addr;
}

long run(long *pc, long *bp, long *sp, char *d_base, long *t_base, long poolsz) {
  long a, cycle;
  long i, *t;
  double fa, fb;
  char *fmt;
  long fargs[5];
  char *f2;
  long ai;
  char spec[32];
  char *sp3;
  double dv;
  long cv;
  long t_addr;

  cycle = 0;
  while (1) {
    i = *pc++; ++cycle;
    if (debug) {
      printf("%ld> %.4s", cycle, instr_name + i * 5);
      if (i <= ADJ || i == IMMF) printf(" %ld\n", *pc); else printf("\n");
    }

    if      (i == LLA) a = (long)(bp + *pc++);
    else if (i == IMM) a = *pc++;
    else if (i == JMP) pc = t_base + *pc;
    else if (i == JSR) { *--sp = (long)(pc + 1 - t_base); pc = t_base + *pc; } 
    else if (i == BZ)  pc = a ? pc + 1 : t_base + *pc;
    else if (i == BNZ) pc = a ? t_base + *pc : pc + 1;
    else if (i == ENT) { *--sp = (long)bp; bp = sp; sp = sp - *pc++; }
    else if (i == ADJ) sp = sp + *pc++;
    else if (i == LEV) { sp = bp; bp = (long *)*sp++; pc = t_base + *sp++; }
    
    else if (i == LI)  a = *(long *)to_addr(a, (long)d_base, poolsz);
    else if (i == LC)  a = *(char *)to_addr(a, (long)d_base, poolsz);
    else if (i == SI)  { t_addr = *sp++; *(long *)to_addr(t_addr, (long)d_base, poolsz) = a; }
    else if (i == SC)  { t_addr = *sp++; a = *(char *)to_addr(t_addr, (long)d_base, poolsz) = a; }
    else if (i == PSH) *--sp = a;

    else if (i == OR)  a = *sp++ |  a;
    else if (i == XOR) a = *sp++ ^  a;
    else if (i == AND) a = *sp++ &  a;
    else if (i == EQ)  a = *sp++ == a;
    else if (i == NE)  a = *sp++ != a;
    else if (i == LT)  a = *sp++ <  a;
    else if (i == GT)  a = *sp++ >  a;
    else if (i == LE)  a = *sp++ <= a;
    else if (i == GE)  a = *sp++ >= a;
    else if (i == SHL) a = *sp++ << a;
    else if (i == SHR) a = *sp++ >> a;
    else if (i == ADD) a = *sp++ +  a;
    else if (i == SUB) a = *sp++ -  a;
    else if (i == MUL) a = *sp++ * a;
    else if (i == DIV) a = *sp++ /  a;
    else if (i == MOD) a = *sp++ %  a;

    else if (i == IMMF) { a = *pc++; } 
    else if (i == LF) { a = double_to_long(*(double *)to_addr(a, (long)d_base, poolsz)); }
    else if (i == SF) { t_addr = *sp++; *(double *)to_addr(t_addr, (long)d_base, poolsz) = long_to_double(a); }
    else if (i == ITF) { fa = (double)a; a = double_to_long(fa); }
    else if (i == ITFS) { fa = (double)(*sp); *sp = double_to_long(fa); }
    else if (i == FTI) { fa = long_to_double(a); a = (long)fa; }

    else if (i == FADD) { fa = long_to_double(*sp++) + long_to_double(a); a = double_to_long(fa); }
    else if (i == FSUB) { fa = long_to_double(*sp++) - long_to_double(a); a = double_to_long(fa); }
    else if (i == FMUL) { fa = long_to_double(*sp++) * long_to_double(a); a = double_to_long(fa); }
    else if (i == FDIV) { fa = long_to_double(*sp++) / long_to_double(a); a = double_to_long(fa); }

    else if (i == FEQ) { fa = long_to_double(*sp++); fb = long_to_double(a); a = (fa == fb); }
    else if (i == FNE) { fa = long_to_double(*sp++); fb = long_to_double(a); a = (fa != fb); }
    else if (i == FLT) { fa = long_to_double(*sp++); fb = long_to_double(a); a = (fa <  fb); }
    else if (i == FGT) { fa = long_to_double(*sp++); fb = long_to_double(a); a = (fa >  fb); }
    else if (i == FLE) { fa = long_to_double(*sp++); fb = long_to_double(a); a = (fa <= fb); }
    else if (i == FGE) { fa = long_to_double(*sp++); fb = long_to_double(a); a = (fa >= fb); }

    // 支援 3 參數的 standard open syscall，並加入寫入 write
    else if (i == OPEN) a = open((char *)to_addr(sp[2], (long)d_base, poolsz), sp[1], *sp);
    else if (i == READ) a = read(sp[2], (char *)to_addr(sp[1], (long)d_base, poolsz), *sp);
    else if (i == WRIT) a = write(sp[2], (char *)to_addr(sp[1], (long)d_base, poolsz), *sp);
    else if (i == CLOS) a = close(*sp);
    else if (i == PRTF) {
      t = sp + pc[1];
      fmt = (char *)to_addr(t[-1], (long)d_base, poolsz);
      fargs[0]=t[-2]; fargs[1]=t[-3]; fargs[2]=t[-4]; fargs[3]=t[-5]; fargs[4]=t[-6];
      f2 = fmt; ai = 0; a = 0;
      while (*f2) {
        if (*f2 != '%') { printf("%c", *f2++); a = a + 1; continue; }
        sp3 = spec;
        *sp3++ = *f2++;
        while (*f2=='-'||*f2=='+'||*f2==' '||*f2=='#'||*f2=='0') *sp3++ = *f2++;
        while (*f2>='0' && *f2<='9') *sp3++ = *f2++;
        if (*f2 == '.') { *sp3++ = *f2++; while (*f2>='0'&&*f2<='9') *sp3++ = *f2++; }
        if (*f2=='l'||*f2=='h'||*f2=='L') *sp3++ = *f2++;
        if (*f2=='l') *sp3++ = *f2++;
        cv = *f2 ? *f2++ : 0;
        *sp3++ = cv; *sp3 = 0;
        if (cv=='f'||cv=='g'||cv=='e'||cv=='F'||cv=='G'||cv=='E') {
          dv = long_to_double(fargs[ai<5?ai:4]);
          a = a + printf(spec, dv);
        } else if (cv=='%') {
          a = a + printf("%%");
        } else if (cv=='s') {
          a = a + printf(spec, (char *)to_addr(fargs[ai<5?ai:4], (long)d_base, poolsz));
        } else {
          a = a + printf(spec, fargs[ai<5?ai:4]);
        }
        if (cv != '%') ai = ai + 1;
      }
    }
    else if (i == MALC) a = (long)malloc(*sp);
    else if (i == FREE) free((void *)*sp);
    else if (i == MSET) a = (long)memset((char *)to_addr(sp[2], (long)d_base, poolsz), sp[1], *sp);
    else if (i == MCMP) a = memcmp((char *)to_addr(sp[2], (long)d_base, poolsz), (char *)to_addr(sp[1], (long)d_base, poolsz), *sp);
    else if (i == EXIT) { if (debug) printf("exit(%ld) cycle = %ld\n", *sp, cycle); return *sp; }
    
    else { printf("unknown instruction = %ld! cycle = %ld\n", i, cycle); return -1; }
  }
}

// -------------------------------------------------------
// 產生並輸出 ELF 檔案的核心邏輯
// -------------------------------------------------------

long compile(long argc, char **argv)
{
  long fd, poolsz;
  struct symbol *idmain;
  long *pc, *bp, *sp;
  long i, *t, exit_offset;
  char *outfile;

  // 註冊 VM Opcode 字串 (加入 WRIT, FLOAT, 等新增的 opcode)
  instr_name = "LLA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,LF  ,SF  ,IMMF,ITF ,ITFS,FTI ,FADD,FSUB,FMUL,FDIV,FEQ ,FNE ,FLT ,FGT ,FLE ,FGE ,PRTF,OPEN,READ,WRIT,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,";

  outfile = 0;
  --argc; ++argv;
  while (argc > 0 && **argv == '-') {
    if ((*argv)[1] == 's') { src = 1; --argc; ++argv; }
    else if ((*argv)[1] == 'd') { debug = 1; --argc; ++argv; }
    else if ((*argv)[1] == 'o') { 
      --argc; ++argv; 
      outfile = *argv; 
      --argc; ++argv; 
    }
    else { --argc; ++argv; }
  }

  if (argc < 1) { printf("usage: c4f [-s] [-d] [-o out.elf] file ...\n"); return -1; }

  poolsz = 1024*1024;
  if (!(sym = (struct symbol *)malloc(poolsz))) { printf("could not malloc(%ld) symbol area\n", poolsz); return -1; }
  if (!(le = e = malloc(poolsz)))               { printf("could not malloc(%ld) text area\n",   poolsz); return -1; }
  if (!(data = malloc(poolsz)))                 { printf("could not malloc(%ld) data area\n",   poolsz); return -1; }
  if (!(sp = malloc(poolsz)))                   { printf("could not malloc(%ld) stack area\n",  poolsz); return -1; }
  
  if (!(symtab_base = (struct Elf64_Sym *)malloc(poolsz))) { return -1; }
  if (!(strtab_base = (char *)malloc(poolsz))) { return -1; }
  if (!(rela_base = (struct Rela *)malloc(poolsz))) { return -1; }

  // open 固定傳入 3 參數，對齊 Native VM opcode 設計
  if ((fd = open(*argv, 0, 0)) < 0) { printf("could not open(%s)\n", *argv); return -1; }

  memset(sym,  0, poolsz);
  memset(e,    0, poolsz);
  memset(data, 0, poolsz);
  
  text_base = e;
  data_base = data;
  symtab_count = 0;
  strtab_ptr = 1;
  strtab_base[0] = 0;
  rela_count = 0;

  // 將 POSIX I/O 及 printf、malloc 註冊為 syscall。加入 extern 關鍵字。
  p = "char double else enum float for break continue if int int64 long return short sizeof while struct extern open read write close printf malloc free memset memcmp exit void main";
  i = Char; while (i <= Extern) { next(); id->v_tk = i++; }
  i = OPEN; while (i <= EXIT) { next(); id->v_class = Sys; id->v_type = INT; id->v_val = i++; }
  next(); id->v_tk = Char; // void -> Char (ptr)
  next(); idmain = id;     // main

  if (!(lp = p = malloc(poolsz))) { printf("could not malloc(%ld) source area\n", poolsz); return -1; }
  if ((i = read(fd, p, poolsz-1)) <= 0) { printf("read() returned %ld\n", i); return -1; }
  p[i] = 0;
  close(fd);

  line = 1;
  next(); 

  if (prog() == -1) return -1;

  // library mode (-o only, no main required)
  if (outfile && !(idmain->v_val)) {
    *++e = EXIT;  // minimal terminator so text is non-empty
    write_elf(outfile);
    return 0;
  }

  if (!(idmain->v_val)) { printf("main() not defined\n"); return -1; }
  
  *++e = PSH;
  *++e = EXIT;
  exit_offset = (long)(e - 1 - text_base);

  if (outfile) {
    write_elf(outfile);
    return 0;
  }

  if (src) return 0;

  bp = sp = (long *)((long)sp + poolsz);
  *--sp = argc;
  *--sp = (long)argv;
  *--sp = exit_offset;

  return run(text_base + idmain->v_val, bp, sp, data_base, text_base, poolsz);
}

int main(int argc, char **argv) {
  return (int)compile(argc, argv);
}