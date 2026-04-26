// jit.c - c5 VM JIT executor (x86-64 and AArch64 backends in one file)
//
// Build for x86-64:
//   gcc -w -g -O0 -DJIT_X86   jit.c -o jit
//
// Build for AArch64 / Apple M-series:
//   cc  -w -g -O1 -DJIT_ARM64 jit.c -o jit
//   (on macOS also add: -fsanitize=address  for debugging)
//
// Usage:
//   jit [-d] <program.elf> [args...]
//
// The JIT reads a linked .elf produced by:
//   c5  -o foo.elf foo.c          # compile
//   c5_tool link -o foo.elf ...   # (optional) link multiple objects
// and JIT-compiles the c5 VM bytecode to native code in two passes.
//
// Design notes common to both backends:
//   - text[0] is always padding (c5 uses *++e); compile loop starts at pc=1.
//   - 2-word instructions also stamp jitmap[pc+1] = jitmap[pc] so that any
//     jump targeting the operand slot gets a valid native pointer.
//   - PRTF nargs: lookahead to text[pc+2] when text[pc+1]==ADJ.
//   - The VM stack is a host malloc'd buffer; grows downward in 8-byte slots.

#include "jit_common.h"

// ---------------------------------------------------------------------------
// Exactly one of JIT_X86 or JIT_ARM64 must be defined at compile time.
// ---------------------------------------------------------------------------
#if !defined(JIT_X86) && !defined(JIT_ARM64)
#  error "Define JIT_X86 or JIT_ARM64 at compile time (-DJIT_X86 / -DJIT_ARM64)"
#endif
#if defined(JIT_X86) && defined(JIT_ARM64)
#  error "Define only one of JIT_X86 or JIT_ARM64"
#endif

// ===========================================================================
//  x86-64 backend
// ===========================================================================
#ifdef JIT_X86

// jitmap[vm_pc] -> native byte address of that VM instruction
static unsigned char **jitmap;
static unsigned char  *je;   // JIT emit cursor

// ---- byte-level emit helpers -----------------------------------------------
static void e1(unsigned char a){*je++=a;}
static void e2(unsigned char a,unsigned char b){*je++=a;*je++=b;}
static void e3(unsigned char a,unsigned char b,unsigned char c){*je++=a;*je++=b;*je++=c;}
static void e4(unsigned char a,unsigned char b,unsigned char c,unsigned char d){
  *je++=a;*je++=b;*je++=c;*je++=d;}
static void ei32(int v){
  *je++=(unsigned char)v;*je++=(unsigned char)(v>>8);
  *je++=(unsigned char)(v>>16);*je++=(unsigned char)(v>>24);}
static void ei64(long v){
  unsigned long u=(unsigned long)v;
  *je++=u;*je++=u>>8;*je++=u>>16;*je++=u>>24;
  *je++=u>>32;*je++=u>>40;*je++=u>>48;*je++=u>>56;}

// movabs $v into common registers
static void mrax(long v){e2(0x48,0xB8);ei64(v);}
static void mr11(long v){e2(0x49,0xBB);ei64(v);}
static void mrsi(long v){e2(0x48,0xBE);ei64(v);}
static void mrcx(long v){e2(0x48,0xB9);ei64(v);}
static void mrdi(long v){e2(0x48,0xBF);ei64(v);}

// ---- VM stack ops ----------------------------------------------------------
// Registers: %r12=vm_sp  %r13=data_base  %r15=vm_bp  %rax=accumulator
// push %rax -> VM stack
static void vpush(void){e4(0x49,0x83,0xEC,0x08);e4(0x49,0x89,0x04,0x24);}
// pop VM stack -> %rcx
static void vpop_rcx(void){e4(0x49,0x8B,0x0C,0x24);e4(0x49,0x83,0xC4,0x08);}
// peek VM sp[n*8] -> %rcx  (n = 0,1,2)
static void vpeek(int n){
  if(n==0){e4(0x49,0x8B,0x0C,0x24);}
  else{e1(0x49);e1(0x8B);e1(0x4C);e1(0x24);e1((unsigned char)(n*8));}}

// ---- to_addr: if 0 <= val < poolsz: val += %r13 (data_base) ---------------
// Operates in %rdi; clobbers %rsi as temp.
static void toa_rdi(void){
  e3(0x48,0x85,0xFF);            // test %rdi,%rdi
  e2(0x7C,0x0E);                 // jl  skip (+14)
  e2(0x48,0xBE);ei64(g_poolsz); // movabs $psz,%rsi
  e3(0x48,0x39,0xF7);            // cmp %rsi,%rdi
  e2(0x7D,0x03);                 // jge skip (+3)
  e3(0x4C,0x01,0xEF);}           // add %r13,%rdi
static void toa_rax(void){e3(0x48,0x89,0xC7);toa_rdi();e3(0x48,0x89,0xF8);}
static void toa_rcx_rdi(void){e3(0x48,0x89,0xCF);toa_rdi();}
static void toa_rcx_rsi(void){toa_rcx_rdi();e3(0x48,0x89,0xFE);}

// ---- call %r11, 16-byte-aligned -------------------------------------------
static void ccall(void){e1(0x53);e3(0x41,0xFF,0xD3);e1(0x5B);}
static void cfn(void *fn){mr11((long)fn);ccall();}

// ---- LEV indirect jump: jmp jitmap[%r11] ----------------------------------
static void lev_jmp(void){
  e2(0x4D,0xBA);ei64((long)(void*)jitmap); // movabs $jitmap,%r10
  e4(0x4B,0xFF,0x24,0xDA);}                // jmp *(%r10,%r11,8)

