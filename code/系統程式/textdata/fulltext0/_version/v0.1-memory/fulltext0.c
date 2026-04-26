#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 2000
#define MAX_LINE_LEN 2048
#define MAX_WORD_LEN 256
#define MAX_INDEX_ENTRIES 50000
#define HASH_SIZE 10007

typedef struct Node {
    int doc_id;
    struct Node* next;
} Node;

typedef struct {
    char term[MAX_WORD_LEN];
    Node* doc_list;
} InvertedIndex;

static InvertedIndex* inverted_index = NULL;
static int index_size = 0;
static char corpus[MAX_LINES][MAX_LINE_LEN];
static int total_docs = 0;

int is_cjk(int c) {
    return (c >= 0x4E00 && c <= 0x9FFF) ||
           (c >= 0x3000 && c <= 0x303F) ||
           (c >= 0xFF00 && c <= 0xFFEF);
}

int utf8_char_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

int decode_utf8(const char* p, int* out_len) {
    int len = utf8_char_len((unsigned char)*p);
    *out_len = len;
    if (len == 1) return (unsigned char)*p;
    int codepoint = 0;
    if (len == 2) {
        codepoint = (*p & 0x1F) << 6;
        codepoint |= (*(p + 1) & 0x3F);
    } else if (len == 3) {
        codepoint = (*p & 0x0F) << 12;
        codepoint |= (*(p + 1) & 0x3F) << 6;
        codepoint |= (*(p + 2) & 0x3F);
    } else if (len == 4) {
        codepoint = (*p & 0x07) << 18;
        codepoint |= (*(p + 1) & 0x3F) << 12;
        codepoint |= (*(p + 2) & 0x3F) << 6;
        codepoint |= (*(p + 3) & 0x3F);
    }
    return codepoint;
}

int is_alpha(int c) {
    return isalpha((unsigned char)c);
}

void extract_terms(const char* text, char terms[][MAX_WORD_LEN], int* term_count) {
    *term_count = 0;
    const char* p = text;

    while (*p) {
        int char_len;
        int cp = decode_utf8(p, &char_len);

        if (is_cjk(cp)) {
            char two[4] = {0}, one[4] = {0};
            memcpy(two, p, char_len > 2 ? char_len : 2);
            memcpy(one, p, char_len);

            strcpy(terms[*term_count], two);
            (*term_count)++;
            strcpy(terms[*term_count], one);
            (*term_count)++;
            p += char_len;
        } else if (is_alpha((unsigned char)*p)) {
            char word[MAX_WORD_LEN] = {0};
            int i = 0;
            while (*p && is_alpha((unsigned char)*p)) {
                word[i++] = tolower((unsigned char)*p);
                p++;
            }
            if (i > 0) {
                strcpy(terms[*term_count], word);
                (*term_count)++;
            }
        } else {
            p++;
        }
    }
}

int find_or_create_index(const char* term) {
    for (int i = 0; i < index_size; i++) {
        if (strcmp(inverted_index[i].term, term) == 0) return i;
    }
    if (index_size >= MAX_INDEX_ENTRIES) return -1;
    strcpy(inverted_index[index_size].term, term);
    inverted_index[index_size].doc_list = NULL;
    return index_size++;
}

void add_to_index(const char* term, int doc_id) {
    int idx = find_or_create_index(term);
    if (idx < 0) return;

    Node* head = inverted_index[idx].doc_list;
    Node* prev = NULL;
    while (head) {
        if (head->doc_id == doc_id) return;
        prev = head;
        head = head->next;
    }

    Node* node = (Node*)malloc(sizeof(Node));
    node->doc_id = doc_id;
    node->next = NULL;
    if (prev) prev->next = node;
    else inverted_index[idx].doc_list = node;
}

void build_index() {
    inverted_index = (InvertedIndex*)calloc(HASH_SIZE, sizeof(InvertedIndex));
    if (!inverted_index) {
        fprintf(stderr, "Memory error\n");
        return;
    }

    FILE* fp = fopen("_data/corpus.txt", "r");
    if (!fp) {
        fprintf(stderr, "Cannot open corpus.txt\n");
        free(inverted_index);
        return;
    }

    char line[MAX_LINE_LEN];
    total_docs = 0;

    while (fgets(line, sizeof(line), fp) && total_docs < MAX_LINES) {
        line[strcspn(line, "\n")] = 0;
        if (strlen(line) == 0) continue;

        char* dot = strchr(line, '.');
        if (dot) {
            strcpy(corpus[total_docs], dot + 1);
        } else {
            strcpy(corpus[total_docs], line);
        }

        char terms[MAX_WORD_LEN][MAX_WORD_LEN];
        int term_count = 0;
        extract_terms(corpus[total_docs], terms, &term_count);

        for (int i = 0; i < term_count; i++) {
            add_to_index(terms[i], total_docs);
        }

        total_docs++;
    }
    fclose(fp);
}

