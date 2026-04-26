/*
 * gmailget.c  -  用 POP3S 抓 Gmail，寫成 mailtoday.md
 *
 * 編譯（macOS）:
 *   gcc -o gmailget gmailget.c -lssl -lcrypto
 *
 * 編譯（Linux/Ubuntu）:
 *   sudo apt install libssl-dev
 *   gcc -o gmailget gmailget.c -lssl -lcrypto
 *
 * 使用方式:
 *   ./gmailget ccckmit@gmail.com "應用程式密碼(16碼)"
 *
 * Gmail 前置作業：
 *   1. Google 帳號 -> 安全性 -> 兩步驟驗證 (開啟)
 *   2. Google 帳號 -> 安全性 -> 應用程式密碼 -> 產生 16 碼
 *   3. Gmail 設定 -> 轉寄和 POP/IMAP -> 啟用 POP (所有郵件)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>

/* OpenSSL */
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

/* ================================================================
 * 設定
 * ================================================================ */
#define POP3_HOST    "pop.gmail.com"
#define POP3_PORT    "995"
#define OUTPUT_FILE  "mailtoday.md"
#define MAX_LINE     8192
#define MAX_ATTACH   64
#define INIT_BUF     (1 << 17)   /* 128 KB */

/* ================================================================
 * SSL 連線包裝
 * ================================================================ */
typedef struct {
    SSL_CTX *ctx;
    SSL     *ssl;
    BIO     *bio;
} Conn;

static int conn_open(Conn *c, const char *host, const char *port) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    c->ctx = SSL_CTX_new(TLS_client_method());
    if (!c->ctx) { fprintf(stderr, "SSL_CTX_new failed\n"); return -1; }

    /* 驗證伺服器憑證 */
    SSL_CTX_set_verify(c->ctx, SSL_VERIFY_PEER, NULL);
    SSL_CTX_set_default_verify_paths(c->ctx);
    /* macOS: Homebrew OpenSSL 不使用系統憑證，手動指定常見路徑 */
    SSL_CTX_load_verify_locations(c->ctx, NULL, "/etc/ssl/certs");
    SSL_CTX_load_verify_locations(c->ctx, "/etc/ssl/cert.pem", NULL);
    SSL_CTX_load_verify_locations(c->ctx,
        "/opt/homebrew/etc/openssl@3/cert.pem", NULL);
    SSL_CTX_load_verify_locations(c->ctx,
        "/usr/local/etc/openssl@3/cert.pem", NULL);

    char addr[256];
    snprintf(addr, sizeof(addr), "%s:%s", host, port);

    c->bio = BIO_new_ssl_connect(c->ctx);
    BIO_get_ssl(c->bio, &c->ssl);
    SSL_set_mode(c->ssl, SSL_MODE_AUTO_RETRY);

    BIO_set_conn_hostname(c->bio, addr);

    if (BIO_do_connect(c->bio) <= 0) {
        fprintf(stderr, "BIO_do_connect(%s) failed: ", addr);
        ERR_print_errors_fp(stderr);
        return -1;
    }
    if (BIO_do_handshake(c->bio) <= 0) {
        fprintf(stderr, "TLS handshake failed\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    return 0;
}

static void conn_close(Conn *c) {
    if (c->bio) BIO_free_all(c->bio);
    if (c->ctx) SSL_CTX_free(c->ctx);
}

/* 讀一行（以 \r\n 或 \n 結尾），去掉尾端 CR/LF */
static int conn_readline(Conn *c, char *buf, int bufsz) {
    int i = 0;
    char ch;
    while (i < bufsz - 1) {
        int n = BIO_read(c->bio, &ch, 1);
        if (n <= 0) break;
        if (ch == '\n') break;
        if (ch != '\r') buf[i++] = ch;
    }
    buf[i] = '\0';
    return i;
}

/* 送出一行指令（自動加 \r\n） */
static int conn_writeline(Conn *c, const char *cmd) {
    char buf[512];
    int len = snprintf(buf, sizeof(buf), "%s\r\n", cmd);
    int n = BIO_write(c->bio, buf, len);
    return (n == len) ? 0 : -1;
}

/* 讀一行，檢查是否以 "+OK" 開頭 */
static int expect_ok(Conn *c, char *out, int outsz) {
    conn_readline(c, out, outsz);
    if (strncmp(out, "+OK", 3) != 0) {
        fprintf(stderr, "POP3 error: %s\n", out);
        return -1;
    }
    return 0;
}

/* ================================================================
 * 動態 buffer（用來接整封信）
 * ================================================================ */
typedef struct { char *data; size_t len; size_t cap; } Buf;

static void buf_init(Buf *b) {
    b->cap  = INIT_BUF;
    b->data = malloc(b->cap);
    b->len  = 0;
    if (b->data) b->data[0] = '\0';
}

static void buf_append(Buf *b, const char *s, size_t n) {
    if (b->len + n + 1 >= b->cap) {
        while (b->len + n + 1 >= b->cap) b->cap *= 2;
        b->data = realloc(b->data, b->cap);
    }
    memcpy(b->data + b->len, s, n);
    b->len += n;
    b->data[b->len] = '\0';
}

static void buf_free(Buf *b) { free(b->data); b->data = NULL; b->len = b->cap = 0; }

/* ================================================================
 * Email 解析（從 mail0c.c 搬來）
 * ================================================================ */
static char *trim_inplace(char *s) {
    if (!s) return s;
    while (*s && isspace((unsigned char)*s)) s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end-1))) end--;
    *end = '\0';
    return s;
}

