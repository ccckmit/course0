/*
 * lz0.c - LZ77 Compression / Decompression
 *
 * Usage:
 *   ./lz0 file.txt       -> 壓縮，輸出 file.txt.lz
 *   ./lz0 file.txt.lz   -> 解壓縮，輸出 file.txt
 *
 * Format per token (1 flag byte per 8 tokens):
 *   Literal  : 1 byte  (flag bit = 0)
 *   Back-ref : 3 bytes (flag bit = 1)
 *              [offset_hi | len-3] [offset_lo]
 *              offset : 12 bits  (1..WINDOW_SIZE)
 *              length :  4 bits  (3..18)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>

#define WINDOW_SIZE   4096   /* sliding window  (12-bit offset) */
#define MAX_MATCH     18     /* max match length (4-bit: 0..15 + 3) */
#define MIN_MATCH     3      /* minimum useful match */

/* ------------------------------------------------------------------ */
/*  Compression                                                         */
/* ------------------------------------------------------------------ */

/*
 * Naive O(n * WINDOW) search — simple and correct.
 * Returns match length (>= MIN_MATCH) or 0.
 */
static int find_match(const unsigned char *buf, int buf_len,
                      int pos, int *match_offset)
{
    int best_len = 0;
    int best_off = 0;
    int start = pos - WINDOW_SIZE;
    if (start < 0) start = 0;

    for (int i = start; i < pos; i++) {
        int len = 0;
        while (len < MAX_MATCH &&
               pos + len < buf_len &&
               buf[i + len] == buf[pos + len])
            len++;

        if (len > best_len) {
            best_len = len;
            best_off = pos - i;          /* 1-based distance back */
            if (best_len == MAX_MATCH) break;
        }
    }

    if (best_len >= MIN_MATCH) {
        *match_offset = best_off;
        return best_len;
    }
    return 0;
}

