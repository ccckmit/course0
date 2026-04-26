/*
 * nosql0.c — 簡化版 BigTable NoSQL 資料庫 (C 實作)
 *
 * 架構說明：
 *   - Wide-column Store：每個 row 有 row_key，底下有多個 column family
 *   - 每個欄位支援多版本（timestamp）
 *   - 資料以 Sorted String Table (SSTable) 概念排序儲存於記憶體
 *   - 支援持久化：寫入 .db 檔案，重啟後可載入
 *
 * 指令：
 *   PUT   <table> <row_key> <col_family:col_qual> <value>
 *   GET   <table> <row_key> <col_family:col_qual>
 *   GET   <table> <row_key> <col_family:col_qual> <版本數>
 *   SCAN  <table> <row_key_prefix>
 *   DEL   <table> <row_key> <col_family:col_qual>
 *   CREATETABLE <table>
 *   DROPTABLE   <table>
 *   LISTTABLES
 *   DUMP  <table>
 *   SAVE  <table>
 *   LOAD  <table>
 *   QUIT
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

/* ── 常數 ─────────────────────────────────── */
#define MAX_TABLES       16
#define MAX_ROWS         4096
#define MAX_CELLS        64       /* 每個 row 最多欄位數 */
#define MAX_VERSIONS     8        /* 每個 cell 最多版本數 */
#define MAX_KEY_LEN      128
#define MAX_VAL_LEN      512
#define MAX_TOKENS       8

/* ── 資料結構 ──────────────────────────────── */

typedef struct {
    int64_t  timestamp;
    char     value[MAX_VAL_LEN];
} Version;

typedef struct {
    char     col_family[MAX_KEY_LEN];   /* e.g. "info" */
    char     col_qual[MAX_KEY_LEN];     /* e.g. "name" */
    int      ver_count;
    Version  versions[MAX_VERSIONS];    /* 最新版本在 index 0 */
} Cell;

typedef struct {
    char     row_key[MAX_KEY_LEN];
    int      cell_count;
    Cell     cells[MAX_CELLS];
} Row;

typedef struct {
    char     name[MAX_KEY_LEN];
    int      row_count;
    Row      rows[MAX_ROWS];            /* 按 row_key 排序 (SSTable 概念) */
} Table;

static Table  tables[MAX_TABLES];
static int    table_count = 0;

/* ── 工具函數 ──────────────────────────────── */

static int64_t now_us(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
}

/* 比較函數，用於 qsort / bsearch */
static int cmp_row(const void *a, const void *b) {
    return strcmp(((Row*)a)->row_key, ((Row*)b)->row_key);
}

static Table *find_table(const char *name) {
    for (int i = 0; i < table_count; i++)
        if (strcmp(tables[i].name, name) == 0)
            return &tables[i];
    return NULL;
}

/* 二分搜尋找 row；若不存在回傳 NULL */
static Row *find_row(Table *t, const char *row_key) {
    int lo = 0, hi = t->row_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        int cmp = strcmp(t->rows[mid].row_key, row_key);
        if (cmp == 0) return &t->rows[mid];
        if (cmp < 0)  lo = mid + 1;
        else          hi = mid - 1;
    }
    return NULL;
}

/* 找或建立 row（維持排序） */
static Row *get_or_create_row(Table *t, const char *row_key) {
    Row *r = find_row(t, row_key);
    if (r) return r;
    if (t->row_count >= MAX_ROWS) { fprintf(stderr, "[ERR] row limit\n"); return NULL; }
    Row *nr = &t->rows[t->row_count++];
    memset(nr, 0, sizeof(*nr));
    strncpy(nr->row_key, row_key, MAX_KEY_LEN-1);
    qsort(t->rows, t->row_count, sizeof(Row), cmp_row);
    return find_row(t, row_key);   /* 排序後重新找指標 */
}

/* 解析 "family:qualifier" */
static int parse_col(const char *col, char *family, char *qual) {
    const char *colon = strchr(col, ':');
    if (!colon) { fprintf(stderr, "[ERR] 欄位格式應為 family:qualifier\n"); return 0; }
    size_t flen = colon - col;
    strncpy(family, col, flen); family[flen] = '\0';
    strncpy(qual, colon+1, MAX_KEY_LEN-1);
    return 1;
}

