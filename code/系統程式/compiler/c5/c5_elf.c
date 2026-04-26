// c5_elf.c - Extracted ELF generation logic
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include <fcntl.h>

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

extern char *data_base;
extern long *text_base;
extern struct Elf64_Sym *symtab_base;
extern char *strtab_base;
extern long symtab_count;
extern long strtab_ptr;
extern char *data;
extern long *e;

extern struct Rela *rela_base;
extern long rela_count;

void w16(char *buf, long p, long v) { buf[p] = v; buf[p+1] = v>>8; }
void w32(char *buf, long p, long v) { buf[p] = v; buf[p+1] = v>>8; buf[p+2] = v>>16; buf[p+3] = v>>24; }
void w64(char *buf, long p, long v) { w32(buf, p, v); w32(buf, p+4, v>>32); }

void write_elf(char *filename) {
  long f;
  char *hdr;
  long text_sz, data_sz, symtab_sz, strtab_sz, shstrtab_sz, rela_sz;
  long off_hdr, off_shdr, off_shstrtab, off_text, off_data, off_symtab, off_strtab, off_rela;
  long i, j, k, sym_p;
  char *shstr;
  char *packed_symtab;
  char *packed_rela;
  long rela_p;

  // .shstrtab: "\0.text\0.data\0.symtab\0.strtab\0.shstrtab\0.rela\0"
  // offsets:    0  1      7      13       21       29         39   44
  shstr = "\0.text\0.data\0.symtab\0.strtab\0.shstrtab\0.rela\0";
  shstrtab_sz = 45;

  text_sz    = (long)(e + 1 - text_base) * sizeof(long);
  data_sz    = (long)(data - data_base);
  symtab_sz  = symtab_count * 24;
  strtab_sz  = strtab_ptr;
  rela_sz    = rela_count * 24;  // each Rela entry: 3 * 8 bytes

  // 7 section headers: null + text + data + symtab + strtab + shstrtab + rela
  off_hdr       = 0;
  off_shdr      = 64;
  off_shstrtab  = off_shdr + 7 * 64;
  off_text      = off_shstrtab + shstrtab_sz;
  off_data      = off_text + text_sz;
  off_symtab    = off_data + data_sz;
  off_strtab    = off_symtab + symtab_sz;
  off_rela      = off_strtab + strtab_sz;

  f = open(filename, 577, 420);
  if (f < 0) { printf("Cannot open output file %s\n", filename); return; }

  hdr = (char *)malloc(off_text);
  memset(hdr, 0, off_text);

  // ELF Header (64 bytes)
  hdr[0] = 0x7f; hdr[1] = 'E'; hdr[2] = 'L'; hdr[3] = 'F';
  hdr[4] = 2; hdr[5] = 1; hdr[6] = 1;
  w16(hdr, 16, 1);      // ET_REL (relocatable)
  w16(hdr, 18, 0xB7);   // EM_AARCH64 (VM uses this as marker)
  w32(hdr, 20, 1);
  w64(hdr, 24, 0);
  w64(hdr, 32, 0);
  w64(hdr, 40, off_shdr);
  w32(hdr, 48, 0);
  w16(hdr, 52, 64);
  w16(hdr, 54, 0);
  w16(hdr, 56, 0);
  w16(hdr, 58, 64);
  w16(hdr, 60, 7);      // e_shnum = 7
  w16(hdr, 62, 5);      // e_shstrndx = 5 (.shstrtab)

  // Shdr 0: null
  // (already zeroed)

  // Shdr 1: .text  (sh_name=1, SHT_PROGBITS=1, SHF_ALLOC|EXEC=6)
  i = off_shdr + 64;
  w32(hdr, i+0, 1);  w32(hdr, i+4, 1);
  w64(hdr, i+8, 6);
  w64(hdr, i+16, 0);
  w64(hdr, i+24, off_text);
  w64(hdr, i+32, text_sz);
  w64(hdr, i+48, 8);

  // Shdr 2: .data  (sh_name=7, SHT_PROGBITS=1, SHF_ALLOC|WRITE=3)
  i = i + 64;
  w32(hdr, i+0, 7);  w32(hdr, i+4, 1);
  w64(hdr, i+8, 3);
  w64(hdr, i+16, 0);
  w64(hdr, i+24, off_data);
  w64(hdr, i+32, data_sz);
  w64(hdr, i+48, 8);

  // Shdr 3: .symtab  (sh_name=13, SHT_SYMTAB=2)
  i = i + 64;
  w32(hdr, i+0, 13);  w32(hdr, i+4, 2);
  w64(hdr, i+8, 0);
  w64(hdr, i+16, 0);
  w64(hdr, i+24, off_symtab);
  w64(hdr, i+32, symtab_sz);
  w32(hdr, i+40, 4);   // sh_link -> .strtab (section 4)
  w32(hdr, i+44, 0);
  w64(hdr, i+48, 8);
  w64(hdr, i+56, 24);  // sh_entsize

  // Shdr 4: .strtab  (sh_name=21, SHT_STRTAB=3)
  i = i + 64;
  w32(hdr, i+0, 21);  w32(hdr, i+4, 3);
  w64(hdr, i+8, 0);
  w64(hdr, i+16, 0);
  w64(hdr, i+24, off_strtab);
  w64(hdr, i+32, strtab_sz);
  w64(hdr, i+48, 1);

  // Shdr 5: .shstrtab  (sh_name=29, SHT_STRTAB=3)
  i = i + 64;
  w32(hdr, i+0, 29);  w32(hdr, i+4, 3);
  w64(hdr, i+8, 0);
  w64(hdr, i+16, 0);
  w64(hdr, i+24, off_shstrtab);
  w64(hdr, i+32, shstrtab_sz);
  w64(hdr, i+48, 1);

  // Shdr 6: .rela  (sh_name=40, SHT_RELA=4, sh_link=3 symtab, sh_info=1 .text)
  i = i + 64;
  w32(hdr, i+0, 39);  w32(hdr, i+4, 4);   // SHT_RELA=4
  w64(hdr, i+8, 0);
  w64(hdr, i+16, 0);
  w64(hdr, i+24, off_rela);
  w64(hdr, i+32, rela_sz);
  w32(hdr, i+40, 3);  // sh_link -> .symtab (section 3)
  w32(hdr, i+44, 1);  // sh_info -> .text   (section 1)
  w64(hdr, i+48, 8);
  w64(hdr, i+56, 24); // sh_entsize

  // .shstrtab content
  k = 0;
  while (k < shstrtab_sz) { hdr[off_shstrtab + k] = shstr[k]; k = k + 1; }

  // pack symtab
  packed_symtab = (char *)malloc(symtab_sz + 8);
  memset(packed_symtab, 0, symtab_sz + 8);
  j = 0;
  while (j < symtab_count) {
    sym_p = j * 24;
    w32(packed_symtab, sym_p + 0,  symtab_base[j].st_name);
    packed_symtab[sym_p + 4] = symtab_base[j].st_info;
    packed_symtab[sym_p + 5] = symtab_base[j].st_other;
    w16(packed_symtab, sym_p + 6,  symtab_base[j].st_shndx);
    w64(packed_symtab, sym_p + 8,  symtab_base[j].st_value);
    w64(packed_symtab, sym_p + 16, symtab_base[j].st_size);
    j = j + 1;
  }

  // pack rela table: { r_offset(8), r_symidx(8), r_type(8) }
  packed_rela = (char *)malloc(rela_sz + 8);
  memset(packed_rela, 0, rela_sz + 8);
  j = 0;
  while (j < rela_count) {
    rela_p = j * 24;
    w64(packed_rela, rela_p + 0,  rela_base[j].r_offset);
    w64(packed_rela, rela_p + 8,  rela_base[j].r_symidx);
    w64(packed_rela, rela_p + 16, rela_base[j].r_type);
    j = j + 1;
  }

  write(f, hdr, off_text);
  write(f, (void *)text_base, text_sz);
  write(f, (void *)data_base, data_sz);
  write(f, packed_symtab, symtab_sz);
  write(f, strtab_base, strtab_sz);
  write(f, packed_rela, rela_sz);

  close(f);
  free(hdr);
  free(packed_symtab);
  free(packed_rela);
  printf(">> ELF written to %s (syms=%ld, relas=%ld) <<\n", filename, symtab_count, rela_count);
}

long add_symbol(struct symbol *s, long type, long value, long shndx) {
  long len;
  long name_offset;
  long i;
  
  len = s->v_hash & 0x3F;
  name_offset = strtab_ptr;
  
  i = 0;
  while (i < len) {
    strtab_base[strtab_ptr] = s->v_name[i];
    strtab_ptr = strtab_ptr + 1;
    i = i + 1;
  }
  strtab_base[strtab_ptr] = 0;
  strtab_ptr = strtab_ptr + 1;

  symtab_base[symtab_count].st_name = name_offset;
  symtab_base[symtab_count].st_info = type;
  symtab_base[symtab_count].st_other = 0;
  symtab_base[symtab_count].st_shndx = shndx;
  symtab_base[symtab_count].st_value = value;
  symtab_base[symtab_count].st_size = 0;
  
  symtab_count = symtab_count + 1;
  return symtab_count - 1;
}

void add_rela(long text_slot, long sym_idx, long type) {
  rela_base[rela_count].r_offset = text_slot;
  rela_base[rela_count].r_symidx = sym_idx;
  rela_base[rela_count].r_type   = type;
  rela_count = rela_count + 1;
}

