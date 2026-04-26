// c5tool.c - Standalone ELF Loader, Virtual Machine, and Linker (Merged)
// Self-hostable: written in the subset of C that c5 understands.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <memory.h>

// -------------------------------------------------------
// Opcodes (must match c5.c exactly)
// -------------------------------------------------------
enum { LLA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       LF  ,SF  ,IMMF,ITF ,ITFS,FTI ,FADD,FSUB,FMUL,FDIV,
       FEQ ,FNE ,FLT ,FGT ,FLE ,FGE ,PRTF_DBL,
       OPEN,READ,WRIT,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT };

// Rela types (must match c5.c exactly)
enum { RELA_DATA = 1, RELA_FUNC = 2, RELA_JMP = 3 };

// ELF shndx
enum { SHN_UNDEF = 0 };

char *instr_name;
long debug;
long poolsz;

// -------------------------------------------------------
// Helper: little-endian read/write
// -------------------------------------------------------
long r16(char *buf, long p) { return (buf[p] & 0xFF) | ((buf[p+1] & 0xFF) << 8); }
long r32(char *buf, long p) { return r16(buf,p) | (r16(buf,p+2) << 16); }
long r64(char *buf, long p) { return r32(buf,p) | (r32(buf,p+4) << 32); }
void w16(char *buf, long p, long v) { buf[p]=v; buf[p+1]=v>>8; }
void w32(char *buf, long p, long v) { buf[p]=v; buf[p+1]=v>>8; buf[p+2]=v>>16; buf[p+3]=v>>24; }
void w64(char *buf, long p, long v) { w32(buf,p,v); w32(buf,p+4,v>>32); }

// -------------------------------------------------------
// mem_cpy (avoid libc dependency when self-hosted)
// -------------------------------------------------------
void mem_cpy(char *dst, char *src, long n) {
  long i; i=0; while (i<n) { dst[i]=src[i]; i=i+1; }
}

// -------------------------------------------------------
// VM execution functions
// -------------------------------------------------------
double long_to_double(long v) { return *(double *)&v; }
long   double_to_long(double d) { return *(long *)&d; }

long to_addr(long addr, long d_base, long poolsz) {
    if (addr >= 0) {
        if (addr < poolsz) return d_base + addr;
    }
    return addr;
}

