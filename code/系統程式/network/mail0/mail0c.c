/*
 * mail0c.c - 從 stdin 讀取 email (RFC 2822 格式)，附加到 mailtoday.md
 *
 * 用法：
 *   echo "..." | ./mail0c
 *   或由 MTA 呼叫（如 .forward / aliases）
 *
 * 編譯：gcc -o mail0c mail0c.c
 */

#define _GNU_SOURCE   /* memmem, strptime, strcasestr */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define OUTPUT_FILE  "mailtoday.md"
#define MAX_LINE     4096
#define MAX_ATTACH   64
#define INIT_BUF     (1 << 16)   /* 64 KB 起始 */

/* ---------- 工具函式 ---------- */

static char *trim_inplace(char *s) {
    if (!s) return s;
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1))) end--;
    *end = '\0';
    return s;
}

static int istarts(const char *s, const char *prefix) {
    return strncasecmp(s, prefix, strlen(prefix)) == 0;
}

static int extract_filename(const char *line, char *out, int outsz) {
    const char *p = strcasestr(line, "filename=");
    if (!p) return 0;
    p += 9;
    if (*p == '"') {
        p++;
        const char *q = strchr(p, '"');
        int len = q ? (int)(q - p) : (int)strlen(p);
        if (len >= outsz) len = outsz - 1;
        strncpy(out, p, len);
        out[len] = '\0';
    } else {
        int len = 0;
        while (p[len] && p[len] != ';' && !isspace((unsigned char)p[len])) len++;
        if (len >= outsz) len = outsz - 1;
        strncpy(out, p, len);
        out[len] = '\0';
    }
    return out[0] != '\0';
}

/* ---------- 讀取全部 stdin ---------- */
static char *read_all_stdin(size_t *out_len) {
    size_t cap = INIT_BUF, len = 0;
    char *buf = malloc(cap);
    if (!buf) return NULL;
    int c;
    while ((c = getchar()) != EOF) {
        if (len + 2 >= cap) {
            cap *= 2;
            char *nb = realloc(buf, cap);
            if (!nb) { free(buf); return NULL; }
            buf = nb;
        }
        buf[len++] = (char)c;
    }
    buf[len] = '\0';
    *out_len = len;
    return buf;
}

/* ---------- 分離 header / body ----------
 * 以第一個空行（\n\n 或 \r\n\r\n）為分界。
 * 回傳指向 raw 中 body 開頭的指標，並設定 *hdr_copy 為複製的 header 文字。
 */
static const char *split_header_body(const char *raw, size_t rawlen,
                                      char **hdr_copy) {
    for (size_t i = 0; i + 1 < rawlen; i++) {
        if (raw[i] == '\n' && raw[i+1] == '\n') {
            *hdr_copy = strndup(raw, i);
            return raw + i + 2;
        }
        if (i + 3 < rawlen &&
            raw[i]=='\r' && raw[i+1]=='\n' &&
            raw[i+2]=='\r' && raw[i+3]=='\n') {
            *hdr_copy = strndup(raw, i);
            return raw + i + 4;
        }
    }
    /* 沒有空行 → 全部是 header，body 為空 */
    *hdr_copy = strndup(raw, rawlen);
    return raw + rawlen;
}

/* ---------- 解析 header 區段 ---------- */
typedef struct { char *key; char *val; } KV;

static KV *parse_headers(char *hdr_text, int *count) {
    /* \r\n → \n */
    {
        char *p = hdr_text, *q = hdr_text;
        while (*p) {
            if (*p == '\r' && *(p+1) == '\n') p++;
            *q++ = *p++;
        }
        *q = '\0';
    }
    /* 展開 header folding（行首空白 = 接續上一行） */
    {
        char *p = hdr_text;
        while (*p) {
            char *nl = strchr(p, '\n');
            if (!nl) break;
            char *next = nl + 1;
            if (*next == ' ' || *next == '\t') {
                *nl = ' ';
                char *src = next;
                while (*src == ' ' || *src == '\t') src++;
                memmove(next, src, strlen(src) + 1);
                p = nl;
            } else {
                p = next;
            }
        }
    }

    int cap = 64;
    KV *kvs = malloc(cap * sizeof(KV));
    int n = 0;
    if (!kvs) { *count = 0; return NULL; }

    char *line = hdr_text;
    while (*line) {
        char *nl = strchr(line, '\n');
        size_t llen = nl ? (size_t)(nl - line) : strlen(line);
        char lbuf[MAX_LINE];
        if (llen >= MAX_LINE) llen = MAX_LINE - 1;
        strncpy(lbuf, line, llen);
        lbuf[llen] = '\0';

        char *colon = strchr(lbuf, ':');
        if (colon) {
            *colon = '\0';
            char *k = trim_inplace(lbuf);
            char *v = trim_inplace(colon + 1);
            if (n >= cap) { cap *= 2; kvs = realloc(kvs, cap * sizeof(KV)); }
            kvs[n].key = strdup(k);
            kvs[n].val = strdup(v);
            n++;
        }

        if (!nl) break;
        line = nl + 1;
    }
    *count = n;
    return kvs;
}

static char *kv_get(KV *kvs, int n, const char *key) {
    for (int i = 0; i < n; i++)
        if (strcasecmp(kvs[i].key, key) == 0) return kvs[i].val;
    return "";
}

static void kv_free(KV *kvs, int n) {
    for (int i = 0; i < n; i++) { free(kvs[i].key); free(kvs[i].val); }
    free(kvs);
}

