/*
 * jpg0.c  —  PPM <-> JPEG converter & info tool, pure C, zero external dependencies
 *
 * Compile:  gcc -O2 -o jpg0 jpg0.c -lm
 *
 * Usage:
 *   1. Info PPM:   ./jpg0 test.ppm
 *   2. Info JPG:   ./jpg0 test.jpg
 *   3. PPM->JPG:   ./jpg0 test.ppm -o test.jpg [quality 1-100]
 *   4. JPG->PPM:   ./jpg0 test.jpg -o test.ppm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

/* ── Utilities ── */
static int endswith(const char *str, const char *suffix) {
    if (!str || !suffix) return 0;
    size_t lenstr = strlen(str), lensuffix = strlen(suffix);
    if (lensuffix > lenstr) return 0;
    const char *a = str + (lenstr - lensuffix);
    while (*a && *suffix) {
        int ca = *a++, cs = *suffix++;
        if (ca >= 'A' && ca <= 'Z') ca += 32;
        if (cs >= 'A' && cs <= 'Z') cs += 32;
        if (ca != cs) return 0;
    }
    return 1;
}

typedef struct { int w, h; uint8_t *rgb; } Image;

static void show_ppm_info(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) { perror(path); return; }
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1 || (strcmp(magic, "P3") != 0 && strcmp(magic, "P6") != 0)) {
        printf("Not a valid PPM file.\n");
        fclose(fp);
        return;
    }
    int w, h, maxval;
    fscanf(fp, "%d %d %d", &w, &h, &maxval);
    printf("File      : %s\n", path);
    printf("Format    : PPM (%s)\n", magic[1] == '3' ? "ASCII" : "Binary");
    printf("Size      : %d x %d\n", w, h);
    printf("Max value : %d\n", maxval);
    fclose(fp);
}

static Image *load_ppm(const char *path) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    char magic[3];
    if (fscanf(fp, "%2s", magic) != 1) { fclose(fp); return NULL; }
    int w, h, maxval;
    if (fscanf(fp, "%d %d %d", &w, &h, &maxval) != 3) { fclose(fp); return NULL; }
    
    Image *img = (Image*)malloc(sizeof *img);
    img->w = w; img->h = h;
    img->rgb = (uint8_t*)malloc((size_t)w * h * 3);
    
    if (strcmp(magic, "P6") == 0) {
        fgetc(fp);
        fread(img->rgb, 1, (size_t)w * h * 3, fp);
    } else {
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int r, g, b;
                fscanf(fp, "%d %d %d", &r, &g, &b);
                img->rgb[(y * w + x) * 3 + 0] = (uint8_t)r;
                img->rgb[(y * w + x) * 3 + 1] = (uint8_t)g;
                img->rgb[(y * w + x) * 3 + 2] = (uint8_t)b;
            }
        }
    }
    fclose(fp);
    return img;
}

static int save_ppm(const Image *img, const char *path) {
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    fprintf(fp, "P6\n%d %d\n255\n", img->w, img->h);
    fwrite(img->rgb, 1, (size_t)img->w * img->h * 3, fp);
    fclose(fp);
    return 0;
}