/* 在 row 中找 cell */
static Cell *find_cell(Row *r, const char *family, const char *qual) {
    for (int i = 0; i < r->cell_count; i++)
        if (strcmp(r->cells[i].col_family, family) == 0 &&
            strcmp(r->cells[i].col_qual,   qual)   == 0)
            return &r->cells[i];
    return NULL;
}

/* ── 指令實作 ─────────────────────────────── */

static void cmd_createtable(const char *name) {
    if (find_table(name)) { printf("[WARN] 資料表 '%s' 已存在\n", name); return; }
    if (table_count >= MAX_TABLES) { fprintf(stderr, "[ERR] table limit\n"); return; }
    Table *t = &tables[table_count++];
    memset(t, 0, sizeof(*t));
    strncpy(t->name, name, MAX_KEY_LEN-1);
    printf("[OK] 建立資料表 '%s'\n", name);
}

static void cmd_droptable(const char *name) {
    for (int i = 0; i < table_count; i++) {
        if (strcmp(tables[i].name, name) == 0) {
            tables[i] = tables[--table_count];
            printf("[OK] 刪除資料表 '%s'\n", name);
            return;
        }
    }
    fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", name);
}

static void cmd_listtables(void) {
    if (table_count == 0) { printf("(無資料表)\n"); return; }
    for (int i = 0; i < table_count; i++)
        printf("  %s  (rows: %d)\n", tables[i].name, tables[i].row_count);
}