static int istarts(const char *s, const char *pre) {
    return strncasecmp(s, pre, strlen(pre)) == 0;
}

static int extract_filename(const char *line, char *out, int outsz) {
    const char *p = strcasestr(line, "filename=");
    if (!p) return 0;
    p += 9;
    if (*p == '"') {
        p++;
        const char *q = strchr(p, '"');
        int len = q ? (int)(q-p) : (int)strlen(p);
        if (len >= outsz) len = outsz-1;
        strncpy(out, p, len); out[len] = '\0';
    } else {
        int len = 0;
        while (p[len] && p[len]!=';' && !isspace((unsigned char)p[len])) len++;
        if (len >= outsz) len = outsz-1;
        strncpy(out, p, len); out[len] = '\0';
    }
    return out[0] != '\0';
}

typedef struct { char *key; char *val; } KV;

static const char *split_header_body(const char *raw, size_t rawlen, char **hdr) {
    for (size_t i = 0; i+1 < rawlen; i++) {
        if (raw[i]=='\n' && raw[i+1]=='\n') {
            *hdr = strndup(raw, i); return raw+i+2;
        }
        if (i+3 < rawlen &&
            raw[i]=='\r' && raw[i+1]=='\n' &&
            raw[i+2]=='\r' && raw[i+3]=='\n') {
            *hdr = strndup(raw, i); return raw+i+4;
        }
    }
    *hdr = strndup(raw, rawlen);
    return raw + rawlen;
}

static KV *parse_headers(char *h, int *cnt) {
    /* \r\n -> \n */
    { char *p=h, *q=h;
      while(*p){ if(*p=='\r'&&*(p+1)=='\n') p++; *q++=*p++; } *q='\0'; }
    /* folding */
    { char *p=h;
      while(*p){ char *nl=strchr(p,'\n'); if(!nl) break;
        char *nx=nl+1;
        if(*nx==' '||*nx=='\t'){
          *nl=' '; char *s=nx; while(*s==' '||*s=='\t') s++;
          memmove(nx,s,strlen(s)+1); p=nl;
        } else p=nx; } }

    int cap=64; KV *kv=malloc(cap*sizeof(KV)); int n=0;
    char *line=h;
    while(*line){
        char *nl=strchr(line,'\n');
        size_t ll=nl?(size_t)(nl-line):strlen(line);
        char lb[MAX_LINE]; if(ll>=MAX_LINE) ll=MAX_LINE-1;
        strncpy(lb,line,ll); lb[ll]='\0';
        char *col=strchr(lb,':');
        if(col){ *col='\0';
          char *k=trim_inplace(lb), *v=trim_inplace(col+1);
          if(n>=cap){cap*=2; kv=realloc(kv,cap*sizeof(KV));}
          kv[n].key=strdup(k); kv[n].val=strdup(v); n++; }
        if(!nl) break;
        line=nl+1;
    }
    *cnt=n; return kv;
}