static const uint8_t DC_LUM_BITS[17] = {0,0,1,5,1,1,1,1,1,1,0,0,0,0,0,0,0};
static const uint8_t DC_LUM_VALS[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
static const uint8_t DC_CHR_BITS[17] = {0,0,3,1,1,1,1,1,1,1,1,1,0,0,0,0,0};
static const uint8_t DC_CHR_VALS[12] = {0,1,2,3,4,5,6,7,8,9,10,11};
static const uint8_t AC_LUM_BITS[17] = {0,0,2,1,3,3,2,4,3,5,5,4,4,0,0,1,125};
static const uint8_t AC_LUM_VALS[162]= {
    0x01,0x02,0x03,0x00,0x04,0x11,0x05,0x12,0x21,0x31,0x41,0x06,0x13,0x51,0x61,0x07,
    0x22,0x71,0x14,0x32,0x81,0x91,0xA1,0x08,0x23,0x42,0xB1,0xC1,0x15,0x52,0xD1,0xF0,
    0x24,0x33,0x62,0x72,0x82,0x09,0x0A,0x16,0x17,0x18,0x19,0x1A,0x25,0x26,0x27,0x28,
    0x29,0x2A,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
    0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,0x69,
    0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x83,0x84,0x85,0x86,0x87,0x88,0x89,
    0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,
    0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,0xC4,0xC5,
    0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xE1,0xE2,
    0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,
    0xF9,0xFA
};
static const uint8_t AC_CHR_BITS[17] = {0,0,2,1,2,4,4,3,4,7,5,4,4,0,1,2,119};
static const uint8_t AC_CHR_VALS[162]= {
    0x00,0x01,0x02,0x03,0x11,0x04,0x05,0x21,0x31,0x06,0x12,0x41,0x51,0x07,0x61,0x71,
    0x13,0x22,0x32,0x81,0x08,0x14,0x42,0x91,0xA1,0xB1,0xC1,0x09,0x23,0x33,0x52,0xF0,
    0x15,0x62,0x72,0xD1,0x0A,0x16,0x24,0x34,0xE1,0x25,0xF1,0x17,0x18,0x19,0x1A,0x26,
    0x27,0x28,0x29,0x2A,0x35,0x36,0x37,0x38,0x39,0x3A,0x43,0x44,0x45,0x46,0x47,0x48,
    0x49,0x4A,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5A,0x63,0x64,0x65,0x66,0x67,0x68,
    0x69,0x6A,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7A,0x82,0x83,0x84,0x85,0x86,0x87,
    0x88,0x89,0x8A,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA2,0xA3,0xA4,0xA5,
    0xA6,0xA7,0xA8,0xA9,0xAA,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xC2,0xC3,
    0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,
    0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,
    0xF9,0xFA
};

static const uint8_t BASE_LUM_QT[64] = {
    16,11,10,16,24,40,51,61, 12,12,14,19,26,58,60,55, 14,13,16,24,40,57,69,56,
    14,17,22,29,51,87,80,62, 18,22,37,56,68,109,103,77, 24,35,55,64,81,104,113,92,
    49,64,78,87,103,121,120,101, 72,92,95,98,112,100,103,99
};
static const uint8_t BASE_CHR_QT[64] = {
    17,18,24,47,99,99,99,99, 18,21,26,66,99,99,99,99, 24,26,56,99,99,99,99,99,
    47,66,99,99,99,99,99,99, 99,99,99,99,99,99,99,99, 99,99,99,99,99,99,99,99,
    99,99,99,99,99,99,99,99, 99,99,99,99,99,99,99,99
};

static const int ZIGZAG[64] = {
     0, 1, 8,16, 9, 2, 3,10, 17,24,32,25,18,11, 4, 5,
    12,19,26,33,40,48,41,34, 27,20,13, 6, 7,14,21,28,
    35,42,49,56,57,50,43,36, 29,22,15,23,30,37,44,51,
    58,59,52,45,38,31,39,46, 53,60,61,54,47,55,62,63
};

/* ── Huffman framework (Encoder/Decoder Dual) ── */
typedef struct {
    uint32_t code[256]; int size[256];
    int mincode[17], maxcode[17], valptr[17];
    uint8_t huffval[256];
} HuffTab;

static void build_huff(HuffTab *ht, const uint8_t *bits, const uint8_t *vals, int nvals) {
    memset(ht, 0, sizeof *ht);
    uint32_t code = 0; int idx = 0;
    for (int len = 1; len <= 16; len++) {
        ht->mincode[len] = code;
        ht->valptr[len] = idx;
        for (int k = 0; k < (int)bits[len] && idx < nvals; k++, idx++) {
            ht->code[vals[idx]] = code;
            ht->size[vals[idx]] = len;
            ht->huffval[idx] = vals[idx];
            code++;
        }
        ht->maxcode[len] = code - 1;
        code <<= 1;
    }
}
static int count_vals(const uint8_t *bits) { int n=0; for(int i=1;i<=16;i++) n+=bits[i]; return n; }

/* ════════════════════════════════════════════════════════
 *  JPEG Decoder & Info
 * ════════════════════════════════════════════════════════ */

typedef struct {
    uint8_t qt[4][64];
    HuffTab ht_dc[4], ht_ac[4];
    int w, h, comps, max_h, max_v;
    int comp_id[3], comp_h[3], comp_v[3], comp_qt[3];
    int sos_dc[3], sos_ac[3];
    int rst_interval, is_progressive;
} JpegContext;

static uint16_t get16(FILE *fp) { int h=fgetc(fp); int l=fgetc(fp); return (h<<8)|l; }

static int load_jpeg_header(FILE *fp, JpegContext *ctx) {
    while(1) {
        int c = fgetc(fp);
        if(c == EOF) return -1;
        if(c != 0xFF) continue;
        int m = fgetc(fp);
        if(m == 0xFF || m == 0xD8) continue;
        if(m == 0xD9) break;

        if(m == 0xDA) { // SOS
            get16(fp);
            int n = fgetc(fp);
            for(int i=0; i<n; i++) {
                int cid = fgetc(fp), table = fgetc(fp);
                for(int j=0; j<ctx->comps; j++) {
                    if(ctx->comp_id[j] == cid) { ctx->sos_dc[j]=table>>4; ctx->sos_ac[j]=table&15; }
                }
            }
            fgetc(fp); fgetc(fp); fgetc(fp); // skip spectral
            return 0; // ready for bitstream
        } else if(m == 0xDD) { // DRI
            get16(fp); ctx->rst_interval = get16(fp);
        } else {
            int len = get16(fp);
            if(m == 0xDB) { // DQT
                int left = len - 2;
                while(left > 0) {
                    int id = fgetc(fp) & 15; left--;
                    for(int i=0; i<64; i++) { ctx->qt[id][i] = fgetc(fp); left--; }
                }
            } else if(m == 0xC4) { // DHT
                int left = len - 2;
                while(left > 0) {
                    int info = fgetc(fp); left--;
                    int tc = info >> 4, id = info & 15, nvals = 0;
                    uint8_t bits[17] = {0};
                    for(int i=1; i<=16; i++) { bits[i] = fgetc(fp); left--; nvals += bits[i]; }
                    uint8_t *vals = malloc(nvals);
                    for(int i=0; i<nvals; i++) { vals[i] = fgetc(fp); left--; }
                    if(tc == 0) build_huff(&ctx->ht_dc[id], bits, vals, nvals);
                    else        build_huff(&ctx->ht_ac[id], bits, vals, nvals);
                    free(vals);
                }
            } else if(m == 0xC0 || m == 0xC1 || m == 0xC2) { // SOF
                if(m == 0xC2) ctx->is_progressive = 1;
                fgetc(fp); // precision
                ctx->h = get16(fp); ctx->w = get16(fp); ctx->comps = fgetc(fp);
                for(int i=0; i<ctx->comps; i++) {
                    ctx->comp_id[i] = fgetc(fp);
                    int samp = fgetc(fp);
                    ctx->comp_h[i] = samp >> 4; ctx->comp_v[i] = samp & 15;
                    ctx->comp_qt[i] = fgetc(fp);
                    if(ctx->comp_h[i] > ctx->max_h) ctx->max_h = ctx->comp_h[i];
                    if(ctx->comp_v[i] > ctx->max_v) ctx->max_v = ctx->comp_v[i];
                }
            } else {
                fseek(fp, len - 2, SEEK_CUR); // skip
            }
        }
    }
    return -1;
}

static void show_jpeg_info(const char *path) {
    FILE *fp = fopen(path, "rb");
    if(!fp) { perror(path); return; }
    JpegContext ctx = {0};
    if(load_jpeg_header(fp, &ctx) == 0) {
        printf("File      : %s\n", path);
        printf("Format    : %s\n", ctx.is_progressive ? "JPEG (Progressive)" : "JPEG (Baseline)");
        printf("Size      : %d x %d\n", ctx.w, ctx.h);
        printf("Channels  : %d\n", ctx.comps);
        for(int i=0; i<ctx.comps; i++)
            printf(" Comp %d   : Subsampling %dx%d\n", i, ctx.comp_h[i], ctx.comp_v[i]);
    } else {
        printf("Not a valid JPEG file.\n");
    }
    fclose(fp);
}

typedef struct { FILE *fp; int buf, bits; } JpegBitBuf;

static int j_get_bit(JpegBitBuf *b) {
    if(b->bits == 0) {
        int c = fgetc(b->fp);
        if(c == 0xFF) {
            int m = fgetc(b->fp);
            if(m != 0x00) { ungetc(m, b->fp); ungetc(0xFF, b->fp); c = 0; }
        }
        b->buf = c; b->bits = 8;
    }
    b->bits--;
    return (b->buf >> b->bits) & 1;
}

static int decode_huff(JpegBitBuf *bb, const HuffTab *ht) {
    int code = 0;
    for(int len=1; len<=16; len++) {
        code = (code << 1) | j_get_bit(bb);
        if(code <= ht->maxcode[len]) return ht->huffval[ht->valptr[len] + (code - ht->mincode[len])];
    }
    return 0;
}

static void idct_block(const float in[8][8], float out[8][8]) {
    static const double PI = 3.14159265358979323846;
    static const double INV_SQRT2 = 0.70710678118654752440;
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            double s = 0.0;
            for (int u = 0; u < 8; u++) {
                double cu = (u == 0) ? INV_SQRT2 : 1.0;
                for (int v = 0; v < 8; v++) {
                    double cv = (v == 0) ? INV_SQRT2 : 1.0;
                    s += cu * cv * in[u][v] * cos((2*x+1)*u*PI/16.0) * cos((2*y+1)*v*PI/16.0);
                }
            }
            out[x][y] = (float)(0.25 * s);
        }
    }
}