static char *get_boundary(const char *ctype) {
    const char *bp = strcasestr(ctype, "boundary=");
    if (!bp) return NULL;
    bp += 9;
    if (*bp == '"') {
        bp++;
        const char *eq = strchr(bp, '"');
        return strndup(bp, eq ? (size_t)(eq - bp) : strlen(bp));
    }
    int blen = 0;
    while (bp[blen] && bp[blen] != ';' && !isspace((unsigned char)bp[blen])) blen++;
    return strndup(bp, blen);
}

/* ---------- 解析 multipart ---------- */
static void parse_multipart(const char *body, size_t body_len,
                             const char *boundary,
                             char **out_text,
                             char **attachments, int *nattach) {
    char delim[512];
    snprintf(delim, sizeof(delim), "--%s", boundary);
    size_t dlen = strlen(delim);

    const char *pos = body;
    const char *end = body + body_len;

    while (pos < end) {
        const char *bd = memmem(pos, end - pos, delim, dlen);
        if (!bd) break;

        const char *after = bd + dlen;
        if (after + 1 < end && after[0] == '-' && after[1] == '-') break;
        if (after < end && *after == '\r') after++;
        if (after < end && *after == '\n') after++;

        const char *next_bd = memmem(after, end - after, delim, dlen);
        const char *part_end = next_bd ? next_bd : end;

        /* 分離此 part 的 header / body */
        char *part_hdr_copy;
        const char *part_body = split_header_body(after, part_end - after, &part_hdr_copy);
        int nkv = 0;
        KV *pkv = parse_headers(part_hdr_copy, &nkv);
        free(part_hdr_copy);

        char *pctype = kv_get(pkv, nkv, "Content-Type");
        char *pdisp  = kv_get(pkv, nkv, "Content-Disposition");

        char fname[256] = "";
        extract_filename(pdisp, fname, 256);
        if (!fname[0]) extract_filename(pctype, fname, 256);

        size_t pblen = (size_t)(part_end - part_body);
        while (pblen > 0 &&
               (part_body[pblen-1] == '\n' || part_body[pblen-1] == '\r')) pblen--;

        int is_attach = (istarts(pdisp, "attachment") || fname[0]);
        int is_text   = istarts(pctype, "text/plain");

        if (is_attach && fname[0]) {
            if (*nattach < MAX_ATTACH)
                attachments[(*nattach)++] = strdup(fname);
        } else if (is_text && *out_text == NULL && pblen > 0) {
            *out_text = strndup(part_body, pblen);
        }

        kv_free(pkv, nkv);
        pos = next_bd ? next_bd : end;
    }
}

/* ---------- main ---------- */
int main(void) {
    size_t rawlen;
    char *raw = read_all_stdin(&rawlen);
    if (!raw) { fprintf(stderr, "mail0c: 讀取 stdin 失敗\n"); return 1; }

    char *hdr_copy;
    const char *body_start = split_header_body(raw, rawlen, &hdr_copy);
    size_t body_len = rawlen - (size_t)(body_start - raw);

    int nkv = 0;
    KV *kvs = parse_headers(hdr_copy, &nkv);
    free(hdr_copy);

    char *from_val = kv_get(kvs, nkv, "From");
    char *subject  = kv_get(kvs, nkv, "Subject");
    char *date_val = kv_get(kvs, nkv, "Date");
    char *ctype    = kv_get(kvs, nkv, "Content-Type");

    /* 時間字串 */
    char time_str[64];
    if (date_val[0]) {
        struct tm tm_val;
        memset(&tm_val, 0, sizeof(tm_val));
        char *ok = strptime(date_val, "%a, %d %b %Y %H:%M:%S %z", &tm_val);
        if (!ok) ok = strptime(date_val, "%d %b %Y %H:%M:%S %z", &tm_val);
        if (ok) {
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S %z", &tm_val);
        } else {
            strncpy(time_str, date_val, sizeof(time_str) - 1);
            time_str[sizeof(time_str) - 1] = '\0';
        }
    } else {
        time_t now = time(NULL);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    }

    /* 解析 body */
    char *body_text = NULL;
    char *attachments[MAX_ATTACH];
    int nattach = 0;

    char *boundary = NULL;
    if (istarts(ctype, "multipart/")) boundary = get_boundary(ctype);

    if (boundary && body_len > 0) {
        parse_multipart(body_start, body_len, boundary,
                        &body_text, attachments, &nattach);
    } else if (body_len > 0) {
        size_t blen = body_len;
        while (blen > 0 &&
               (body_start[blen-1] == '\n' || body_start[blen-1] == '\r')) blen--;
        if (blen > 0) body_text = strndup(body_start, blen);
    }

    /* 寫入 mailtoday.md */
    FILE *fp = fopen(OUTPUT_FILE, "a");
    if (!fp) { perror("fopen " OUTPUT_FILE); return 1; }

    fprintf(fp, "## time: %s\n\n", time_str);
    fprintf(fp, "from: %s\n",  from_val[0] ? from_val : "(unknown)");
    fprintf(fp, "title: %s\n", subject[0]  ? subject  : "(no subject)");

    if (nattach > 0) {
        fprintf(fp, "attach: ");
        for (int i = 0; i < nattach; i++) {
            if (i > 0) fprintf(fp, ", ");
            fprintf(fp, "%s", attachments[i]);
            free(attachments[i]);
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "body:\n\n");
    if (body_text && body_text[0]) fprintf(fp, "%s\n", body_text);
    fprintf(fp, "\n");

    fclose(fp);

    kv_free(kvs, nkv);
    free(body_text);
    free(boundary);
    free(raw);

    fprintf(stderr, "mail0c: appended to %s\n", OUTPUT_FILE);
    return 0;
}