int* search_index(const char* query, int* result_count) {
    char terms[MAX_WORD_LEN][MAX_WORD_LEN];
    int term_count = 0;
    extract_terms(query, terms, &term_count);

    if (term_count == 0) {
        *result_count = 0;
        return NULL;
    }

    int** doc_sets = (int**)malloc(sizeof(int*) * term_count);
    int* doc_counts = (int*)malloc(sizeof(int) * term_count);

    for (int i = 0; i < term_count; i++) {
        doc_sets[i] = (int*)malloc(sizeof(int) * total_docs);
        doc_counts[i] = 0;
    }

    for (int i = 0; i < term_count; i++) {
        for (int j = 0; j < index_size; j++) {
            if (strcmp(inverted_index[j].term, terms[i]) == 0) {
                Node* node = inverted_index[j].doc_list;
                while (node) {
                    doc_sets[i][doc_counts[i]++] = node->doc_id;
                    node = node->next;
                }
                break;
            }
        }
    }

    int* intersect = (int*)malloc(sizeof(int) * total_docs);
    int intersect_count = 0;

    for (int i = 0; i < doc_counts[0]; i++) {
        int doc_id = doc_sets[0][i];
        int match = 1;
        for (int j = 1; j < term_count; j++) {
            int found = 0;
            for (int k = 0; k < doc_counts[j]; k++) {
                if (doc_sets[j][k] == doc_id) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                match = 0;
                break;
            }
        }
        if (match) {
            intersect[intersect_count++] = doc_id;
        }
    }

    for (int i = 0; i < term_count; i++) {
        free(doc_sets[i]);
    }
    free(doc_sets);
    free(doc_counts);

    *result_count = intersect_count;
    return intersect;
}

int contains_keyword(const char* text, const char* keyword) {
    char terms[MAX_WORD_LEN][MAX_WORD_LEN];
    int term_count = 0;
    extract_terms(keyword, terms, &term_count);

    if (term_count == 0) return 0;

    for (int i = 0; i < term_count; i++) {
        const char* p = text;
        int found = 0;
        while (*p) {
            int char_len;
            int cp = decode_utf8(p, &char_len);

            if (is_cjk(cp)) {
                char two[4] = {0}, one[4] = {0};
                memcpy(two, p, char_len > 2 ? char_len : 2);
                memcpy(one, p, char_len);

                if (strcmp(terms[i], two) == 0 || strcmp(terms[i], one) == 0) {
                    found = 1;
                    break;
                }
                p += char_len;
            } else if (is_alpha((unsigned char)*p)) {
                char word[MAX_WORD_LEN] = {0};
                int j = 0;
                while (*p && is_alpha((unsigned char)*p)) {
                    word[j++] = tolower((unsigned char)*p);
                    p++;
                }
                if (strcmp(terms[i], word) == 0) {
                    found = 1;
                    break;
                }
            } else {
                p++;
            }
        }
        if (!found) return 0;
    }
    return 1;
}

int* search(const char* query, int* result_count) {
    int* candidates = search_index(query, result_count);
    if (!candidates || *result_count == 0) return candidates;

    int* filtered = (int*)malloc(sizeof(int) * (*result_count));
    int filtered_count = 0;

    for (int i = 0; i < *result_count; i++) {
        if (contains_keyword(corpus[candidates[i]], query)) {
            filtered[filtered_count++] = candidates[i];
        }
    }

    free(candidates);
    *result_count = filtered_count;
    return filtered;
}

void free_index() {
    if (!inverted_index) return;
    for (int i = 0; i < index_size; i++) {
        Node* node = inverted_index[i].doc_list;
        while (node) {
            Node* next = node->next;
            free(node);
            node = next;
        }
    }
    free(inverted_index);
    inverted_index = NULL;
}

int main(int argc, char* argv[]) {
    build_index();
    printf("Index built: %d terms, %d docs\n", index_size, total_docs);

    if (argc > 1) {
        int count = 0;
        int* results = search(argv[1], &count);
        printf("Query '%s': %d results\n", argv[1], count);
        for (int i = 0; i < count && i < 10; i++) {
            printf(" [%d] %s\n", results[i] + 1, corpus[results[i]]);
        }
        if (count > 10) printf(" ... and %d more\n", count - 10);
        free(results);
    }

    free_index();
    return 0;
}