static void dec_block(JpegBitBuf *bb, JpegContext *ctx, int comp, float out[8][8], int *prev_dc) {
    HuffTab *ht_dc = &ctx->ht_dc[ctx->sos_dc[comp]];
    HuffTab *ht_ac = &ctx->ht_ac[ctx->sos_ac[comp]];
    uint8_t *qt = ctx->qt[ctx->comp_qt[comp]];
    float F[8][8] = {0};

    int cat = decode_huff(bb, ht_dc), diff = 0;
    if(cat > 0) {
        int bits = 0; for(int i=0; i<cat; i++) bits = (bits << 1) | j_get_bit(bb);
        diff = bits; if(!(bits & (1 << (cat-1)))) diff -= (1 << cat) - 1;
    }
    *prev_dc += diff;
    F[ZIGZAG[0]/8][ZIGZAG[0]%8] = *prev_dc * qt[0];

    int k = 1;
    while(k < 64) {
        int sym = decode_huff(bb, ht_ac);
        if(sym == 0x00) break; // EOB
        if(sym == 0xF0) { k += 16; continue; } // ZRL
        int run = sym >> 4, size = sym & 15;
        k += run;
        if(k >= 64) break;
        int bits = 0; for(int i=0; i<size; i++) bits = (bits << 1) | j_get_bit(bb);
        int val = bits; if(!(bits & (1 << (size-1)))) val -= (1 << size) - 1;
        F[ZIGZAG[k]/8][ZIGZAG[k]%8] = val * qt[k];
        k++;
    }
    idct_block(F, out);
}

