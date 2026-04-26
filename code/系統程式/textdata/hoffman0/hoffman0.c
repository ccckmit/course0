/*
 * huffman.c - Huffman Encoding / Decoding 示範程式
 *
 * 功能：
 *   1. 統計輸入字串各字元的出現頻率
 *   2. 建構 Huffman Tree（最小堆積優先佇列）
 *   3. 產生每個字元的 Huffman 編碼表
 *   4. 對字串進行編碼（輸出 0/1 位元字串）
 *   5. 對編碼結果進行解碼，還原原始字串
 *   6. 顯示壓縮效益統計
 *
 * 編譯：gcc -o huffman huffman.c
 * 執行：./huffman
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CHAR   256   /* ASCII 字元範圍 */
#define MAX_CODE   256   /* 每個字元最長編碼位元數 */

/* ── Huffman Tree 節點 ── */
typedef struct Node {
    unsigned char ch;    /* 字元（只有葉節點有意義） */
    int           freq;  /* 出現頻率 */
    struct Node  *left;
    struct Node  *right;
} Node;

/* ── 最小堆積（Min-Heap）── */
typedef struct {
    Node **data;
    int    size;
    int    capacity;
} MinHeap;

/* ── 編碼表條目 ── */
typedef struct {
    char bits[MAX_CODE]; /* 位元字串，e.g. "1010" */
    int  len;            /* 位元長度 */
} CodeEntry;

/* ======================================================
 *  最小堆積操作
 * ====================================================== */
MinHeap *heap_create(int cap)
{
    MinHeap *h   = malloc(sizeof(MinHeap));
    h->data      = malloc(cap * sizeof(Node *));
    h->size      = 0;
    h->capacity  = cap;
    return h;
}

void heap_swap(MinHeap *h, int i, int j)
{
    Node *tmp  = h->data[i];
    h->data[i] = h->data[j];
    h->data[j] = tmp;
}

void heap_push(MinHeap *h, Node *node)
{
    int i = h->size++;
    h->data[i] = node;
    /* 上浮 */
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (h->data[parent]->freq > h->data[i]->freq) {
            heap_swap(h, parent, i);
            i = parent;
        } else break;
    }
}

Node *heap_pop(MinHeap *h)
{
    Node *top    = h->data[0];
    h->data[0]   = h->data[--h->size];
    /* 下沉 */
    int i = 0;
    while (1) {
        int l = 2*i+1, r = 2*i+2, smallest = i;
        if (l < h->size && h->data[l]->freq < h->data[smallest]->freq)
            smallest = l;
        if (r < h->size && h->data[r]->freq < h->data[smallest]->freq)
            smallest = r;
        if (smallest == i) break;
        heap_swap(h, i, smallest);
        i = smallest;
    }
    return top;
}

/* ======================================================
 *  建立 Huffman Tree
 * ====================================================== */
Node *node_new(unsigned char ch, int freq, Node *l, Node *r)
{
    Node *n = malloc(sizeof(Node));
    n->ch    = ch;
    n->freq  = freq;
    n->left  = l;
    n->right = r;
    return n;
}

Node *build_tree(int freq[])
{
    MinHeap *h = heap_create(MAX_CHAR);

    for (int i = 0; i < MAX_CHAR; i++)
        if (freq[i] > 0)
            heap_push(h, node_new((unsigned char)i, freq[i], NULL, NULL));

    /* 只有一種字元的邊界情況 */
    if (h->size == 1) {
        Node *only = heap_pop(h);
        Node *root = node_new(0, only->freq, only, NULL);
        free(h->data); free(h);
        return root;
    }

    while (h->size > 1) {
        Node *a = heap_pop(h);
        Node *b = heap_pop(h);
        heap_push(h, node_new(0, a->freq + b->freq, a, b));
    }

    Node *root = heap_pop(h);
    free(h->data);
    free(h);
    return root;
}

/* ======================================================
 *  遞迴產生編碼表
 * ====================================================== */
void generate_codes(Node *node, char *buf, int depth, CodeEntry table[])
{
    if (!node) return;

    if (!node->left && !node->right) {
        /* 葉節點 */
        buf[depth] = '\0';
        strcpy(table[node->ch].bits, depth == 0 ? "0" : buf);
        table[node->ch].len = depth == 0 ? 1 : depth;
        return;
    }
    buf[depth] = '0';
    generate_codes(node->left,  buf, depth+1, table);
    buf[depth] = '1';
    generate_codes(node->right, buf, depth+1, table);
}

/* ======================================================
 *  釋放樹記憶體
 * ====================================================== */