// ---- standard callee-save epilogue ----------------------------------------
static void do_epilogue(void){
  e1(0x5B);e2(0x41,0x5F);e2(0x41,0x5E);e2(0x41,0x5D);e2(0x41,0x5C);
  e1(0x5D);e1(0xC3);}

// ---- compile one VM instruction -------------------------------------------
static long compile_one(long pc) {
  long op = g_text[pc];

  jitmap[pc] = je;
  if ((op>=LLA&&op<=ADJ)||op==IMMF)
    if (pc+1 < g_tlen) jitmap[pc+1] = je;

  if (g_debug) {
    static const char *NM[] = {
      "LLA","IMM","JMP","JSR","BZ","BNZ","ENT","ADJ","LEV","LI","LC","SI","SC","PSH",
      "OR","XOR","AND","EQ","NE","LT","GT","LE","GE","SHL","SHR","ADD","SUB","MUL",
      "DIV","MOD","LF","SF","IMMF","ITF","ITFS","FTI","FADD","FSUB","FMUL","FDIV",
      "FEQ","FNE","FLT","FGT","FLE","FGE","PRTF_DBL","OPEN","READ","WRIT","CLOS",
      "PRTF","MALC","FREE","MSET","MCMP","EXIT"};
    if (op>=0&&op<=EXIT) {
      fprintf(stderr,"[%4ld] %s",pc,NM[op]);
      if ((op>=LLA&&op<=ADJ)||op==IMMF) fprintf(stderr," %ld",g_text[pc+1]);
      fputc('\n',stderr);
    }
  }

  if (op==LLA) {
    long n=g_text[++pc]; long off=n*8;
    if (off>=-128&&off<=127){e4(0x49,0x8D,0x47,(unsigned char)(off&0xFF));}
    else{e3(0x49,0x8D,0x87);ei32((int)off);}
  }
  else if (op==IMM)  { mrax(g_text[++pc]); }
  else if (op==JMP)  { pc++; e1(0xE9);ei32(0); }
  else if (op==JSR)  { long callee=g_text[++pc]; mrax(pc+1); vpush(); (void)callee; e1(0xE9);ei32(0); }
  else if (op==BZ)   { pc++; e3(0x48,0x85,0xC0); e2(0x0F,0x84);ei32(0); }
  else if (op==BNZ)  { pc++; e3(0x48,0x85,0xC0); e2(0x0F,0x85);ei32(0); }
  else if (op==ENT) {
    long n=g_text[++pc];
    e4(0x49,0x83,0xEC,0x08); e3(0x4D,0x89,0x3C); e1(0x24); // push bp
    e3(0x4D,0x89,0xE7);                                      // bp = sp
    if (n>0) {
      long sz=n*8;
      if (sz<=127) e4(0x49,0x83,0xEC,(unsigned char)(sz&0xFF));
      else{e3(0x49,0x81,0xEC);ei32((int)sz);}
    }
  }
  else if (op==ADJ) {
    long n=g_text[++pc]; long sz=n*8;
    if (sz>0) {
      if (sz<=127) e4(0x49,0x83,0xC4,(unsigned char)(sz&0xFF));
      else{e3(0x49,0x81,0xC4);ei32((int)sz);}
    } else if (sz<0) {
      long isz=-sz;
      if (isz<=127) e4(0x49,0x83,0xEC,(unsigned char)(isz&0xFF));
      else{e3(0x49,0x81,0xEC);ei32((int)isz);}
    }
  }
  else if (op==LEV) {
    e3(0x4D,0x89,0xFC);            // sp = bp
    e3(0x4D,0x8B,0x3C); e1(0x24); // pop bp
    e4(0x49,0x83,0xC4,0x08);
    e4(0x4D,0x8B,0x1C,0x24);       // pop ret-idx -> r11
    e4(0x49,0x83,0xC4,0x08);
    lev_jmp();
  }
  else if (op==LI)  { toa_rax(); e3(0x48,0x8B,0x00); }
  else if (op==LC)  { toa_rax(); e3(0x0F,0xB6,0x00); }
  else if (op==SI)  { e1(0x50); vpop_rcx(); toa_rcx_rdi(); e1(0x59); e3(0x48,0x89,0x0F); }
  else if (op==SC)  { e1(0x50); vpop_rcx(); toa_rcx_rdi(); e1(0x59);
                      e2(0x88,0x0F); e3(0x0F,0xB6,0xC1); }
  else if (op==PSH) { vpush(); }

  else if (op==OR)  { vpop_rcx(); e3(0x48,0x09,0xC8); }
  else if (op==XOR) { vpop_rcx(); e3(0x48,0x31,0xC8); }
  else if (op==AND) { vpop_rcx(); e3(0x48,0x21,0xC8); }
  else if (op==ADD) { vpop_rcx(); e3(0x48,0x01,0xC8); }
  else if (op==SUB) { vpop_rcx(); e3(0x48,0x29,0xC1); e3(0x48,0x89,0xC8); }
  else if (op==MUL) { vpop_rcx(); e4(0x48,0x0F,0xAF,0xC1); }
  else if (op==DIV) { vpop_rcx(); e3(0x48,0x87,0xC1); e2(0x48,0x99); e3(0x48,0xF7,0xF9); }
  else if (op==MOD) { vpop_rcx(); e3(0x48,0x87,0xC1); e2(0x48,0x99); e3(0x48,0xF7,0xF9);
                      e3(0x48,0x89,0xD0); }
  else if (op==SHL) { vpop_rcx(); e3(0x48,0x87,0xC1); e3(0x48,0xD3,0xE0); }
  else if (op==SHR) { vpop_rcx(); e3(0x48,0x87,0xC1); e3(0x48,0xD3,0xF8); }
  else if (op>=EQ&&op<=GE) {
    unsigned char CC[]={0x94,0x95,0x9C,0x9F,0x9E,0x9D};
    vpop_rcx();
    e3(0x48,0x31,0xD2);             // xor %rdx,%rdx
    e3(0x48,0x39,0xC1);             // cmp %rax,%rcx  (left - right)
    e3(0x0F,CC[op-EQ],0xD2);        // setXX %dl
    e3(0x48,0x89,0xD0);             // mov %rdx,%rax
  }

  // ---- Floating-point (raw bits travel in %rax / %rcx) -------------------
  else if (op==IMMF) { mrax(g_text[++pc]); }
  else if (op==LF) {
    toa_rax();
    e4(0xF2,0x0F,0x10,0x00);       // movsd (%rax),%xmm0
    e4(0x66,0x48,0x0F,0x7E); e1(0xC0); // movq %xmm0,%rax
  }
  else if (op==SF) {
    e1(0x50); vpop_rcx(); toa_rcx_rdi(); e1(0x58);
    e4(0x66,0x48,0x0F,0x6E); e1(0xC0); // movq %rax,%xmm0
    e4(0xF2,0x0F,0x11,0x07);           // movsd %xmm0,(%rdi)
  }
  else if (op==ITF) {
    e4(0xF2,0x48,0x0F,0x2A); e1(0xC0); // cvtsi2sdq %rax,%xmm0
    e4(0x66,0x48,0x0F,0x7E); e1(0xC0);
  }
  else if (op==ITFS) {
    e4(0x49,0x8B,0x0C,0x24);           // mov (%r12),%rcx
    e4(0xF2,0x48,0x0F,0x2A); e1(0xC1); // cvtsi2sdq %rcx,%xmm0
    e4(0x66,0x48,0x0F,0x7E); e1(0xC1); // movq %xmm0,%rcx
    e4(0x49,0x89,0x0C,0x24);           // mov %rcx,(%r12)
  }
  else if (op==FTI) {
    e4(0x66,0x48,0x0F,0x6E); e1(0xC0);
    e4(0xF2,0x48,0x0F,0x2C); e1(0xC0); // cvttsd2si %xmm0,%rax
  }
  else if (op==FADD||op==FSUB||op==FMUL||op==FDIV) {
    vpop_rcx();
    e4(0x66,0x48,0x0F,0x6E); e1(0xC1); // movq %rcx,%xmm0 (left)
    e4(0x66,0x48,0x0F,0x6E); e1(0xC8); // movq %rax,%xmm1 (right)
    if      (op==FADD) e4(0xF2,0x0F,0x58,0xC1);
    else if (op==FSUB) e4(0xF2,0x0F,0x5C,0xC1);
    else if (op==FMUL) e4(0xF2,0x0F,0x59,0xC1);
    else               e4(0xF2,0x0F,0x5E,0xC1);
    e4(0x66,0x48,0x0F,0x7E); e1(0xC0);
  }
  else if (op>=FEQ&&op<=FGE) {
    unsigned char FC[]={0x94,0x95,0x92,0x97,0x96,0x93};
    vpop_rcx();
    e4(0x66,0x48,0x0F,0x6E); e1(0xC1); // movq %rcx,%xmm0 (left)
    e4(0x66,0x48,0x0F,0x6E); e1(0xC8); // movq %rax,%xmm1 (right)
    e3(0x48,0x31,0xD2);                 // xor %rdx,%rdx
    e4(0x66,0x0F,0x2E,0xC1);            // ucomisd %xmm1,%xmm0
    e3(0x0F,FC[op-FEQ],0xD2);
    e3(0x48,0x89,0xD0);                 // mov %rdx,%rax
  }

  // ---- System calls -------------------------------------------------------
  else if (op==OPEN) {
    vpeek(2); toa_rcx_rdi(); e1(0x57);
    e1(0x49);e1(0x8B);e1(0x74);e1(0x24);e1(0x08); // mov 8(%r12),%rsi
    vpeek(0); e3(0x48,0x89,0xCA);
    e1(0x5F); cfn((void*)open);
    e3(0x48,0x63,0xC0);
  }
  else if (op==READ) {
    vpeek(2); e3(0x48,0x89,0xCF); e1(0x57);
    vpeek(1); toa_rcx_rsi();
    vpeek(0); e3(0x48,0x89,0xCA);
    e1(0x5F); cfn((void*)read);
    e3(0x48,0x63,0xC0);
  }
  else if (op==WRIT) {
    vpeek(2); e3(0x48,0x89,0xCF); e1(0x57);
    vpeek(1); toa_rcx_rsi();
    vpeek(0); e3(0x48,0x89,0xCA);
    e1(0x5F); cfn((void*)write);
    e3(0x48,0x63,0xC0);
  }
  else if (op==CLOS) { vpeek(0); e3(0x48,0x89,0xCF); cfn((void*)close); e3(0x48,0x63,0xC0); }
  else if (op==PRTF) {
    long nargs=(pc+2<g_tlen&&g_text[pc+1]==ADJ)?g_text[pc+2]:((pc+1<g_tlen)?g_text[pc+1]:1);
    e3(0x4C,0x89,0xE7); mrsi(nargs); e3(0x4C,0x89,0xEA); mrcx(g_poolsz);
    cfn((void*)prtf_shim);
  }
  else if (op==MALC)    { vpeek(0); e3(0x48,0x89,0xCF); cfn((void*)malloc); }
  else if (op==FREE_OP) { vpeek(0); e3(0x48,0x89,0xCF); cfn((void*)free); }
  else if (op==MSET) {
    vpeek(2); toa_rcx_rdi(); e1(0x57);
    vpeek(1); e3(0x48,0x89,0xCE);
    vpeek(0); e3(0x48,0x89,0xCA);
    e1(0x5F); cfn((void*)memset);
  }
  else if (op==MCMP) {
    vpeek(2); toa_rcx_rdi(); e1(0x57);
    vpeek(1); toa_rcx_rsi();
    vpeek(0); e3(0x48,0x89,0xCA);
    e1(0x5F); cfn((void*)memcmp);
    e3(0x48,0x63,0xC0);
  }
  else if (op==EXIT) { vpeek(0); e3(0x48,0x89,0xC8); do_epilogue(); }
  else if (op==PRTF_DBL) { /* placeholder, unused */ }
  else { fprintf(stderr,"JIT x86: unknown opcode %ld at pc=%ld\n",op,pc); e2(0x0F,0x0B); }

  return pc;
}