static Image *load_jpeg(const char *path) {
    FILE *fp = fopen(path, "rb");
    if(!fp) return NULL;
    JpegContext ctx = {0};
    if(load_jpeg_header(fp, &ctx) != 0 || ctx.is_progressive) { 
        if(ctx.is_progressive) printf("Progressive JPEG not supported for decoding.\n");
        fclose(fp); return NULL; 
    }

    Image *img = malloc(sizeof(Image));
    img->w = ctx.w; img->h = ctx.h;
    img->rgb = malloc((size_t)ctx.w * ctx.h * 3);

    JpegBitBuf bb = {fp, 0, 0};
    int prev_dc[3] = {0};
    int mcu_w = ctx.max_h * 8, mcu_h = ctx.max_v * 8;
    int mcus_x = (ctx.w + mcu_w - 1) / mcu_w, mcus_y = (ctx.h + mcu_h - 1) / mcu_h;
    int mcu_count = 0;

    for(int my=0; my<mcus_y; my++) {
        for(int mx=0; mx<mcus_x; mx++) {
            float mcu_data[3][4][8][8] = {0};
            for(int c=0; c<ctx.comps; c++) {
                int b = 0;
                for(int v=0; v<ctx.comp_v[c]; v++) {
                    for(int u=0; u<ctx.comp_h[c]; u++) {
                        dec_block(&bb, &ctx, c, mcu_data[c][b], &prev_dc[c]); b++;
                    }
                }
            }

            for(int py=0; py<mcu_h; py++) {
                int y = my * mcu_h + py; if(y >= ctx.h) continue;
                for(int px=0; px<mcu_w; px++) {
                    int x = mx * mcu_w + px; if(x >= ctx.w) continue;
                    float Y=0, Cb=0, Cr=0;
                    for(int c=0; c<ctx.comps; c++) {
                        int comp_x = px * ctx.comp_h[c] / ctx.max_h;
                        int comp_y = py * ctx.comp_v[c] / ctx.max_v;
                        int b = (comp_y / 8) * ctx.comp_h[c] + (comp_x / 8);
                        float val = mcu_data[c][b][comp_y % 8][comp_x % 8];
                        if(c==0) Y=val; else if(c==1) Cb=val; else if(c==2) Cr=val;
                    }
                    if(ctx.comps == 1) { Cb = 0; Cr = 0; }
                    int R = (int)(Y + 1.402 * Cr + 128);
                    int G = (int)(Y - 0.344136 * Cb - 0.714136 * Cr + 128);
                    int B = (int)(Y + 1.772 * Cb + 128);
                    img->rgb[(y*ctx.w+x)*3+0] = R<0?0:R>255?255:R;
                    img->rgb[(y*ctx.w+x)*3+1] = G<0?0:G>255?255:G;
                    img->rgb[(y*ctx.w+x)*3+2] = B<0?0:B>255?255:B;
                }
            }

            if(ctx.rst_interval > 0 && ++mcu_count == ctx.rst_interval) {
                mcu_count = 0; bb.bits = 0; int c;
                while((c = fgetc(fp)) != EOF) {
                    if(c == 0xFF) { int m = fgetc(fp); if(m >= 0xD0 && m <= 0xD7) break; }
                }
                memset(prev_dc, 0, sizeof prev_dc);
            }
        }
    }
    fclose(fp); return img;
}