static char *kv_get(KV *kv, int n, const char *key) {
    for(int i=0;i<n;i++) if(strcasecmp(kv[i].key,key)==0) return kv[i].val;
    return "";
}
static void kv_free(KV *kv, int n) {
    for(int i=0;i<n;i++){free(kv[i].key);free(kv[i].val);} free(kv);
}

static char *get_boundary(const char *ct) {
    const char *bp=strcasestr(ct,"boundary="); if(!bp) return NULL;
    bp+=9;
    if(*bp=='"'){ bp++; const char *eq=strchr(bp,'"');
      return strndup(bp, eq?(size_t)(eq-bp):strlen(bp)); }
    int l=0;
    while(bp[l]&&bp[l]!=';'&&!isspace((unsigned char)bp[l])) l++;
    return strndup(bp,l);
}

static void parse_multipart(const char *body, size_t blen,
                             const char *boundary,
                             char **out_text,
                             char **attachments, int *nattach) {
    char delim[512]; snprintf(delim,sizeof(delim),"--%s",boundary);
    size_t dlen=strlen(delim);
    const char *pos=body, *end=body+blen;
    while(pos<end){
        const char *bd=memmem(pos,end-pos,delim,dlen); if(!bd) break;
        const char *after=bd+dlen;
        if(after+1<end&&after[0]=='-'&&after[1]=='-') break;
        if(after<end&&*after=='\r') after++;
        if(after<end&&*after=='\n') after++;
        const char *nbd=memmem(after,end-after,delim,dlen);
        const char *pend=nbd?nbd:end;

        char *phdr; const char *pbody=split_header_body(after,pend-after,&phdr);
        int nkv=0; KV *pkv=parse_headers(phdr,&nkv); free(phdr);
        char *pct=kv_get(pkv,nkv,"Content-Type");
        char *pdi=kv_get(pkv,nkv,"Content-Disposition");
        char fname[256]="";
        extract_filename(pdi,fname,256);
        if(!fname[0]) extract_filename(pct,fname,256);
        size_t pl=(size_t)(pend-pbody);
        while(pl>0&&(pbody[pl-1]=='\n'||pbody[pl-1]=='\r')) pl--;
        if((istarts(pdi,"attachment")||fname[0])&&fname[0]){
            if(*nattach<MAX_ATTACH) attachments[(*nattach)++]=strdup(fname);
        } else if(istarts(pct,"text/plain")&&*out_text==NULL&&pl>0){
            *out_text=strndup(pbody,pl);
        }
        kv_free(pkv,nkv); pos=nbd?nbd:end;
    }
}

/* ================================================================
 * Base64 解碼（用於 quoted-printable / base64 Subject 等）
 * 這裡只做簡單的 RFC 2047 encoded-word 解碼
 * ================================================================ */
static const char b64t[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static int b64val(char c) {
    const char *p = strchr(b64t, c);
    return p ? (int)(p - b64t) : -1;
}

/* 解碼一段 base64，回傳 malloc 字串 */
static char *b64decode(const char *src, size_t slen) {
    size_t out_cap = slen * 3 / 4 + 4;
    char *out = malloc(out_cap);
    size_t oi = 0;
    int acc = 0, bits = 0;
    for (size_t i = 0; i < slen; i++) {
        if (src[i] == '=') break;
        int v = b64val(src[i]);
        if (v < 0) continue;
        acc = (acc << 6) | v;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out[oi++] = (char)((acc >> bits) & 0xFF);
        }
    }
    out[oi] = '\0';
    return out;
}

/* RFC 2047: =?charset?B?...?= 或 =?charset?Q?...?=
 * 只做基本解碼，把結果拼回來 */