static void cmd_put(const char *tname, const char *row_key,
                    const char *col, const char *value) {
    Table *t = find_table(tname);
    if (!t) { fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", tname); return; }

    char family[MAX_KEY_LEN], qual[MAX_KEY_LEN];
    if (!parse_col(col, family, qual)) return;

    Row *r = get_or_create_row(t, row_key);
    if (!r) return;

    Cell *c = find_cell(r, family, qual);
    if (!c) {
        if (r->cell_count >= MAX_CELLS) { fprintf(stderr, "[ERR] cell limit\n"); return; }
        c = &r->cells[r->cell_count++];
        memset(c, 0, sizeof(*c));
        strncpy(c->col_family, family, MAX_KEY_LEN-1);
        strncpy(c->col_qual,   qual,   MAX_KEY_LEN-1);
    }

    /* 插入新版本（往後移，保持 index 0 最新） */
    if (c->ver_count < MAX_VERSIONS) c->ver_count++;
    memmove(&c->versions[1], &c->versions[0], (c->ver_count-1)*sizeof(Version));
    c->versions[0].timestamp = now_us();
    strncpy(c->versions[0].value, value, MAX_VAL_LEN-1);

    printf("[OK] PUT %s/%s/%s = \"%s\"  (ts=%lld)\n",
           tname, row_key, col, value, (long long)c->versions[0].timestamp);
}

static void cmd_get(const char *tname, const char *row_key,
                    const char *col, int versions) {
    Table *t = find_table(tname);
    if (!t) { fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", tname); return; }

    Row *r = find_row(t, row_key);
    if (!r) { printf("(無此 row: %s)\n", row_key); return; }

    char family[MAX_KEY_LEN], qual[MAX_KEY_LEN];
    if (!parse_col(col, family, qual)) return;

    Cell *c = find_cell(r, family, qual);
    if (!c || c->ver_count == 0) { printf("(無此欄位: %s)\n", col); return; }

    int show = (versions < 1 || versions > c->ver_count) ? 1 : versions;
    for (int i = 0; i < show; i++)
        printf("  [v%d] ts=%-20lld  value=\"%s\"\n",
               i, (long long)c->versions[i].timestamp, c->versions[i].value);
}

static void cmd_scan(const char *tname, const char *prefix) {
    Table *t = find_table(tname);
    if (!t) { fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", tname); return; }

    size_t plen = strlen(prefix);
    int found = 0;
    for (int ri = 0; ri < t->row_count; ri++) {
        Row *r = &t->rows[ri];
        if (plen > 0 && strncmp(r->row_key, prefix, plen) != 0) continue;
        printf("  ROW: %s\n", r->row_key);
        for (int ci = 0; ci < r->cell_count; ci++) {
            Cell *c = &r->cells[ci];
            if (c->ver_count == 0) continue;
            printf("    %s:%s = \"%s\"  (ts=%lld)\n",
                   c->col_family, c->col_qual,
                   c->versions[0].value, (long long)c->versions[0].timestamp);
        }
        found++;
    }
    if (!found) printf("(無符合 prefix='%s' 的 row)\n", prefix);
}

static void cmd_del(const char *tname, const char *row_key, const char *col) {
    Table *t = find_table(tname);
    if (!t) { fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", tname); return; }

    Row *r = find_row(t, row_key);
    if (!r) { fprintf(stderr, "[ERR] 找不到 row '%s'\n", row_key); return; }

    char family[MAX_KEY_LEN], qual[MAX_KEY_LEN];
    if (!parse_col(col, family, qual)) return;

    for (int i = 0; i < r->cell_count; i++) {
        if (strcmp(r->cells[i].col_family, family) == 0 &&
            strcmp(r->cells[i].col_qual,   qual)   == 0) {
            r->cells[i] = r->cells[--r->cell_count];
            printf("[OK] DEL %s/%s/%s\n", tname, row_key, col);
            return;
        }
    }
    fprintf(stderr, "[ERR] 找不到欄位 '%s'\n", col);
}

static void cmd_dump(const char *tname) {
    Table *t = find_table(tname);
    if (!t) { fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", tname); return; }

    printf("=== TABLE: %s  (%d rows) ===\n", t->name, t->row_count);
    for (int ri = 0; ri < t->row_count; ri++) {
        Row *r = &t->rows[ri];
        printf("  ROW: %s\n", r->row_key);
        for (int ci = 0; ci < r->cell_count; ci++) {
            Cell *c = &r->cells[ci];
            printf("    %s:%s  (%d versions)\n",
                   c->col_family, c->col_qual, c->ver_count);
            for (int vi = 0; vi < c->ver_count; vi++)
                printf("      [v%d] ts=%-20lld  \"%s\"\n",
                       vi, (long long)c->versions[vi].timestamp,
                       c->versions[vi].value);
        }
    }
}

/* ── 持久化（簡單文字格式） ─────────────────── */

static void cmd_save(const char *tname) {
    Table *t = find_table(tname);
    if (!t) { fprintf(stderr, "[ERR] 找不到資料表 '%s'\n", tname); return; }

    char path[256];
    snprintf(path, sizeof(path), "%s.db", tname);
    FILE *f = fopen(path, "w");
    if (!f) { perror("fopen"); return; }

    fprintf(f, "TABLE %s\n", t->name);
    for (int ri = 0; ri < t->row_count; ri++) {
        Row *r = &t->rows[ri];
        for (int ci = 0; ci < r->cell_count; ci++) {
            Cell *c = &r->cells[ci];
            for (int vi = c->ver_count-1; vi >= 0; vi--) {
                fprintf(f, "CELL %s %s %s %lld %s\n",
                        r->row_key, c->col_family, c->col_qual,
                        (long long)c->versions[vi].timestamp,
                        c->versions[vi].value);
            }
        }
    }
    fclose(f);
    printf("[OK] 儲存至 %s\n", path);
}

static void cmd_load(const char *tname) {
    char path[256];
    snprintf(path, sizeof(path), "%s.db", tname);
    FILE *f = fopen(path, "r");
    if (!f) { perror("fopen"); return; }

    char line[1024];
    /* 第一行：TABLE name */
    if (!fgets(line, sizeof(line), f)) { fclose(f); return; }
    char loaded_name[MAX_KEY_LEN];
    sscanf(line, "TABLE %s", loaded_name);

    if (!find_table(loaded_name)) cmd_createtable(loaded_name);
    Table *t = find_table(loaded_name);

    while (fgets(line, sizeof(line), f)) {
        char rk[MAX_KEY_LEN], fam[MAX_KEY_LEN], ql[MAX_KEY_LEN], val[MAX_VAL_LEN];
        long long ts;
        /* CELL row_key family qualifier timestamp value */
        if (sscanf(line, "CELL %127s %127s %127s %lld %511[^\n]",
                   rk, fam, ql, &ts, val) != 5) continue;

        Row *r = get_or_create_row(t, rk);
        if (!r) continue;
        Cell *c = find_cell(r, fam, ql);
        if (!c) {
            if (r->cell_count >= MAX_CELLS) continue;
            c = &r->cells[r->cell_count++];
            memset(c, 0, sizeof(*c));
            strncpy(c->col_family, fam, MAX_KEY_LEN-1);
            strncpy(c->col_qual,   ql,  MAX_KEY_LEN-1);
        }
        if (c->ver_count < MAX_VERSIONS) {
            Version *v = &c->versions[c->ver_count++];
            v->timestamp = (int64_t)ts;
            strncpy(v->value, val, MAX_VAL_LEN-1);
        }
    }
    fclose(f);
    printf("[OK] 從 %s 載入資料表 '%s'\n", path, loaded_name);
}

/* ── REPL 主迴圈 ──────────────────────────── */

static int tokenize(char *line, char *tok[], int max) {
    int n = 0;
    char *p = line;
    while (*p && n < max) {
        while (*p == ' ' || *p == '\t') p++;
        if (!*p || *p == '\n') break;
        if (*p == '"') {                   /* 帶引號的值 */
            p++;
            tok[n++] = p;
            while (*p && *p != '"') p++;
            if (*p == '"') *p++ = '\0';
        } else {
            tok[n++] = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\n') p++;
            if (*p) *p++ = '\0';
        }
    }
    return n;
}

int main(void) {
    char line[1024];
    printf("nosql0 — 簡化版 BigTable (輸入 QUIT 離開, help 查看指令)\n");
    printf("> ");
    fflush(stdout);

    while (fgets(line, sizeof(line), stdin)) {
        char *tok[MAX_TOKENS];
        int n = tokenize(line, tok, MAX_TOKENS);
        if (n == 0) { printf("> "); fflush(stdout); continue; }

        char *cmd = tok[0];

        if (strcasecmp(cmd, "quit") == 0 || strcasecmp(cmd, "exit") == 0) {
            printf("bye.\n"); break;

        } else if (strcasecmp(cmd, "help") == 0) {
            printf(
                "指令列表：\n"
                "  CREATETABLE <table>\n"
                "  DROPTABLE   <table>\n"
                "  LISTTABLES\n"
                "  PUT  <table> <row_key> <family:qual> <value>\n"
                "  GET  <table> <row_key> <family:qual> [versions]\n"
                "  SCAN <table> <row_key_prefix>\n"
                "  DEL  <table> <row_key> <family:qual>\n"
                "  DUMP <table>\n"
                "  SAVE <table>\n"
                "  LOAD <table>\n"
                "  QUIT\n"
            );

        } else if (strcasecmp(cmd, "createtable") == 0 && n >= 2) {
            cmd_createtable(tok[1]);

        } else if (strcasecmp(cmd, "droptable") == 0 && n >= 2) {
            cmd_droptable(tok[1]);

        } else if (strcasecmp(cmd, "listtables") == 0) {
            cmd_listtables();

        } else if (strcasecmp(cmd, "put") == 0 && n >= 5) {
            cmd_put(tok[1], tok[2], tok[3], tok[4]);

        } else if (strcasecmp(cmd, "get") == 0 && n >= 4) {
            int ver = (n >= 5) ? atoi(tok[4]) : 1;
            cmd_get(tok[1], tok[2], tok[3], ver);

        } else if (strcasecmp(cmd, "scan") == 0 && n >= 3) {
            cmd_scan(tok[1], tok[2]);

        } else if (strcasecmp(cmd, "del") == 0 && n >= 4) {
            cmd_del(tok[1], tok[2], tok[3]);

        } else if (strcasecmp(cmd, "dump") == 0 && n >= 2) {
            cmd_dump(tok[1]);

        } else if (strcasecmp(cmd, "save") == 0 && n >= 2) {
            cmd_save(tok[1]);

        } else if (strcasecmp(cmd, "load") == 0 && n >= 2) {
            cmd_load(tok[1]);

        } else {
            fprintf(stderr, "[ERR] 未知指令或參數不足: %s\n", cmd);
        }

        printf("> ");
        fflush(stdout);
    }
    return 0;
}