/* ════════════════════════════════════════════════════════
 *  JPEG Encoder
 * ════════════════════════════════════════════════════════ */

typedef struct { FILE *fp; uint8_t buf; int fill; } BitBufEnc;

static void bb_init(BitBufEnc *b, FILE *fp) { b->fp=fp; b->buf=0; b->fill=0; }
static void bb_emit(BitBufEnc *b, uint8_t byte) { fputc(byte, b->fp); if(byte == 0xFF) fputc(0x00, b->fp); }
static void bb_write(BitBufEnc *b, uint32_t bits, int n) {
    for (int i = n-1; i >= 0; i--) {
        b->buf = (b->buf << 1) | ((bits >> i) & 1);
        if (++b->fill == 8) { bb_emit(b, b->buf); b->buf=0; b->fill=0; }
    }
}
static void bb_flush(BitBufEnc *b) { if(b->fill>0) { b->buf <<= (8 - b->fill); bb_emit(b, b->buf); } }

static void fdct(const float in[8][8], float out[8][8]) {
    static const double PI = 3.14159265358979323846;
    static const double INV_SQRT2 = 0.70710678118654752440;
    for (int u = 0; u < 8; u++) {
        double cu = (u == 0) ? INV_SQRT2 : 1.0;
        for (int v = 0; v < 8; v++) {
            double cv = (v == 0) ? INV_SQRT2 : 1.0;
            double s  = 0.0;
            for (int x = 0; x < 8; x++)
                for (int y = 0; y < 8; y++)
                    s += in[x][y] * cos((2*x+1)*u*PI/16.0) * cos((2*y+1)*v*PI/16.0);
            out[u][v] = (float)(0.25 * cu * cv * s);
        }
    }
}