// ---- x86-64 JIT driver (pass1 + pass2 + run) ------------------------------
static int jit_run(long main_off, int argc, char **argv) {
  long jmem_sz = 64*1024*1024;
  unsigned char *jitmem = (unsigned char*)mmap(NULL, jmem_sz,
    PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANON, -1, 0);
  if (jitmem == MAP_FAILED) { perror("mmap"); return 1; }

  jitmap = (unsigned char**)calloc(g_tlen+4, sizeof(unsigned char*));
  je = jitmem;

  // Entry wrapper: long entry(long *vm_sp, char *d_base, long *unused)
  //                rdi=vm_sp  rsi=d_base
  unsigned char *entry = je;
  e1(0x55); e3(0x48,0x89,0xE5);   // push %rbp; mov %rsp,%rbp
  e2(0x41,0x54); e2(0x41,0x55);   // push %r12; push %r13
  e2(0x41,0x56); e2(0x41,0x57);   // push %r14; push %r15
  e1(0x53);                        // push %rbx  (6 pushes -> 16-aligned)
  e3(0x49,0x89,0xFC);              // mov %rdi,%r12  (vm_sp)
  e3(0x49,0x89,0xF5);              // mov %rsi,%r13  (d_base)
  e3(0x4D,0x89,0xE7);              // mov %r12,%r15  (initial bp)
  e3(0x48,0x31,0xC0);              // xor %rax,%rax
  unsigned char *entry_jmp = je;
  e1(0xE9); ei32(0);               // jmp <main> -- patched after pass 1

  // Pass 1: compile (start at pc=1, skip padding at 0)
  jitmap[0] = je; e2(0x0F,0x0B);  // ud2 sentinel for slot 0
  for (long pc=1; pc<g_tlen;)
    pc = compile_one(pc) + 1;
  e3(0x48,0x31,0xC0); do_epilogue(); // fallthrough sentinel

  // Pass 2: patch branch targets
  for (long pc=1; pc<g_tlen;) {
    long op = g_text[pc];
    if (op<0||op>EXIT) { pc++; continue; }
    unsigned char *nat = jitmap[pc];

    if (op==JMP) {
      long tgt=g_text[pc+1];
      unsigned char *p=nat+1; int rel=(int)(jitmap[tgt]-(p+4));
      p[0]=rel;p[1]=rel>>8;p[2]=rel>>16;p[3]=rel>>24; pc+=2;
    } else if (op==JSR) {
      long tgt=g_text[pc+1];
      // layout: mrax(10) + vpush(8) + e9(1) + imm32(4); patch at nat+19
      unsigned char *p=nat+19; int rel=(int)(jitmap[tgt]-(p+4));
      p[0]=rel;p[1]=rel>>8;p[2]=rel>>16;p[3]=rel>>24; pc+=2;
    } else if (op==BZ||op==BNZ) {
      long tgt=g_text[pc+1];
      // layout: test(3) + 0F 84/85(2) + imm32(4); patch at nat+5
      unsigned char *p=nat+5; int rel=(int)(jitmap[tgt]-(p+4));
      p[0]=rel;p[1]=rel>>8;p[2]=rel>>16;p[3]=rel>>24; pc+=2;
    } else if ((op>=LLA&&op<=ADJ)||op==IMMF) { pc+=2; }
    else { pc++; }
  }

  // Patch entry jump to main
  { unsigned char *p=entry_jmp+1;
    int rel=(int)(jitmap[main_off]-(p+4));
    p[0]=rel;p[1]=rel>>8;p[2]=rel>>16;p[3]=rel>>24; }

  if (g_debug) fprintf(stderr,"JIT x86: %ld native bytes\n",(long)(je-jitmem));

  // Setup VM stack and run
  long vm_stk_sz = 1024*1024;
  long *vm_stk   = (long*)malloc(vm_stk_sz);
  long *vsp      = (long*)((char*)vm_stk + vm_stk_sz);
  long exit_idx  = g_tlen - 2;
  *--vsp = (long)argc;
  *--vsp = (long)argv;
  *--vsp = exit_idx;

  typedef long(*jfn_t)(long*,char*,long*);
  return (int)((jfn_t)(void*)entry)(vsp, g_data, g_text);
}