void tree_free(Node *node)
{
    if (!node) return;
    tree_free(node->left);
    tree_free(node->right);
    free(node);
}

/* ======================================================
 *  編碼
 * ====================================================== */
char *encode(const char *text, CodeEntry table[])
{
    /* 先計算所需長度 */
    size_t total = 0;
    for (const char *p = text; *p; p++)
        total += table[(unsigned char)*p].len;

    char *out = malloc(total + 1);
    out[0] = '\0';
    for (const char *p = text; *p; p++)
        strcat(out, table[(unsigned char)*p].bits);
    return out;
}

/* ======================================================
 *  解碼
 * ====================================================== */
char *decode(const char *bits, Node *root, int text_len)
{
    char *out = malloc(text_len + 1);
    int   pos = 0;
    Node *cur = root;

    for (const char *p = bits; *p && pos < text_len; p++) {
        cur = (*p == '0') ? cur->left : cur->right;
        if (!cur->left && !cur->right) {   /* 葉節點 */
            out[pos++] = (char)cur->ch;
            cur = root;
        }
    }
    out[pos] = '\0';
    return out;
}

/* ======================================================
 *  印出編碼表
 * ====================================================== */
void print_table(CodeEntry table[], int freq[])
{
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║          Huffman 編碼表                  ║\n");
    printf("╠═══════╦════════╦═══════════╦════════════╣\n");
    printf("║ 字元  ║ 頻率   ║ 位元數    ║ 編碼       ║\n");
    printf("╠═══════╬════════╬═══════════╬════════════╣\n");

    for (int i = 0; i < MAX_CHAR; i++) {
        if (freq[i] == 0) continue;
        char display[8];
        if (i == ' ')       sprintf(display, "SPACE");
        else if (i == '\n') sprintf(display, "\\n   ");
        else                sprintf(display, "  %c  ", i);
        printf("║ %-5s ║ %-6d ║ %-9d ║ %-10s ║\n",
               display, freq[i], table[i].len, table[i].bits);
    }
    printf("╚═══════╩════════╩═══════════╩════════════╝\n");
}

/* ======================================================
 *  主程式
 * ====================================================== */
int main(void)
{
    /* 測試字串 */
    const char *texts[] = {
        "hello huffman",
        "aaabbc",
        "this is an example of huffman encoding",
        "abc abcd abc abcd abc abcd abc abcd abc abcd abc abcd",
        NULL
    };

    for (int t = 0; texts[t] != NULL; t++) {
        const char *text = texts[t];
        int text_len = (int)strlen(text);

        printf("\n══════════════════════════════════════════════════\n");
        printf("  原始字串：\"%s\"\n", text);
        printf("  原始長度：%d 字元  (%d bits, 以 8-bit ASCII 計)\n",
               text_len, text_len * 8);

        /* 1. 頻率統計 */
        int freq[MAX_CHAR] = {0};
        for (const char *p = text; *p; p++)
            freq[(unsigned char)*p]++;

        /* 2. 建構 Huffman Tree */
        Node *root = build_tree(freq);

        /* 3. 產生編碼表 */
        CodeEntry table[MAX_CHAR];
        memset(table, 0, sizeof(table));
        char buf[MAX_CODE];
        generate_codes(root, buf, 0, table);

        /* 4. 印出編碼表 */
        print_table(table, freq);

        /* 5. 編碼 */
        char *encoded = encode(text, table);
        int enc_len   = (int)strlen(encoded);
        printf("\n編碼結果（%d bits）：\n", enc_len);

        /* 每 64 位元換行，增加可讀性 */
        for (int i = 0; encoded[i]; i++) {
            putchar(encoded[i]);
            if ((i+1) % 64 == 0) putchar('\n');
        }
        putchar('\n');

        /* 6. 解碼 */
        char *decoded = decode(encoded, root, text_len);
        printf("\n解碼結果：\"%s\"\n", decoded);

        /* 7. 驗證 */
        int ok = (strcmp(text, decoded) == 0);
        printf("驗證：%s\n", ok ? "✓ 編解碼結果一致" : "✗ 錯誤！");

        /* 8. 壓縮統計 */
        double ratio = 100.0 * (1.0 - (double)enc_len / (text_len * 8));
        printf("\n壓縮統計：\n");
        printf("  原始大小：%5d bits\n", text_len * 8);
        printf("  壓縮後  ：%5d bits\n", enc_len);
        printf("  壓縮率  ：%.1f%%\n", ratio);

        free(encoded);
        free(decoded);
        tree_free(root);
    }

    printf("\n══════════════════════════════════════════════════\n");
    return 0;
}