static void enc_block(BitBufEnc *bb, const float blk[8][8], int *prev_dc, const uint8_t *qt,
                      const HuffTab *dc_ht, const HuffTab *ac_ht) {
    float F[8][8]; fdct(blk, F);
    int coeff[64];
    for (int k=0; k<64; k++) { int zz = ZIGZAG[k]; coeff[k] = (int)roundf(F[zz/8][zz%8] / qt[k]); }

    int diff = coeff[0] - *prev_dc; *prev_dc = coeff[0];
    int absd = diff < 0 ? -diff : diff, cat = 0;
    while ((1<<cat) <= absd) cat++;
    bb_write(bb, dc_ht->code[cat], dc_ht->size[cat]);
    if (cat > 0) bb_write(bb, diff >= 0 ? diff : diff+(1<<cat)-1, cat);

    int run = 0;
    for (int k = 1; k < 64; k++) {
        int ac = coeff[k];
        if (ac == 0) { if (++run == 16) { bb_write(bb, ac_ht->code[0xF0], ac_ht->size[0xF0]); run = 0; } } 
        else {
            int absa = ac < 0 ? -ac : ac, size = 0;
            while ((1<<size) <= absa) size++;
            int sym = (run << 4) | size;
            bb_write(bb, ac_ht->code[sym], ac_ht->size[sym]);
            bb_write(bb, ac >= 0 ? ac : ac+(1<<size)-1, size);
            run = 0;
        }
    }
    if (run > 0) bb_write(bb, ac_ht->code[0x00], ac_ht->size[0x00]);
}

static void put16(FILE *fp, uint16_t v) { fputc(v>>8,fp); fputc(v&0xFF,fp); }

static void scale_qt(uint8_t *out, const uint8_t *base, int q) {
    if (q < 1) q=1; else if (q>100) q=100;
    int s = (q < 50) ? (5000/q) : (200-2*q);
    for (int i=0;i<64;i++) {
        int v = ((int)base[i]*s+50)/100;
        out[i] = (uint8_t)(v<1?1:(v>255?255:v));
    }
}

static int save_jpeg(const Image *img, const char *path, int quality) {
    FILE *fp = fopen(path,"wb");
    if (!fp) return -1;
    uint8_t lum_qt[64], chr_qt[64];
    scale_qt(lum_qt, BASE_LUM_QT, quality); scale_qt(chr_qt, BASE_CHR_QT, quality);

    int nDCL=count_vals(DC_LUM_BITS), nDCC=count_vals(DC_CHR_BITS);
    int nACL=count_vals(AC_LUM_BITS), nACC=count_vals(AC_CHR_BITS);
    HuffTab ht_dcL, ht_dcC, ht_acL, ht_acC;
    build_huff(&ht_dcL, DC_LUM_BITS, DC_LUM_VALS, nDCL); build_huff(&ht_dcC, DC_CHR_BITS, DC_CHR_VALS, nDCC);
    build_huff(&ht_acL, AC_LUM_BITS, AC_LUM_VALS, nACL); build_huff(&ht_acC, AC_CHR_BITS, AC_CHR_VALS, nACC);

    put16(fp,0xFFD8); // SOI
    put16(fp,0xFFE0); put16(fp,16); fputs("JFIF",fp); fputc(0,fp); put16(fp,0x0101); fputc(0,fp); put16(fp,1); put16(fp,1); put16(fp,0);
    put16(fp,0xFFDB); put16(fp,67); fputc(0,fp); fwrite(lum_qt,1,64,fp);
    put16(fp,0xFFDB); put16(fp,67); fputc(1,fp); fwrite(chr_qt,1,64,fp);
    put16(fp,0xFFC0); put16(fp,17); fputc(8,fp); put16(fp,img->h); put16(fp,img->w); fputc(3,fp);
    fputc(1,fp); fputc(0x11,fp); fputc(0,fp); fputc(2,fp); fputc(0x11,fp); fputc(1,fp); fputc(3,fp); fputc(0x11,fp); fputc(1,fp);
    put16(fp,0xFFC4); put16(fp,19+nDCL); fputc(0x00,fp); fwrite(DC_LUM_BITS+1,1,16,fp); fwrite(DC_LUM_VALS,1,nDCL,fp);
    put16(fp,0xFFC4); put16(fp,19+nDCC); fputc(0x01,fp); fwrite(DC_CHR_BITS+1,1,16,fp); fwrite(DC_CHR_VALS,1,nDCC,fp);
    put16(fp,0xFFC4); put16(fp,19+nACL); fputc(0x10,fp); fwrite(AC_LUM_BITS+1,1,16,fp); fwrite(AC_LUM_VALS,1,nACL,fp);
    put16(fp,0xFFC4); put16(fp,19+nACC); fputc(0x11,fp); fwrite(AC_CHR_BITS+1,1,16,fp); fwrite(AC_CHR_VALS,1,nACC,fp);
    put16(fp,0xFFDA); put16(fp,12); fputc(3,fp); fputc(1,fp); fputc(0x00,fp); fputc(2,fp); fputc(0x11,fp); fputc(3,fp); fputc(0x11,fp); fputc(0,fp); fputc(63,fp); fputc(0,fp);

    BitBufEnc bb; bb_init(&bb,fp);
    int dc_Y=0, dc_Cb=0, dc_Cr=0, W=img->w, H=img->h;
    for (int by=0; by<((H+7)&~7); by+=8) {
        for (int bx=0; bx<((W+7)&~7); bx+=8) {
            float Y[8][8], Cb[8][8], Cr[8][8];
            for (int dy=0; dy<8; dy++) {
                int py = by+dy; if (py>=H) py=H-1;
                for (int dx=0; dx<8; dx++) {
                    int px = bx+dx; if (px>=W) px=W-1;
                    float R = img->rgb[(py*W+px)*3+0], G = img->rgb[(py*W+px)*3+1], B = img->rgb[(py*W+px)*3+2];
                    Y[dy][dx]  =  0.29900f*R + 0.58700f*G + 0.11400f*B - 128.0f;
                    Cb[dy][dx] = -0.16874f*R - 0.33126f*G + 0.50000f*B;
                    Cr[dy][dx] =  0.50000f*R - 0.41869f*G - 0.08131f*B;
                }
            }
            enc_block(&bb, Y, &dc_Y, lum_qt, &ht_dcL, &ht_acL);
            enc_block(&bb, Cb, &dc_Cb, chr_qt, &ht_dcC, &ht_acC);
            enc_block(&bb, Cr, &dc_Cr, chr_qt, &ht_dcC, &ht_acC);
        }
    }
    bb_flush(&bb); put16(fp,0xFFD9); fclose(fp);
    return 0;
}