#endif // JIT_X86


// ===========================================================================
//  AArch64 backend (Apple M-series and Linux ARM64)
// ===========================================================================
#ifdef JIT_ARM64

#include <stdint.h>
#ifdef __APPLE__
#  include <pthread.h>
#  include <libkern/OSCacheControl.h>
#endif

// AArch64 register aliases (callee-saved: x19-x28)
#define VA      19   // VM accumulator
#define VSP     20   // VM stack pointer
#define VDB     21   // VM data base
#define VBP     22   // VM frame pointer (bp)
#define VJMAP   23   // jitmap base
#define VPSZ    24   // VM pool size
#define S0       9   // scratch 0
#define S1      10   // scratch 1
#define S2      11   // scratch 2
#define CFNREG   8   // C function pointer
#define XZR     31
#define SP_REG  31

static uint32_t **jitmap;
static uint32_t  *je;

static void emit(uint32_t insn) { *je++ = insn; }

// ---- AArch64 instruction encoders -----------------------------------------
// MOV_imm64: always 4 instructions (MOVZ + 3 MOVK) for stable snippet size
static void MOV_imm64(int Rd, long v) {
  unsigned long u = (unsigned long)v;
  emit(0xD2800000|((0<<21)|((u      &0xFFFF)<<5)|Rd));
  emit(0xF2800000|((1<<21)|(((u>>16)&0xFFFF)<<5)|Rd));
  emit(0xF2800000|((2<<21)|(((u>>32)&0xFFFF)<<5)|Rd));
  emit(0xF2800000|((3<<21)|(((u>>48)&0xFFFF)<<5)|Rd));
}
static void MOV(int Rd,int Rn){ emit(0xAA0003E0|(Rn<<16)|Rd); }
static void ADD_imm(int Rd,int Rn,int i){ emit(0x91000000|(i<<10)|(Rn<<5)|Rd); }
static void SUB_imm(int Rd,int Rn,int i){ emit(0xD1000000|(i<<10)|(Rn<<5)|Rd); }
static void ADD_reg(int Rd,int Rn,int Rm){ emit(0x8B000000|(Rm<<16)|(Rn<<5)|Rd); }
static void SUB_reg(int Rd,int Rn,int Rm){ emit(0xCB000000|(Rm<<16)|(Rn<<5)|Rd); }
static void MUL_r(int Rd,int Rn,int Rm) { emit(0x9B007C00|(Rm<<16)|(Rn<<5)|Rd); }
static void SDIV(int Rd,int Rn,int Rm)  { emit(0x9AC00C00|(Rm<<16)|(Rn<<5)|Rd); }
static void MSUB(int Rd,int Rn,int Rm,int Ra){ emit(0x9B008000|(Rm<<16)|(Ra<<10)|(Rn<<5)|Rd); }
static void AND_reg(int Rd,int Rn,int Rm){ emit(0x8A000000|(Rm<<16)|(Rn<<5)|Rd); }
static void ORR_reg(int Rd,int Rn,int Rm){ emit(0xAA000000|(Rm<<16)|(Rn<<5)|Rd); }
static void EOR_reg(int Rd,int Rn,int Rm){ emit(0xCA000000|(Rm<<16)|(Rn<<5)|Rd); }
static void LSLV(int Rd,int Rn,int Rm)  { emit(0x9AC02000|(Rm<<16)|(Rn<<5)|Rd); }
static void ASRV(int Rd,int Rn,int Rm)  { emit(0x9AC02800|(Rm<<16)|(Rn<<5)|Rd); }
static void LDR(int Rt,int Rn,int o)    { emit(0xF9400000|((o/8)<<10)|(Rn<<5)|Rt); }
static void STR(int Rt,int Rn,int o)    { emit(0xF9000000|((o/8)<<10)|(Rn<<5)|Rt); }
static void LDRB(int Rt,int Rn)         { emit(0x39400000|(Rn<<5)|Rt); }
static void STRB(int Rt,int Rn)         { emit(0x39000000|(Rn<<5)|Rt); }
static void LDR_lsl3(int Rt,int Rb,int Ri){ emit(0xF8607800|(Ri<<16)|(Rb<<5)|Rt); }
static void CMP(int Rn,int Rm)          { emit(0xEB00001F|(Rm<<16)|(Rn<<5)); }
static void CMP_imm(int Rn,int i)       { emit(0xF100001F|(i<<10)|(Rn<<5)); }
static void CSET(int Rd,int cond)       { emit(0x9A9F07E0|((cond^1)<<12)|Rd); }
#define CC_EQ 0
#define CC_NE 1
#define CC_GE 10
#define CC_LT 11
#define CC_GT 12
#define CC_LE 13
static void SXTW(int Xd,int Wn){ emit(0x93407C00|(Wn<<5)|Xd); }
static void B   (int off){ emit(0x14000000|(off&0x3FFFFFF)); }
static void BLR (int Rn) { emit(0xD63F0000|(Rn<<5)); }
static void BR  (int Rn) { emit(0xD61F0000|(Rn<<5)); }
static void RET (void)   { emit(0xD65F03C0); }
static void CBZ (int Rn,int off){ emit(0xB4000000|((off&0x7FFFF)<<5)|Rn); }
static void CBNZ(int Rn,int off){ emit(0xB5000000|((off&0x7FFFF)<<5)|Rn); }
static void Bcond(int c,int off){ emit(0x54000000|((off&0x7FFFF)<<5)|c); }
// Floating-point
static void FMOV_g2f(int Dd,int Xn){ emit(0x9E670000|(Xn<<5)|Dd); }
static void FMOV_f2g(int Xd,int Dn){ emit(0x9E660000|(Dn<<5)|Xd); }
static void FADD_d(int Dd,int Dn,int Dm){ emit(0x1E602800|(Dm<<16)|(Dn<<5)|Dd); }
static void FSUB_d(int Dd,int Dn,int Dm){ emit(0x1E603800|(Dm<<16)|(Dn<<5)|Dd); }
static void FMUL_d(int Dd,int Dn,int Dm){ emit(0x1E600800|(Dm<<16)|(Dn<<5)|Dd); }
static void FDIV_d(int Dd,int Dn,int Dm){ emit(0x1E601800|(Dm<<16)|(Dn<<5)|Dd); }
static void FCMP_d(int Dn,int Dm)       { emit(0x1E602000|(Dm<<16)|(Dn<<5)); }
static void SCVTF (int Dd,int Xn)       { emit(0x9E620000|(Xn<<5)|Dd); }
static void FCVTZS(int Xd,int Dn)       { emit(0x9E780000|(Dn<<5)|Xd); }
static void LDR_d (int Dt,int Rn)       { emit(0xFD400000|(Rn<<5)|Dt); }
static void STR_d (int Dt,int Rn)       { emit(0xFD000000|(Rn<<5)|Dt); }