long run(long *pc, long *bp, long *sp, char *d_base, long *t_base, long poolsz) {
  long a, cycle, i, *t, t_addr, ai, cv;
  double fa, fb, dv;
  char *fmt, *f2, *sp3, spec[32];
  long fargs[5];

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
        sp3 = spec; *sp3++ = *f2++;
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

long load_and_run(char *filename, long vm_argc, char **vm_argv) {
    long *text_base, *sp, *bp;
    char *data_base;
    long fd;
    
    char *file_buf;
    long file_size;
    long e_shoff, e_shnum;
    long sh_text, sh_data, sh_symtab, sh_strtab;
    long text_off, text_sz, data_off, data_sz;
    long symtab_off, symtab_sz, strtab_off, strtab_sz;
    long i, num_syms, sym_p, name_off;
    char *sym_name;
    long main_offset, text_count, exit_offset;

    fd = open(filename, 0, 0);
    if (fd < 0) { printf("Failed to open %s\n", filename); return -1; }

    file_buf = (char *)malloc(poolsz);
    file_size = read(fd, file_buf, poolsz);
    close(fd);

    if (file_buf[0] != 0x7f || file_buf[1] != 'E' || file_buf[2] != 'L' || file_buf[3] != 'F') {
        printf("Error: Not a valid ELF file.\n");
        return -1;
    }

    e_shoff = r64(file_buf, 40);
    e_shnum = r16(file_buf, 60);

    sh_text = e_shoff + 64 * 1;
    sh_data = e_shoff + 64 * 2;
    sh_symtab = e_shoff + 64 * 3;
    sh_strtab = e_shoff + 64 * 4;

    text_off = r64(file_buf, sh_text + 24);
    text_sz  = r64(file_buf, sh_text + 32);

    data_off = r64(file_buf, sh_data + 24);
    data_sz  = r64(file_buf, sh_data + 32);

    symtab_off = r64(file_buf, sh_symtab + 24);
    symtab_sz  = r64(file_buf, sh_symtab + 32);

    strtab_off = r64(file_buf, sh_strtab + 24);
    strtab_sz  = r64(file_buf, sh_strtab + 32);

    text_base = (long *)malloc(text_sz + 8);
    data_base = (char *)malloc(data_sz + 8);
    sp = (long *)malloc(poolsz);

    mem_cpy((char *)text_base, file_buf + text_off, text_sz);
    mem_cpy(data_base, file_buf + data_off, data_sz);

    main_offset = -1;
    num_syms = symtab_sz / 24;
    i = 0;
    while (i < num_syms) {
        sym_p = symtab_off + i * 24;
        name_off = r32(file_buf, sym_p + 0);
        sym_name = file_buf + strtab_off + name_off;

        if (sym_name[0] == 'm' && sym_name[1] == 'a' && sym_name[2] == 'i' && sym_name[3] == 'n' && sym_name[4] == 0) {
            main_offset = r64(file_buf, sym_p + 8);
            break;
        }
        i = i + 1;
    }

    if (main_offset == -1) {
        printf("Error: 'main' symbol not found in ELF!\n");
        return -1;
    }

    bp = sp = (long *)((long)sp + poolsz);

    text_count = text_sz / sizeof(long);
    exit_offset = text_count - 2;

    *--sp = vm_argc;
    *--sp = (long)vm_argv;
    *--sp = exit_offset;

    return run(text_base + main_offset, bp, sp, data_base, text_base, poolsz);
}

// -------------------------------------------------------
// Linker State & Functions
// -------------------------------------------------------
char  *elf_buf[32];
long   elf_text_off[32];
long   elf_text_sz[32];
long   elf_data_off[32];
long   elf_data_sz[32];
long   elf_sym_off[32];
long   elf_sym_sz[32];
long   elf_str_off[32];
long   elf_rela_off[32];
long   elf_rela_sz[32];
long   elf_text_base[32];
long   elf_data_base[32];

// -------------------------------------------------------
// Global symbol table (across all ELFs)
// -------------------------------------------------------
char  gsym_name_pool[65536];
long  gsym_name_ptr;

long  gsym_name[4096];
long  gsym_def[4096];
long  gsym_val[4096];
long  gsym_is_func[4096];
long  gsym_count;


long name_eq(char *a, char *b) {
  while (*a && *b) { if (*a != *b) return 0; a = a+1; b = b+1; }
  return (*a == 0 && *b == 0);
}

long name_len(char *s) { long n; n=0; while (s[n]) n=n+1; return n; }

long pool_add(char *name) {
  long off; long i; long len;
  off = gsym_name_ptr;
  len = name_len(name);
  i = 0;
  while (i <= len) {
    gsym_name_pool[gsym_name_ptr] = name[i];
    gsym_name_ptr = gsym_name_ptr + 1;
    i = i + 1;
  }
  return off;
}

// find or create global symbol; return index
long gsym_find(char *name) {
  long i;
  i = 0;
  while (i < gsym_count) {
    if (name_eq(gsym_name_pool + gsym_name[i], name)) return i;
    i = i + 1;
  }
  return -1;
}

long gsym_add(char *name) {
  long idx;
  idx = gsym_count;
  gsym_name[idx]   = pool_add(name);
  gsym_def[idx]    = 0;
  gsym_val[idx]    = 0;
  gsym_is_func[idx]= 0;
  gsym_count = gsym_count + 1;
  return idx;
}



// -------------------------------------------------------
// Read one ELF file into elf_buf[ei] and fill elf_* arrays
// -------------------------------------------------------

long read_elf(long ei, char *filename) {
  long fd; long sz;
  long e_shoff; long e_shnum; long e_shstrndx;
  long si; long base;
  long sh_name; long sh_type; long sh_off; long sh_sz; long sh_link;
  char *shstr;

  fd = open(filename, 0, 0);
  if (fd < 0) { printf("c5tool link: cannot open %s\n", filename); return -1; }

  elf_buf[ei] = (char *)malloc(poolsz);
  sz = read(fd, elf_buf[ei], poolsz);
  close(fd);

  if (sz < 16) { printf("c5tool link: file too small: %s\n", filename); return -1; }
  if (elf_buf[ei][0] != 0x7f || elf_buf[ei][1] != 'E' ||
      elf_buf[ei][2] != 'L'  || elf_buf[ei][3] != 'F') {
    printf("c5tool link: not ELF: %s\n", filename); return -1;
  }

  e_shoff    = r64(elf_buf[ei], 40);
  e_shnum    = r16(elf_buf[ei], 60);
  e_shstrndx = r16(elf_buf[ei], 62);

  // locate .shstrtab
  base = e_shoff + e_shstrndx * 64;
  shstr = elf_buf[ei] + r64(elf_buf[ei], base + 24);

  // init
  elf_text_off[ei] = 0; elf_text_sz[ei]  = 0;
  elf_data_off[ei] = 0; elf_data_sz[ei]  = 0;
  elf_sym_off[ei]  = 0; elf_sym_sz[ei]   = 0;
  elf_str_off[ei]  = 0;
  elf_rela_off[ei] = 0; elf_rela_sz[ei]  = 0;

  si = 0;
  while (si < e_shnum) {
    base    = e_shoff + si * 64;
    sh_name = r32(elf_buf[ei], base + 0);
    sh_type = r32(elf_buf[ei], base + 4);
    sh_off  = r64(elf_buf[ei], base + 24);
    sh_sz   = r64(elf_buf[ei], base + 32);
    sh_link = r32(elf_buf[ei], base + 40);

    // compare section name from shstrtab
    if (shstr[sh_name+0]=='.' && shstr[sh_name+1]=='t' && shstr[sh_name+2]=='e' &&
        shstr[sh_name+3]=='x' && shstr[sh_name+4]=='t' && shstr[sh_name+5]==0) {
      elf_text_off[ei] = sh_off; elf_text_sz[ei] = sh_sz;
    }
    else if (shstr[sh_name+0]=='.' && shstr[sh_name+1]=='d' && shstr[sh_name+2]=='a' &&
             shstr[sh_name+3]=='t' && shstr[sh_name+4]=='a' && shstr[sh_name+5]==0) {
      elf_data_off[ei] = sh_off; elf_data_sz[ei] = sh_sz;
    }
    else if (shstr[sh_name+0]=='.' && shstr[sh_name+1]=='s' && shstr[sh_name+2]=='y' &&
             shstr[sh_name+3]=='m' && shstr[sh_name+4]=='t' && shstr[sh_name+5]=='a' &&
             shstr[sh_name+6]=='b' && shstr[sh_name+7]==0) {
      elf_sym_off[ei] = sh_off; elf_sym_sz[ei] = sh_sz;
      elf_str_off[ei] = r64(elf_buf[ei], e_shoff + sh_link * 64 + 24);
    }
    else if (shstr[sh_name+0]=='.' && shstr[sh_name+1]=='r' && shstr[sh_name+2]=='e' &&
             shstr[sh_name+3]=='l' && shstr[sh_name+4]=='a' && shstr[sh_name+5]==0) {
      elf_rela_off[ei] = sh_off; elf_rela_sz[ei] = sh_sz;
    }
    si = si + 1;
  }
  return 0;
}

// -------------------------------------------------------
// Pass 1: collect all symbol definitions into global symtab
// -------------------------------------------------------
long pass1_collect(long n_elfs) {
  long ei; long i; long n; long base;
  long st_name; long st_info; long st_shndx; long st_value;
  char *symname; long gidx; long is_func;
  long text_long_base; long data_byte_base;

  ei = 0;
  while (ei < n_elfs) {
    text_long_base = elf_text_base[ei]; // already set in layout pass
    data_byte_base = elf_data_base[ei];
    n = elf_sym_sz[ei] / 24;
    i = 0;
    while (i < n) {
      base     = elf_sym_off[ei] + i * 24;
      st_name  = r32(elf_buf[ei], base + 0);
      st_info  = elf_buf[ei][base + 4] & 0xFF;
      st_shndx = r16(elf_buf[ei], base + 6);
      st_value = r64(elf_buf[ei], base + 8);

      symname = elf_buf[ei] + elf_str_off[ei] + st_name;
      is_func = (st_info == 0x12) ? 1 : 0;

      if (st_shndx != SHN_UNDEF) {
        // defined symbol
        gidx = gsym_find(symname);
        if (gidx < 0) gidx = gsym_add(symname);
        if (gsym_def[gidx]) {
          printf("c5tool link: duplicate definition of '%s'\n", symname);
          return -1;
        }
        gsym_def[gidx]     = 1;
        gsym_is_func[gidx] = is_func;
        if (is_func) {
          gsym_val[gidx] = text_long_base + st_value; // absolute text long-index
        } else {
          gsym_val[gidx] = data_byte_base + st_value; // absolute data byte-offset
        }
      } else {
        // undefined — just ensure it exists in table
        gidx = gsym_find(symname);
        if (gidx < 0) gsym_add(symname);
      }
      i = i + 1;
    }
    ei = ei + 1;
  }
  return 0;
}

// -------------------------------------------------------
// Pass 2: apply relocations to linked text
// -------------------------------------------------------
long pass2_reloc(long n_elfs, long *linked_text, char *linked_data) {
  long ei; long i; long n;
  long r_offset; long r_symidx; long r_type;
  long base; long gidx; long target;
  long nsym; long sym_base; long sym_shndx; long sym_val; long sym_name_off;
  char *symname;
  long text_long_base; long data_byte_base;

  ei = 0;
  while (ei < n_elfs) {
    text_long_base = elf_text_base[ei];
    data_byte_base = elf_data_base[ei];
    n = elf_rela_sz[ei] / 24;
    i = 0;
    while (i < n) {
      base     = elf_rela_off[ei] + i * 24;
      r_offset = r64(elf_buf[ei], base + 0);
      r_symidx = r64(elf_buf[ei], base + 8);
      r_type   = r64(elf_buf[ei], base + 16);

      // absolute slot in linked text
      // r_offset is already in long-units relative to this ELF's text
      target = text_long_base + r_offset;

      if (r_symidx == -1) {
        // internal reloc: add base offset to existing value
        if (r_type == RELA_FUNC || r_type == RELA_JMP) {
          linked_text[target] = linked_text[target] + text_long_base;
        } else {
          // RELA_DATA: existing value is data byte-offset within this ELF
          linked_text[target] = linked_text[target] + data_byte_base;
        }
      } else {
        // external reloc: look up in per-ELF symtab then global table
        sym_base     = elf_sym_off[ei] + r_symidx * 24;
        sym_name_off = r32(elf_buf[ei], sym_base + 0);
        symname      = elf_buf[ei] + elf_str_off[ei] + sym_name_off;

        gidx = gsym_find(symname);
        if (gidx < 0 || !gsym_def[gidx]) {
          printf("c5tool link: undefined symbol '%s'\n", symname);
          return -1;
        }
        linked_text[target] = gsym_val[gidx];
      }
      i = i + 1;
    }
    ei = ei + 1;
  }
  return 0;
}

// -------------------------------------------------------
// write_linked_elf: output the final linked ELF (no rela)
// Section layout: null + .text + .data + .symtab + .strtab + .shstrtab
// -------------------------------------------------------
void write_linked_elf(char *filename, long *linked_text, long text_longs,
                      char *linked_data, long data_sz,
                      long entry_offset) {
  long f;
  char *hdr;
  long text_sz; long symtab_sz; long strtab_sz; long shstrtab_sz;
  long off_shdr; long off_shstrtab; long off_text; long off_data;
  long off_symtab; long off_strtab;
  long i; long j;
  char *shstr;
  char *packed_sym; long sym_p;
  char *packed_str; long str_p;

  // build minimal symtab: just 'main' for VM to find entry
  // (linked ELF has no undefined symbols)
  // strtab: "\0main\0"
  shstr = "\0.text\0.data\0.symtab\0.strtab\0.shstrtab\0";
  shstrtab_sz = 40;

  text_sz    = text_longs * sizeof(long);
  // symtab: 1 entry for 'main'
  symtab_sz  = 1 * 24;
  // strtab: "\0main\0"  = 6 bytes
  strtab_sz  = 6;

  // 6 sections: null + text + data + symtab + strtab + shstrtab
  off_shdr       = 64;
  off_shstrtab   = off_shdr + 6 * 64;
  off_text       = off_shstrtab + shstrtab_sz;
  off_data       = off_text + text_sz;
  off_symtab     = off_data + data_sz;
  off_strtab     = off_symtab + symtab_sz;

  f = open(filename, 577, 420);
  if (f < 0) { printf("c5tool link: cannot open output %s\n", filename); return; }

  hdr = (char *)malloc(off_text);
  i = 0; while (i < off_text) { hdr[i] = 0; i = i + 1; }

  // ELF header
  hdr[0]=0x7f; hdr[1]='E'; hdr[2]='L'; hdr[3]='F';
  hdr[4]=2; hdr[5]=1; hdr[6]=1;
  w16(hdr, 16, 2);       // ET_EXEC
  w16(hdr, 18, 0xB7);    // EM_AARCH64 (VM marker)
  w32(hdr, 20, 1);
  w64(hdr, 24, entry_offset); // e_entry = main's text long-index
  w64(hdr, 32, 0);
  w64(hdr, 40, off_shdr);
  w32(hdr, 48, 0);
  w16(hdr, 52, 64);
  w16(hdr, 54, 0);
  w16(hdr, 56, 0);
  w16(hdr, 58, 64);
  w16(hdr, 60, 6);       // e_shnum
  w16(hdr, 62, 5);       // e_shstrndx

  // Shdr 0: null (already zero)

  // Shdr 1: .text
  i = off_shdr + 64;
  w32(hdr, i+0, 1); w32(hdr, i+4, 1);
  w64(hdr, i+8, 6); w64(hdr, i+16, 0);
  w64(hdr, i+24, off_text); w64(hdr, i+32, text_sz);
  w64(hdr, i+48, 8);

  // Shdr 2: .data
  i = i + 64;
  w32(hdr, i+0, 7); w32(hdr, i+4, 1);
  w64(hdr, i+8, 3); w64(hdr, i+16, 0);
  w64(hdr, i+24, off_data); w64(hdr, i+32, data_sz);
  w64(hdr, i+48, 8);

  // Shdr 3: .symtab
  i = i + 64;
  w32(hdr, i+0, 13); w32(hdr, i+4, 2);
  w64(hdr, i+8, 0); w64(hdr, i+16, 0);
  w64(hdr, i+24, off_symtab); w64(hdr, i+32, symtab_sz);
  w32(hdr, i+40, 4);   // sh_link -> .strtab
  w32(hdr, i+44, 0);
  w64(hdr, i+48, 8); w64(hdr, i+56, 24);

  // Shdr 4: .strtab
  i = i + 64;
  w32(hdr, i+0, 21); w32(hdr, i+4, 3);
  w64(hdr, i+8, 0); w64(hdr, i+16, 0);
  w64(hdr, i+24, off_strtab); w64(hdr, i+32, strtab_sz);
  w64(hdr, i+48, 1);

  // Shdr 5: .shstrtab
  i = i + 64;
  w32(hdr, i+0, 29); w32(hdr, i+4, 3);
  w64(hdr, i+8, 0); w64(hdr, i+16, 0);
  w64(hdr, i+24, off_shstrtab); w64(hdr, i+32, shstrtab_sz);
  w64(hdr, i+48, 1);

  // .shstrtab content
  j = 0; while (j < shstrtab_sz) { hdr[off_shstrtab+j] = shstr[j]; j=j+1; }

  // packed symtab: single entry for 'main'
  packed_sym = (char *)malloc(symtab_sz);
  i = 0; while (i < symtab_sz) { packed_sym[i]=0; i=i+1; }
  w32(packed_sym, 0,  1);            // st_name -> "main" at strtab[1]
  packed_sym[4] = 0x12;              // STB_GLOBAL | STT_FUNC
  packed_sym[5] = 0;
  w16(packed_sym, 6,  1);            // st_shndx = .text
  w64(packed_sym, 8,  entry_offset); // st_value = main's long-index
  w64(packed_sym, 16, 0);

  // strtab: "\0main\0"
  packed_str = (char *)malloc(strtab_sz);
  packed_str[0]=0; packed_str[1]='m'; packed_str[2]='a';
  packed_str[3]='i'; packed_str[4]='n'; packed_str[5]=0;

  write(f, hdr, off_text);
  write(f, (void *)linked_text, text_sz);
  write(f, linked_data, data_sz);
  write(f, packed_sym, symtab_sz);
  write(f, packed_str, strtab_sz);

  close(f);
  free(hdr); free(packed_sym); free(packed_str);
}


long link_elfs(char *outfile, long n_elfs) {
  long ei; long i;
  long total_text_longs; long total_data_bytes;
  long *linked_text; char *linked_data;
  long text_longs_i; long data_bytes_i;
  long main_gidx; long entry_offset;
  long exit_slot;

  total_text_longs = 1;
  total_data_bytes = 0;
  ei = 0;
  while (ei < n_elfs) {
    elf_text_base[ei] = total_text_longs;
    elf_data_base[ei] = total_data_bytes;
    total_text_longs = total_text_longs + elf_text_sz[ei] / sizeof(long);
    total_data_bytes = total_data_bytes + elf_data_sz[ei];
    ei = ei + 1;
  }

  total_text_longs = total_text_longs + 2;

  linked_text = (long *)malloc(total_text_longs * sizeof(long));
  linked_data = (char *)malloc(total_data_bytes + 8);
  i = 0; while (i < total_text_longs) { linked_text[i]=0; i=i+1; }
  i = 0; while (i < total_data_bytes + 8) { linked_data[i]=0; i=i+1; }

  ei = 0;
  while (ei < n_elfs) {
    text_longs_i = elf_text_sz[ei] / sizeof(long);
    data_bytes_i = elf_data_sz[ei];
    mem_cpy((char *)(linked_text + elf_text_base[ei]),
            elf_buf[ei] + elf_text_off[ei],
            elf_text_sz[ei]);
    if (data_bytes_i > 0) {
      mem_cpy(linked_data + elf_data_base[ei],
              elf_buf[ei] + elf_data_off[ei],
              data_bytes_i);
    }
    ei = ei + 1;
  }

  if (pass1_collect(n_elfs) < 0) return -1;
  if (pass2_reloc(n_elfs, linked_text, linked_data) < 0) return -1;

  main_gidx = gsym_find("main");
  if (main_gidx < 0 || !gsym_def[main_gidx]) {
    printf("c5tool: 'main' not defined\n"); return -1;
  }
  entry_offset = gsym_val[main_gidx];

  exit_slot = total_text_longs - 2;
  linked_text[exit_slot]     = PSH;
  linked_text[exit_slot + 1] = EXIT;

  i = 1;
  while (i < total_text_longs - 1) {
    if (linked_text[i] == JMP) {
      if (linked_text[i+1] == -1 || linked_text[i+1] == -2) {
        linked_text[i+1] = exit_slot;
      }
      i = i + 2;
    } else {
      i = i + 1;
    }
  }

  write_linked_elf(outfile, linked_text, total_text_longs,
                   linked_data, total_data_bytes, entry_offset);

  printf(">> c5tool: linked %ld ELF(s) -> %s (text=%ld longs, data=%ld bytes, entry=%ld) <<\n",
         n_elfs, outfile, total_text_longs, total_data_bytes, entry_offset);
  return 0;
}


// -------------------------------------------------------
// Unified Main
// -------------------------------------------------------
int main(int argc, char **argv) {
  long i, n_elfs, arg_idx;
  char *outfile;
  long is_link;

  poolsz = 1024 * 1024;
  debug = 0;
  outfile = 0;
  n_elfs  = 0;
  gsym_name_ptr = 0;
  gsym_count    = 0;
  gsym_name_pool[0] = 0;
  instr_name = "LLA ,IMM ,JMP ,JSR ,BZ  ,BNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PSH ,OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,LF  ,SF  ,IMMF,ITF ,ITFS,FTI ,FADD,FSUB,FMUL,FDIV,FEQ ,FNE ,FLT ,FGT ,FLE ,FGE ,PRTF,OPEN,READ,WRIT,CLOS,PRTF,MALC,FREE,MSET,MCMP,EXIT,";

  if (argc < 2) {
    printf("Usage: c5tool run [-d] <program.elf> [args...]\n       c5tool link -o out.elf a.elf b.elf ...\n");
    return -1;
  }

  is_link = 0;
  if (argv[1][0] == 'l' && argv[1][1] == 'i' && argv[1][2] == 'n' && argv[1][3] == 'k' && argv[1][4] == 0) {
    is_link = 1;
  } else if (argv[1][0] == 'r' && argv[1][1] == 'u' && argv[1][2] == 'n' && argv[1][3] == 0) {
    is_link = 0;
  } else {
    printf("Unknown subcommand: %s\n", argv[1]);
    return -1;
  }

  if (is_link) {
    i = 2;
    while (i < argc) {
      if (argv[i][0] == '-' && argv[i][1] == 'o') {
        i = i + 1;
        if (i >= argc) { printf("c5tool: -o requires filename\n"); return -1; }
        outfile = argv[i];
      } else {
        if (n_elfs >= 32) { printf("c5tool: too many input files\n"); return -1; }
        if (read_elf(n_elfs, argv[i]) < 0) return -1;
        n_elfs = n_elfs + 1;
      }
      i = i + 1;
    }
    if (!outfile) { printf("Usage: c5tool link -o out.elf a.elf b.elf ...\n"); return -1; }
    if (n_elfs == 0) { printf("c5tool: no input files\n"); return -1; }
    return link_elfs(outfile, n_elfs);
  } else {
    arg_idx = 2;
    if (argc > 2 && argv[2][0] == '-' && argv[2][1] == 'd') {
      debug = 1;
      arg_idx = 3;
    }
    if (arg_idx >= argc) {
      printf("Usage: c5tool run [-d] <program.elf> [args...]\n");
      return -1;
    }
    return load_and_run(argv[arg_idx], argc - arg_idx, &argv[arg_idx]);
  }
}