static char *decode_rfc2047(const char *s) {
    char *result = malloc(strlen(s) * 2 + 4);
    result[0] = '\0';
    size_t ri = 0;
    const char *p = s;
    while (*p) {
        if (strncmp(p, "=?", 2) == 0) {
            /* 找 ?B? 或 ?Q? */
            const char *q = p + 2;
            const char *cs_end = strchr(q, '?');
            if (!cs_end) { result[ri++] = *p++; continue; }
            char enc = toupper((unsigned char)*(cs_end+1));
            if (*(cs_end+1) == '\0' || *(cs_end+2) != '?') {
                result[ri++] = *p++; continue;
            }
            const char *data_start = cs_end + 3;
            const char *data_end   = strstr(data_start, "?=");
            if (!data_end) { result[ri++] = *p++; continue; }

            if (enc == 'B') {
                char *dec = b64decode(data_start, data_end - data_start);
                size_t dl = strlen(dec);
                memcpy(result + ri, dec, dl);
                ri += dl;
                free(dec);
            } else if (enc == 'Q') {
                /* quoted-printable */
                for (const char *d = data_start; d < data_end; d++) {
                    if (*d == '_') { result[ri++] = ' '; }
                    else if (*d == '=' && d+2 < data_end) {
                        char hex[3] = { d[1], d[2], 0 };
                        result[ri++] = (char)strtol(hex, NULL, 16);
                        d += 2;
                    } else { result[ri++] = *d; }
                }
            }
            p = data_end + 2;
            /* 跳過 encoded-word 之間的空白 */
            while (*p == ' ' || *p == '\t') {
                if (strncmp(p+1, "=?", 2) == 0) p++;
                else { result[ri++] = *p++; break; }
                /* 若下一個也是 encoded-word，直接跳過空白 */
                if (strncmp(p, " ", 1)==0 && strncmp(p+1,"=?",2)==0) p++;
                else break;
            }
        } else {
            result[ri++] = *p++;
        }
    }
    result[ri] = '\0';
    return result;
}

/* ================================================================
 * 把一封原始信件（Buf）寫入 mailtoday.md
 * ================================================================ */
static void write_mail_to_md(const char *raw, size_t rawlen) {
    char *hdr_copy;
    const char *body_start = split_header_body(raw, rawlen, &hdr_copy);
    size_t body_len = rawlen - (size_t)(body_start - raw);

    int nkv = 0;
    KV *kvs = parse_headers(hdr_copy, &nkv);
    free(hdr_copy);

    char *from_raw = kv_get(kvs, nkv, "From");
    char *subj_raw = kv_get(kvs, nkv, "Subject");
    char *date_val = kv_get(kvs, nkv, "Date");
    char *ctype    = kv_get(kvs, nkv, "Content-Type");

    /* 解碼 RFC 2047 編碼的 From / Subject */
    char *from_val = decode_rfc2047(from_raw);
    char *subject  = decode_rfc2047(subj_raw);

    /* 時間字串 */
    char time_str[64];
    if (date_val[0]) {
        struct tm tm_val; memset(&tm_val, 0, sizeof(tm_val));
        char *ok = strptime(date_val, "%a, %d %b %Y %H:%M:%S %z", &tm_val);
        if (!ok) ok = strptime(date_val, "%d %b %Y %H:%M:%S %z", &tm_val);
        if (ok) strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S %z", &tm_val);
        else { strncpy(time_str, date_val, 63); time_str[63]='\0'; }
    } else {
        time_t now = time(NULL);
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    }

    /* 解析 body */
    char *body_text = NULL;
    char *attachments[MAX_ATTACH]; int nattach = 0;
    char *boundary = NULL;
    if (istarts(ctype, "multipart/")) boundary = get_boundary(ctype);

    if (boundary && body_len > 0) {
        parse_multipart(body_start, body_len, boundary,
                        &body_text, attachments, &nattach);
    } else if (body_len > 0) {
        size_t bl = body_len;
        while (bl > 0 && (body_start[bl-1]=='\n'||body_start[bl-1]=='\r')) bl--;
        if (bl > 0) body_text = strndup(body_start, bl);
    }

    /* 寫入 md */
    FILE *fp = fopen(OUTPUT_FILE, "a");
    if (!fp) { perror("fopen"); goto cleanup; }

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

cleanup:
    kv_free(kvs, nkv);
    free(body_text);
    free(boundary);
    free(from_val);
    free(subject);
}

/* ================================================================
 * POP3 抓信主流程
 * ================================================================ */