/* ════════════════════════════════════════════════════════
 *  Main CLI Routing
 * ════════════════════════════════════════════════════════ */

int main(int argc, char *argv[]) {
    if (argc == 2) {
        if (endswith(argv[1], ".ppm")) show_ppm_info(argv[1]);
        else if (endswith(argv[1], ".jpg") || endswith(argv[1], ".jpeg")) show_jpeg_info(argv[1]);
        else printf("Unsupported format for info.\n");
        return 0;
    } else if (argc >= 4 && strcmp(argv[2], "-o") == 0) {
        if (endswith(argv[1], ".ppm") && (endswith(argv[3], ".jpg") || endswith(argv[3], ".jpeg"))) {
            Image *img = load_ppm(argv[1]);
            if (!img) { printf("Failed to load PPM.\n"); return 1; }
            int q = (argc >= 5) ? atoi(argv[4]) : 75;
            if (save_jpeg(img, argv[3], q) == 0) printf("Converted: %s -> %s (Q=%d)\n", argv[1], argv[3], q);
            else printf("Error writing JPEG.\n");
            free(img->rgb); free(img);
        } else if ((endswith(argv[1], ".jpg") || endswith(argv[1], ".jpeg")) && endswith(argv[3], ".ppm")) {
            Image *img = load_jpeg(argv[1]);
            if (!img) { printf("Failed to decode JPEG.\n"); return 1; }
            if (save_ppm(img, argv[3]) == 0) printf("Converted: %s -> %s\n", argv[1], argv[3]);
            else printf("Error writing PPM.\n");
            free(img->rgb); free(img);
        } else {
            printf("Unsupported conversion path. Only PPM<->JPG is supported.\n");
        }
        return 0;
    }

    printf("Usage:\n");
    printf("  Show PPM info   : %s  test.ppm\n", argv[0]);
    printf("  Show JPG info   : %s  test.jpg\n", argv[0]);
    printf("  Convert PPM->JPG: %s  test.ppm -o test.jpg [quality]\n", argv[0]);
    printf("  Convert JPG->PPM: %s  test.jpg -o test.ppm\n", argv[0]);
    return 1;
}