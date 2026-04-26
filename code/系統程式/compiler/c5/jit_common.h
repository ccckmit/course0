// jit_common.h - Shared code for jit.c (x86-64 and AArch64 backends)
//
// Provides:
//   - Opcode enum           (must match c5.c / c5_tool.c exactly)
//   - Global state          (g_debug, g_poolsz, g_text, g_data, g_tlen)
//   - ELF helpers           (r16 / r32 / r64)
//   - prtf_shim             (identical on both architectures)
//   - load_elf()            (parse ELF, load text/data, find main)
//
// Usage: #include "jit_common.h" once at the top of jit.c, before
// the architecture-specific backend section.

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// ---- c5 VM opcodes (must match c5.c / c5_tool.c) -------------------------
enum {
  LLA,IMM,JMP,JSR,BZ,BNZ,ENT,ADJ,LEV,LI,LC,SI,SC,PSH,
  OR,XOR,AND,EQ,NE,LT,GT,LE,GE,SHL,SHR,ADD,SUB,MUL,DIV,MOD,
  LF,SF,IMMF,ITF,ITFS,FTI,FADD,FSUB,FMUL,FDIV,
  FEQ,FNE,FLT,FGT,FLE,FGE,PRTF_DBL,
  OPEN,READ,WRIT,CLOS,PRTF,MALC,FREE_OP,MSET,MCMP,EXIT
};

// ---- Shared globals -------------------------------------------------------
static int    g_debug;
static long   g_poolsz;
static long  *g_text;
static char  *g_data;
static long   g_tlen;

// ---- ELF little-endian helpers -------------------------------------------
static long r16(char *b, long p) {
  return (unsigned char)b[p] | ((unsigned char)b[p+1] << 8);
}
static long r32(char *b, long p) {
  return r16(b,p) | (r16(b,p+2) << 16);
}
static long r64(char *b, long p) {
  return (long)( (unsigned long)(unsigned int)r32(b,p)
               | ((unsigned long)(unsigned int)r32(b,p+4) << 32) );
}

// ---- printf shim (called by JIT'd PRTF snippets) -------------------------
// Signature: long prtf_shim(long *vm_sp, long nargs, char *d_base, long poolsz)
static long prtf_shim(long *vsp, long nargs, char *db, long psz) {
  long *t = vsp + nargs;
  long  fr = t[-1];
  char *fmt = (fr >= 0 && fr < psz) ? db + fr : (char *)fr;
  long fa[8];
  for (int k = 0; k < 8; k++) fa[k] = (k < nargs-1) ? t[-2-k] : 0;

  char *f = fmt; long ai = 0, a = 0;
  while (*f) {
    if (*f != '%') { putchar(*f++); a++; continue; }
    char sp[64], *s = sp; *s++ = *f++;
    while (*f=='-'||*f=='+'||*f==' '||*f=='#'||*f=='0') *s++ = *f++;
    while (*f>='0' && *f<='9') *s++ = *f++;
    if (*f == '.') { *s++ = *f++; while (*f>='0'&&*f<='9') *s++ = *f++; }
    if (*f=='l'||*f=='h'||*f=='L') *s++ = *f++;
    if (*f=='l') *s++ = *f++;
    long cv = *f ? *f++ : 0; *s++ = (char)cv; *s = 0;
    long v = (ai < 8) ? fa[ai] : 0;
    if (cv=='f'||cv=='g'||cv=='e'||cv=='F'||cv=='G'||cv=='E') {
      double d; memcpy(&d, &v, 8); a += printf(sp, d);
    } else if (cv == '%') {
      a += printf("%%");
    } else if (cv == 's') {
      char *sv = (v>=0&&v<psz) ? db+v : (char *)v; a += printf(sp, sv);
    } else {
      a += printf(sp, v);
    }
    if (cv != '%') ai++;
  }
  return a;
}

// ---- ELF loader ----------------------------------------------------------
// Reads filename, fills g_text / g_data / g_tlen.
// Returns main's text long-index on success, -1 on error.
static long load_elf(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd < 0) { perror(filename); return -1; }

  g_poolsz = 4 * 1024 * 1024;
  char *fb = (char *)malloc(g_poolsz);
  long  fsz = read(fd, fb, g_poolsz-1); close(fd);

  if (fsz < 64) { fprintf(stderr, "file too small\n"); return -1; }
  if ((unsigned char)fb[0] != 0x7f || fb[1]!='E' || fb[2]!='L' || fb[3]!='F') {
    fprintf(stderr, "not ELF\n"); return -1;
  }

  long eshoff    = r64(fb, 40);
  long eshnum    = r16(fb, 60);
  long eshstrndx = r16(fb, 62);
  char *shstr    = fb + r64(fb, eshoff + eshstrndx*64 + 24);

  long toff=0, tsz=0, doff=0, dsz=0, soff=0, ssz=0, stroff=0;
  for (long si = 0; si < eshnum; si++) {
    long b   = eshoff + si*64;
    long shn = r32(fb, b);
    long sho = r64(fb, b+24);
    long shs = r64(fb, b+32);
    long slk = r32(fb, b+40);
    char *n  = shstr + shn;
    if      (!strcmp(n, ".text"))   { toff=sho; tsz=shs; }
    else if (!strcmp(n, ".data"))   { doff=sho; dsz=shs; }
    else if (!strcmp(n, ".symtab")) { soff=sho; ssz=shs;
                                      stroff=r64(fb, eshoff+slk*64+24); }
  }
  if (!tsz) { fprintf(stderr, "no .text section\n"); return -1; }

  g_tlen = tsz / sizeof(long);
  g_text = (long *)calloc(g_tlen + 4, sizeof(long));
  g_data = (char *)calloc(dsz + 16, 1);
  memcpy(g_text, fb + toff, tsz);
  if (dsz > 0) memcpy(g_data, fb + doff, dsz);

  long main_off = -1;
  for (long i = 0; i < ssz/24; i++) {
    long p = soff + i*24;
    if (!strcmp(fb + stroff + r32(fb,p), "main")) {
      main_off = r64(fb, p+8); break;
    }
  }
  if (main_off < 0) { fprintf(stderr, "'main' not found\n"); return -1; }

  if (g_debug)
    fprintf(stderr, "ELF: text=%ld longs, data=%ld bytes, main@%ld\n",
            g_tlen, dsz, main_off);
  free(fb);
  return main_off;
}