// ---- VM stack helpers ------------------------------------------------------
static void vm_push(void){ SUB_imm(VSP,VSP,8); STR(VA,VSP,0); }
static void vm_pop (void){ LDR(S0,VSP,0); ADD_imm(VSP,VSP,8); }
static void vm_peek(int n){ LDR(S0,VSP,n*8); }

// ---- to_addr: if 0 <= x0 < poolsz: x0 += VDB (x21) ----------------------
// Always exactly 5 instructions.
static void to_addr_x0(void){
  CMP_imm(0,0);       // CMP x0, #0
  Bcond(CC_LT,4);     // B.LT +4 insns (skip)
  CMP(0,VPSZ);        // CMP x0, x24
  Bcond(CC_GE,2);     // B.GE +2 insns (skip)
  ADD_reg(0,0,VDB);   // ADD x0, x0, x21
}
static void to_addr_s0(void){ MOV(0,S0); to_addr_x0(); }

// ---- call a C function pointer stored in CFNREG (x8) ---------------------
static void call_fn(void *fn){ MOV_imm64(CFNREG,(long)fn); BLR(CFNREG); }

// ---- compile one VM instruction -------------------------------------------
static long compile_one(long pc) {
  long op = g_text[pc];
  jitmap[pc] = je;
  if ((op>=LLA&&op<=ADJ)||op==IMMF)
    if (pc+1 < g_tlen) jitmap[pc+1] = je;

  if (g_debug) {
    static const char *NM[] = {
      "LLA","IMM","JMP","JSR","BZ","BNZ","ENT","ADJ","LEV","LI","LC","SI","SC","PSH",
      "OR","XOR","AND","EQ","NE","LT","GT","LE","GE","SHL","SHR","ADD","SUB","MUL",
      "DIV","MOD","LF","SF","IMMF","ITF","ITFS","FTI","FADD","FSUB","FMUL","FDIV",
      "FEQ","FNE","FLT","FGT","FLE","FGE","PRTF_DBL","OPEN","READ","WRIT","CLOS",
      "PRTF","MALC","FREE","MSET","MCMP","EXIT"};
    if (op>=0&&op<=EXIT) {
      fprintf(stderr,"[%4ld] %s",pc,NM[op]);
      if ((op>=LLA&&op<=ADJ)||op==IMMF) fprintf(stderr," %ld",g_text[pc+1]);
      fputc('\n',stderr);
    }
  }

  if (op==LLA) {
    long n=g_text[++pc]; long off=n*8;
    if (off>=0&&off<=4095)       ADD_imm(VA,VBP,(int)off);
    else if (off<0&&off>=-4096)  SUB_imm(VA,VBP,(int)(-off));
    else { MOV_imm64(S0,off); ADD_reg(VA,VBP,S0); }
  }
  else if (op==IMM)  { MOV_imm64(VA,g_text[++pc]); }
  else if (op==JMP)  { pc++; B(0); }
  else if (op==JSR)  {
    // Fixed layout: MOV_imm64(4) + vm_push(2) + B(1) = 7 insns
    long callee=g_text[++pc]; (void)callee;
    MOV_imm64(VA,pc+1); vm_push(); B(0);
  }
  else if (op==BZ)   { pc++; CBZ(VA,0); }
  else if (op==BNZ)  { pc++; CBNZ(VA,0); }
  else if (op==ENT) {
    long n=g_text[++pc];
    SUB_imm(VSP,VSP,8); STR(VBP,VSP,0); MOV(VBP,VSP);
    if (n>0) {
      long sz=n*8;
      if (sz<=4095) SUB_imm(VSP,VSP,(int)sz);
      else { MOV_imm64(S0,sz); SUB_reg(VSP,VSP,S0); }
    }
  }
  else if (op==ADJ) {
    long n=g_text[++pc]; long sz=n*8;
    if (sz>0) {
      if (sz<=4095) ADD_imm(VSP,VSP,(int)sz);
      else { MOV_imm64(S0,sz); ADD_reg(VSP,VSP,S0); }
    } else if (sz<0) {
      long isz=-sz;
      if (isz<=4095) SUB_imm(VSP,VSP,(int)isz);
      else { MOV_imm64(S0,isz); SUB_reg(VSP,VSP,S0); }
    }
  }
  else if (op==LEV) {
    MOV(VSP,VBP);
    LDR(VBP,VSP,0); ADD_imm(VSP,VSP,8);  // pop bp
    LDR(S0, VSP,0); ADD_imm(VSP,VSP,8);  // pop ret-idx -> S0
    LDR_lsl3(S1,VJMAP,S0);               // S1 = jitmap[S0]
    BR(S1);
  }
  else if (op==LI)  { MOV(0,VA); to_addr_x0(); LDR(VA,0,0); }
  else if (op==LC)  { MOV(0,VA); to_addr_x0(); LDRB(VA,0); }
  else if (op==SI)  { vm_pop(); to_addr_s0(); STR(VA,0,0); }
  else if (op==SC)  { vm_pop(); to_addr_s0(); STRB(VA,0); emit(0x92401E73u); /* AND x19,#0xFF */ }
  else if (op==PSH) { vm_push(); }

  else if (op==OR)  { vm_pop(); ORR_reg(VA,VA,S0); }
  else if (op==XOR) { vm_pop(); EOR_reg(VA,VA,S0); }
  else if (op==AND) { vm_pop(); AND_reg(VA,VA,S0); }
  else if (op==ADD) { vm_pop(); ADD_reg(VA,S0,VA); }
  else if (op==SUB) { vm_pop(); SUB_reg(VA,S0,VA); }
  else if (op==MUL) { vm_pop(); MUL_r(VA,S0,VA); }
  else if (op==DIV) { vm_pop(); SDIV(VA,S0,VA); }
  else if (op==MOD) { vm_pop(); SDIV(S1,S0,VA); MSUB(VA,S1,VA,S0); }
  else if (op==SHL) { vm_pop(); LSLV(VA,S0,VA); }
  else if (op==SHR) { vm_pop(); ASRV(VA,S0,VA); }
  else if (op>=EQ&&op<=GE) {
    static const int CC[]={CC_EQ,CC_NE,CC_LT,CC_GT,CC_LE,CC_GE};
    vm_pop(); CMP(S0,VA); CSET(VA,CC[op-EQ]);
  }

  // ---- Floating-point (raw double bits travel in x19 / S0) ---------------
  else if (op==IMMF) { MOV_imm64(VA,g_text[++pc]); }
  else if (op==LF)   { MOV(0,VA); to_addr_x0(); LDR(VA,0,0); }
  else if (op==SF)   { vm_pop(); to_addr_s0(); STR(VA,0,0); }
  else if (op==ITF)  { SCVTF(0,VA); FMOV_f2g(VA,0); }
  else if (op==ITFS) { LDR(S0,VSP,0); SCVTF(0,S0); FMOV_f2g(S0,0); STR(S0,VSP,0); }
  else if (op==FTI)  { FMOV_g2f(0,VA); FCVTZS(VA,0); }
  else if (op==FADD||op==FSUB||op==FMUL||op==FDIV) {
    vm_pop(); FMOV_g2f(0,S0); FMOV_g2f(1,VA);
    if      (op==FADD) FADD_d(0,0,1);
    else if (op==FSUB) FSUB_d(0,0,1);
    else if (op==FMUL) FMUL_d(0,0,1);
    else               FDIV_d(0,0,1);
    FMOV_f2g(VA,0);
  }
  else if (op>=FEQ&&op<=FGE) {
    static const int FCC[]={CC_EQ,CC_NE,CC_LT,CC_GT,CC_LE,CC_GE};
    vm_pop(); FMOV_g2f(0,S0); FMOV_g2f(1,VA);
    FCMP_d(0,1); CSET(VA,FCC[op-FEQ]);
  }

  // ---- System calls -------------------------------------------------------
  else if (op==OPEN) {
    vm_peek(2); to_addr_s0(); MOV(S2,0);
    vm_peek(1); MOV(S1,S0);
    vm_peek(0);
    MOV(0,S2); MOV(1,S1); MOV(2,S0);
    call_fn((void*)open); SXTW(0,0); MOV(VA,0);
  }
  else if (op==READ) {
    vm_peek(2); MOV(S2,S0);
    vm_peek(1); to_addr_s0(); MOV(S1,0);
    vm_peek(0);
    MOV(0,S2); MOV(1,S1); MOV(2,S0);
    call_fn((void*)read); SXTW(0,0); MOV(VA,0);
  }
  else if (op==WRIT) {
    vm_peek(2); MOV(S2,S0);
    vm_peek(1); to_addr_s0(); MOV(S1,0);
    vm_peek(0);
    MOV(0,S2); MOV(1,S1); MOV(2,S0);
    call_fn((void*)write); SXTW(0,0); MOV(VA,0);
  }
  else if (op==CLOS) {
    vm_peek(0); MOV(0,S0); call_fn((void*)close); SXTW(0,0); MOV(VA,0);
  }
  else if (op==PRTF) {
    long nargs=(pc+2<g_tlen&&g_text[pc+1]==ADJ)?g_text[pc+2]:(pc+1<g_tlen?g_text[pc+1]:1);
    MOV(0,VSP); MOV_imm64(1,nargs); MOV(2,VDB); MOV_imm64(3,g_poolsz);
    call_fn((void*)prtf_shim); MOV(VA,0);
  }
  else if (op==MALC)    { vm_peek(0); MOV(0,S0); call_fn((void*)malloc); MOV(VA,0); }
  else if (op==FREE_OP) { vm_peek(0); MOV(0,S0); call_fn((void*)free); }
  else if (op==MSET) {
    vm_peek(2); to_addr_s0(); MOV(S2,0);
    vm_peek(1); MOV(S1,S0);
    vm_peek(0);
    MOV(0,S2); MOV(1,S1); MOV(2,S0);
    call_fn((void*)memset); MOV(VA,0);
  }
  else if (op==MCMP) {
    vm_peek(2); to_addr_s0(); MOV(S2,0);
    vm_peek(1); to_addr_s0(); MOV(S1,0);
    vm_peek(0);
    MOV(0,S2); MOV(1,S1); MOV(2,S0);
    call_fn((void*)memcmp); SXTW(0,0); MOV(VA,0);
  }
  else if (op==EXIT) {
    // 3-insn snippet: vm_peek(0) + MOV x0,S0 + B epilogue (patched later)
    vm_peek(0); MOV(0,S0); B(0);
  }
  else if (op==PRTF_DBL) { /* placeholder, unused */ }
  else { fprintf(stderr,"JIT arm64: unknown opcode %ld at pc=%ld\n",op,pc); emit(0xD4200000u); }

  return pc;
}