static int compress(const char *in_path, const char *out_path)
{
    /* --- read entire input --- */
    FILE *fin = fopen(in_path, "rb");
    if (!fin) { perror(in_path); return 1; }

    fseek(fin, 0, SEEK_END);
    long file_len = ftell(fin);
    rewind(fin);

    unsigned char *buf = malloc(file_len);
    if (!buf) { fclose(fin); fputs("Out of memory\n", stderr); return 1; }
    if (fread(buf, 1, file_len, fin) != (size_t)file_len) {
        perror("fread"); fclose(fin); free(buf); return 1;
    }
    fclose(fin);

    FILE *fout = fopen(out_path, "wb");
    if (!fout) { perror(out_path); free(buf); return 1; }

    /* Write 8-byte magic + original size */
    const char magic[8] = "LZ0\x1a\x00\x77\x00\x01";
    fwrite(magic, 1, 8, fout);
    uint32_t orig = (uint32_t)file_len;
    fwrite(&orig, 4, 1, fout);         /* little-endian on most hosts */

    /* --- encode --- */
    /*
     * We buffer up to 8 tokens, then flush:
     *   1 flag byte  (bit i set => token i is a back-ref)
     *   followed by the raw token bytes
     */
    unsigned char flag_byte = 0;
    unsigned char token_buf[8 * 3];   /* worst case 3 bytes each */
    int  token_count = 0;
    int  token_bytes = 0;

    int pos = 0;
    while (pos < file_len) {
        int off = 0;
        int len = find_match(buf, (int)file_len, pos, &off);

        if (len >= MIN_MATCH) {
            /* back-reference: encode offset (12 bit) and len-3 (4 bit)
             * byte0: [off_hi 4 bits | (len-3) 4 bits]
             * byte1: [off_lo 8 bits]                                  */
            int enc_len = len - MIN_MATCH;          /* 0..15 */
            int enc_off = off - 1;                  /* 0..4095 */
            unsigned char b0 = (unsigned char)(((enc_off >> 4) & 0xF0) | (enc_len & 0x0F));
            unsigned char b1 = (unsigned char)(enc_off & 0xFF);
            token_buf[token_bytes++] = b0;
            token_buf[token_bytes++] = b1;
            flag_byte |= (unsigned char)(1 << token_count);
            pos += len;
        } else {
            /* literal */
            token_buf[token_bytes++] = buf[pos++];
            /* flag bit stays 0 */
        }

        token_count++;

        if (token_count == 8) {
            fputc(flag_byte, fout);
            fwrite(token_buf, 1, token_bytes, fout);
            flag_byte   = 0;
            token_count = 0;
            token_bytes = 0;
        }
    }

    /* flush remaining tokens */
    if (token_count > 0) {
        fputc(flag_byte, fout);
        fwrite(token_buf, 1, token_bytes, fout);
    }

    fclose(fout);
    free(buf);

    printf("壓縮完成: %s  (%ld -> ", in_path, file_len);
    FILE *tmp = fopen(out_path, "rb");
    if (tmp) { fseek(tmp, 0, SEEK_END); printf("%ld bytes)\n", ftell(tmp)); fclose(tmp); }
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Decompression                                                       */
/* ------------------------------------------------------------------ */

static int decompress(const char *in_path, const char *out_path)
{
    FILE *fin = fopen(in_path, "rb");
    if (!fin) { perror(in_path); return 1; }

    /* verify magic */
    char magic[8];
    if (fread(magic, 1, 8, fin) != 8 ||
        memcmp(magic, "LZ0\x1a\x00\x77\x00\x01", 8) != 0) {
        fputs("錯誤: 不是有效的 .lz 檔案\n", stderr);
        fclose(fin); return 1;
    }

    uint32_t orig_size;
    if (fread(&orig_size, 4, 1, fin) != 1) {
        fputs("錯誤: 無法讀取原始大小\n", stderr);
        fclose(fin); return 1;
    }

    unsigned char *out = malloc(orig_size);
    if (!out) { fclose(fin); fputs("Out of memory\n", stderr); return 1; }

    int opos = 0;

    while (opos < (int)orig_size) {
        int flag = fgetc(fin);
        if (flag == EOF) break;

        for (int i = 0; i < 8 && opos < (int)orig_size; i++) {
            if (flag & (1 << i)) {
                /* back-reference */
                int b0 = fgetc(fin);
                int b1 = fgetc(fin);
                if (b0 == EOF || b1 == EOF) goto done;

                int enc_off = ((b0 & 0xF0) << 4) | b1;   /* 0..4095 */
                int enc_len = b0 & 0x0F;                   /* 0..15  */
                int off = enc_off + 1;
                int len = enc_len + MIN_MATCH;

                int src = opos - off;
                for (int k = 0; k < len && opos < (int)orig_size; k++)
                    out[opos++] = out[src + k];
            } else {
                /* literal */
                int c = fgetc(fin);
                if (c == EOF) goto done;
                out[opos++] = (unsigned char)c;
            }
        }
    }
done:
    fclose(fin);

    FILE *fout = fopen(out_path, "wb");
    if (!fout) { perror(out_path); free(out); return 1; }
    fwrite(out, 1, opos, fout);
    fclose(fout);
    free(out);

    printf("解壓縮完成: %s  (%u bytes)\n", out_path, orig_size);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  main                                                                */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "用法: %s <檔案>\n", argv[0]);
        fprintf(stderr, "  壓縮:   %s file.txt     -> file.txt.lz\n", argv[0]);
        fprintf(stderr, "  解壓縮: %s file.txt.lz  -> file.txt\n",    argv[0]);
        return 1;
    }

    const char *input = argv[1];
    size_t len = strlen(input);

    /* detect .lz extension */
    const char *ext = ".lz";
    size_t ext_len = strlen(ext);

    if (len > ext_len &&
        strcmp(input + len - ext_len, ext) == 0) {
        /* --- decompress --- */
        char *out_path = malloc(len + 1);
        memcpy(out_path, input, len - ext_len);
        out_path[len - ext_len] = '\0';
        int ret = decompress(input, out_path);
        free(out_path);
        return ret;
    } else {
        /* --- compress --- */
        char *out_path = malloc(len + ext_len + 1);
        sprintf(out_path, "%s%s", input, ext);
        int ret = compress(input, out_path);
        free(out_path);
        return ret;
    }
}