static int pop3_fetch(const char *user, const char *pass) {
    Conn c = {0};
    char line[MAX_LINE];

    printf("[1/6] 連線到 %s:%s ...\n", POP3_HOST, POP3_PORT);
    if (conn_open(&c, POP3_HOST, POP3_PORT) < 0) return -1;

    /* 收 greeting */
    if (expect_ok(&c, line, sizeof(line)) < 0) goto err;
    printf("      伺服器: %s\n", line);

    /* USER */
    printf("[2/6] 登入 %s ...\n", user);
    char cmd[512];
    snprintf(cmd, sizeof(cmd), "USER %s", user);
    conn_writeline(&c, cmd);
    if (expect_ok(&c, line, sizeof(line)) < 0) goto err;

    /* PASS */
    snprintf(cmd, sizeof(cmd), "PASS %s", pass);
    conn_writeline(&c, cmd);
    if (expect_ok(&c, line, sizeof(line)) < 0) {
        fprintf(stderr, "登入失敗，請確認應用程式密碼是否正確\n");
        goto err;
    }
    printf("      登入成功\n");

    /* STAT：取得信件總數 */
    printf("[3/6] 查詢信箱狀態 ...\n");
    conn_writeline(&c, "STAT");
    if (expect_ok(&c, line, sizeof(line)) < 0) goto err;
    int total = 0, total_oct = 0;
    sscanf(line + 4, "%d %d", &total, &total_oct);
    printf("      共 %d 封信\n", total);

    if (total == 0) {
        printf("      沒有新信件\n");
        conn_writeline(&c, "QUIT");
        conn_readline(&c, line, sizeof(line));
        conn_close(&c);
        return 0;
    }

    /* 清空今日的 md，重新寫入 */
    printf("[4/6] 清空 %s，重新寫入 ...\n", OUTPUT_FILE);
    FILE *fp = fopen(OUTPUT_FILE, "w");
    if (fp) fclose(fp);

    /* RETR 每封信 */
    printf("[5/6] 下載並解析信件 ...\n");
    int fetched = 0;
    for (int i = 1; i <= total; i++) {
        snprintf(cmd, sizeof(cmd), "RETR %d", i);
        conn_writeline(&c, cmd);
        if (expect_ok(&c, line, sizeof(line)) < 0) {
            fprintf(stderr, "  RETR %d 失敗，跳過\n", i);
            continue;
        }

        /* 讀取信件內容直到獨立的 "." 行 */
        Buf mail; buf_init(&mail);
        while (1) {
            conn_readline(&c, line, sizeof(line));
            /* POP3 byte-stuffing: 行首 ".." -> "." */
            if (strcmp(line, ".") == 0) break;
            const char *content = (line[0]=='.'&&line[1]=='.') ? line+1 : line;
            buf_append(&mail, content, strlen(content));
            buf_append(&mail, "\n", 1);
        }

        printf("  [%d/%d] 解析信件 (%zu bytes)\n", i, total, mail.len);
        write_mail_to_md(mail.data, mail.len);
        buf_free(&mail);
        fetched++;
    }

    /* QUIT */
    printf("[6/6] 完成，共寫入 %d 封，傳送 QUIT\n", fetched);
    conn_writeline(&c, "QUIT");
    conn_readline(&c, line, sizeof(line));

    conn_close(&c);
    printf("\n已寫入 %s\n", OUTPUT_FILE);
    return 0;

err:
    conn_close(&c);
    return -1;
}

/* ================================================================
 * main
 * ================================================================ */
int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr,
            "用法: %s <gmail帳號> <應用程式密碼>\n"
            "範例: %s ccckmit@gmail.com \"abcd efgh ijkl mnop\"\n"
            "\n"
            "Gmail 前置作業:\n"
            "  1. Google 帳號 -> 安全性 -> 啟用兩步驟驗證\n"
            "  2. Google 帳號 -> 安全性 -> 應用程式密碼 -> 產生 16 碼\n"
            "  3. Gmail 設定 -> 轉寄和 POP/IMAP -> 啟用 POP\n",
            argv[0], argv[0]);
        return 1;
    }

    /* 把 "abcd efgh ijkl mnop" 中的空格去掉 */
    char pass[64] = "";
    const char *src = argv[2];
    int pi = 0;
    while (*src && pi < 63) {
        if (*src != ' ') pass[pi++] = *src;
        src++;
    }
    pass[pi] = '\0';

    return pop3_fetch(argv[1], pass) == 0 ? 0 : 1;
}