// ---- AArch64 JIT driver ---------------------------------------------------
static int jit_run(long main_off, int argc, char **argv) {
  long jmem_sz = 64*1024*1024;
  int mmap_flags = MAP_PRIVATE|MAP_ANON;
#ifdef MAP_JIT
  mmap_flags |= MAP_JIT;
#endif
  void *jitmem = mmap(NULL,jmem_sz,PROT_READ|PROT_WRITE|PROT_EXEC,mmap_flags,-1,0);
  if (jitmem==MAP_FAILED) { perror("mmap"); return 1; }

  jitmap = (uint32_t**)calloc(g_tlen+4, sizeof(uint32_t*));
  je = (uint32_t*)jitmem;

#if defined(__APPLE__) && defined(__aarch64__)
  pthread_jit_write_protect_np(0);
#endif

  // Entry wrapper: long entry(long *vm_sp, char *d_base, long *unused)
  //                x0=vm_sp  x1=d_base
  uint32_t *entry = je;
  emit(0xA9BB53F3u); // STP x19,x20,[sp,#-80]!
  emit(0xA9015BF5u); // STP x21,x22,[sp,#16]
  emit(0xA90263F7u); // STP x23,x24,[sp,#32]
  emit(0xA9036BF9u); // STP x25,x26,[sp,#48]
  emit(0xA9047BFDu); // STP x29,x30,[sp,#64]
  MOV(29,SP_REG);
  MOV(VSP,0); MOV(VDB,1); MOV(VBP,VSP);
  MOV_imm64(VA,0); MOV_imm64(VPSZ,g_poolsz);

  uint32_t *jmap_patch = je;
  MOV_imm64(VJMAP,0);   // patched after pass 1
  uint32_t *entry_jmp = je;
  B(0);                 // jump to main, patched after pass 1

  // Shared epilogue (EXIT snippets branch here; x0 = exit code)
  uint32_t *epilogue = je;
  emit(0xA9447BFDu); emit(0xA9436BF9u); emit(0xA94263F7u);
  emit(0xA9415BF5u); emit(0xA8C553F3u);
  RET();

  // Pass 1
  jitmap[0] = je; emit(0xD4200000u); // BRK #0 sentinel
  for (long pc=1; pc<g_tlen;)
    pc = compile_one(pc) + 1;
  MOV_imm64(0,0); B((int)(epilogue-je)); // fallthrough sentinel

  // Patch EXIT branch targets
  for (long pc=1; pc<g_tlen;) {
    long op=g_text[pc];
    if (op==EXIT) {
      uint32_t *b_insn = jitmap[pc]+2;
      int rel=(int)(epilogue-b_insn);
      *b_insn = 0x14000000u|(rel&0x3FFFFFF);
    }
    if ((op>=LLA&&op<=ADJ)||op==IMMF) pc+=2; else pc++;
  }

  // Patch jitmap base into entry wrapper
  { uint32_t *save=je; je=jmap_patch; MOV_imm64(VJMAP,(long)(void*)jitmap); je=save; }

  // Pass 2: patch branch targets
  for (long pc=1; pc<g_tlen;) {
    long op=g_text[pc];
    if (op<0||op>EXIT) { pc++; continue; }
    uint32_t *nat=jitmap[pc];

    if (op==JMP) {
      long tgt=g_text[pc+1];
      int rel=(int)(jitmap[tgt]-nat);
      nat[0]=0x14000000u|(rel&0x3FFFFFF); pc+=2;
    } else if (op==JSR) {
      long tgt=g_text[pc+1];
      // JSR: 4+2+1=7 insns; B at nat[6]
      int rel=(int)(jitmap[tgt]-(nat+6));
      nat[6]=0x14000000u|(rel&0x3FFFFFF); pc+=2;
    } else if (op==BZ) {
      long tgt=g_text[pc+1];
      int rel=(int)(jitmap[tgt]-nat);
      nat[0]=0xB4000000u|((rel&0x7FFFF)<<5)|VA; pc+=2;
    } else if (op==BNZ) {
      long tgt=g_text[pc+1];
      int rel=(int)(jitmap[tgt]-nat);
      nat[0]=0xB5000000u|((rel&0x7FFFF)<<5)|VA; pc+=2;
    } else if ((op>=LLA&&op<=ADJ)||op==IMMF) { pc+=2; }
    else { pc++; }
  }

  // Patch entry jump to main
  { int rel=(int)(jitmap[main_off]-entry_jmp); *entry_jmp=0x14000000u|(rel&0x3FFFFFF); }

  if (g_debug) fprintf(stderr,"JIT arm64: %ld insns emitted\n",(long)(je-(uint32_t*)jitmem));

#if defined(__APPLE__) && defined(__aarch64__)
  pthread_jit_write_protect_np(1);
  sys_icache_invalidate(jitmem,(char*)je-(char*)jitmem);
#endif

  // Setup VM stack and run
  long vm_stk_sz=1024*1024;
  long *vm_stk=(long*)malloc(vm_stk_sz);
  long *vsp=(long*)((char*)vm_stk+vm_stk_sz);
  long exit_idx=g_tlen-2;
  *--vsp=(long)argc;
  *--vsp=(long)argv;
  *--vsp=exit_idx;

  typedef long(*jfn_t)(long*,char*,long*);
  return (int)((jfn_t)(void*)entry)(vsp,g_data,g_text);
}

#endif // JIT_ARM64


// ===========================================================================
//  main (shared by both backends)
// ===========================================================================
int main(int argc, char **argv) {
  int ao = 1;
  if (argc < 2) {
    fprintf(stderr,"Usage: jit [-d] <program.elf> [args...]\n"); return 1;
  }
  if (!strcmp(argv[1],"-d")) { g_debug=1; ao=2; }
  if (ao >= argc) {
    fprintf(stderr,"Usage: jit [-d] <program.elf> [args...]\n"); return 1;
  }

  long main_off = load_elf(argv[ao]);
  if (main_off < 0) return 1;

  return jit_run(main_off, argc-ao, argv+ao);
}
