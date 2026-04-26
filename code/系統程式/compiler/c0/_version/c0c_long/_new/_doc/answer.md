


這裡為您提供擴充後的完整編譯器原始碼。為了讓編譯器能夠編譯自身（Self-hosting），我增加了以下支援與修復：

1. **`ast.h` 結構定義**：補充了遺漏的 `ast.h`。
2. **前置處理器 (`macro.c`) 強化**：
   - 支援 `#include "..."` 遞迴展開本地標頭檔，並防止重複引入（Include guard 行為）。
   - 支援 `#include <...>` 時，自動替換為內建的 `libc` 函式定義（如 `printf`, `malloc`, `FILE*` 等）。
   - 內建展開 `#define NULL 0`, `SEEK_SET` 等巨集。
3. **Lexer / Parser 擴充**：
   - 支援 `extern` 宣告（使全域變數/函式能在多個檔案中共用）。
   - 支援 `enum` 列舉常數解析（如 `TokenType`, `ValueType` 等）。
   - 支援 `...` (Variadic arguments)，讓 `printf`, `sprintf` 等變數參數函式能被正確呼叫。
   - 支援雙重指標 `**` 的靈活轉換（內部將其對齊為 `i64*` 來正確處理 8-byte 指標運算）。
   - 支援 `sizeof(型別)` 更廣泛的判斷。
4. **Codegen 增強**：
   - 修正指標的 `p += 1`、`p++` 計算，正確對應 LLVM 的 `getelementptr`，而非簡單整數相加。
   - 支援 LLVM opaque pointer 強制轉型 (`inttoptr` / `ptrtoint`)，放寬指標型別比對，增強相容性。

請將以下檔案儲存覆蓋：

--- START OF FILE ast.h ---

#ifndef C0C_AST_H
#define C0C_AST_H

typedef enum {
    TY_INT, TY_CHAR, TY_VOID, TY_FLOAT, TY_DOUBLE, 
    TY_UCHAR, TY_SHORT, TY_USHORT, TY_UINT, TY_LONG, TY_ULONG,
    TY_INT_PTR, TY_CHAR_PTR, TY_VOID_PTR, TY_FLOAT_PTR, TY_DOUBLE_PTR,
    TY_UCHAR_PTR, TY_SHORT_PTR, TY_USHORT_PTR, TY_UINT_PTR, TY_LONG_PTR, TY_ULONG_PTR,
    TY_STRUCT, TY_STRUCT_PTR
} CType;

typedef enum {
    AST_NUM, AST_FLOAT, AST_STR, AST_VAR, AST_CALL, AST_ADDR, AST_DEREF,
    AST_INDEX, AST_MEMBER, AST_BINOP, AST_UNARY, AST_CAST, AST_SIZEOF,
    AST_INCDEC, AST_DECL, AST_ASSIGN, AST_EXPR_STMT, AST_BLOCK, AST_IF,
    AST_WHILE, AST_FOR, AST_DO, AST_SWITCH, AST_CASE, AST_BREAK,
    AST_CONTINUE, AST_RETURN, AST_FUNC, AST_GLOBAL
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    struct ASTNode *next;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *cond;
    struct ASTNode *then_body;
    struct ASTNode *else_body;
    struct ASTNode *body;
    struct ASTNode *init;
    struct ASTNode *update;
    
    int op;
    int val; // 也在函數中用來標記 is_vararg
    double fval;
    char name[64];
    char str_val[256];
    
    CType ty;
    int array_len;
    int struct_id;
    int init_kind;
    int is_default;
    int is_prefix;
    int is_decl; // 也用來標記 extern
} ASTNode;

typedef struct {
    char name[64];
    CType ty;
    int offset;
    int struct_id;
} StructField;

typedef struct {
    char name[64];
    int size;
    StructField fields[64];
    int field_cnt;
} StructDef;

extern StructDef g_struct_defs[64];
extern int g_struct_def_cnt;

ASTNode* new_node(ASTNodeType type);

#endif

--- START OF FILE ast.c ---

#include "ast.h"
#include <stdlib.h>

StructDef g_struct_defs[64];
int g_struct_def_cnt = 0;

ASTNode* new_node(ASTNodeType type) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = type;
    return n;
}

--- START OF FILE macro.h ---

#ifndef C0C_MACRO_H
#define C0C_MACRO_H

#include <stddef.h>

void macro_init(void);
void macro_free(void);

int macro_parse_line(char *line, int line_num);
char *macro_expand(const char *input);
char *macro_expand_file(const char *filename);

void macro_define(const char *name, const char *replacement);
void macro_undef(const char *name);
int macro_defined(const char *name);

void macro_enable_expand(void);
void macro_disable_expand(void);

#endif

--- START OF FILE macro.c ---

#include "macro.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MACROS 256
#define MAX_OUTPUT_SIZE (1 << 20)

typedef struct {
    char name[64];
    char replacement[512];
    int is_function;
    int arg_count;
} Macro;

static Macro macros[MAX_MACROS];
static int macro_count = 0;
static char included_files[128][64];
static int included_count = 0;

static void add_macro(const char *name, const char *replacement) {
    Macro *m = NULL;
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0) { m = &macros[i]; break; }
    }
    if (!m && macro_count < MAX_MACROS) {
        m = &macros[macro_count++];
        strncpy(m->name, name, sizeof(m->name) - 1);
        m->name[sizeof(m->name) - 1] = '\0';
    }
    if (m) {
        strncpy(m->replacement, replacement, sizeof(m->replacement) - 1);
        m->replacement[sizeof(m->replacement) - 1] = '\0';
        m->is_function = 0;
        m->arg_count = 0;
    }
}

void macro_init(void) {
    macro_count = 0;
    included_count = 0;
    add_macro("NULL", "0");
    add_macro("SEEK_SET", "0");
    add_macro("SEEK_END", "2");
    add_macro("SEEK_CUR", "1");
}

void macro_free(void) {
    macro_count = 0;
}

static Macro *find_macro(const char *name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0) return &macros[i];
    }
    return NULL;
}

void macro_define(const char *name, const char *replacement) {
    add_macro(name, replacement);
}

void macro_undef(const char *name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0) {
            for (int j = i; j < macro_count - 1; j++) {
                macros[j] = macros[j + 1];
            }
            macro_count--;
            return;
        }
    }
}

int macro_defined(const char *name) {
    return find_macro(name) != NULL;
}

void macro_enable_expand(void) {}
void macro_disable_expand(void) {}

static void process_define(char *line) {
    char *p = line + 7;
    while (*p == ' ' || *p == '\t') p++;
    
    char name[64];
    int i = 0;
    while (*p && (isalnum(*p) || *p == '_')) {
        if (i < 63) name[i++] = *p;
        p++;
    }
    name[i] = '\0';
    
    Macro *m = find_macro(name);
    if (!m && macro_count < MAX_MACROS) {
        m = &macros[macro_count++];
        strncpy(m->name, name, sizeof(m->name) - 1);
        m->name[sizeof(m->name) - 1] = '\0';
        m->is_function = 0;
        m->arg_count = 0;
    }
    
    if (m) {
        if (*p == '(') {
            m->is_function = 1;
            p++;
            char arg_names[10][32];
            int argc = 0;
            while (*p && *p != ')') {
                while (*p == ' ' || *p == '\t') p++;
                if (*p && *p != ',' && *p != ')') {
                    int j = 0;
                    while (*p && (isalnum(*p) || *p == '_')) {
                        if (j < 31) arg_names[argc][j++] = *p;
                        p++;
                    }
                    arg_names[argc][j] = '\0';
                    argc++;
                }
                if (*p == ',') p++;
                while (*p == ' ' || *p == '\t') p++;
            }
            m->arg_count = argc;
            if (*p == ')') p++;
            while (*p == ' ' || *p == '\t') p++;
            
            char *comment = strstr(p, "//");
            if (comment) *comment = '\0';
            
            char repl[512];
            strncpy(repl, p, sizeof(repl) - 1);
            repl[sizeof(repl) - 1] = '\0';
            
            for (int a = 0; a < argc; a++) {
                char from[32], to[32];
                sprintf(to, "$%d", a + 1);
                strncpy(from, arg_names[a], sizeof(from) - 1);
                from[sizeof(from) - 1] = '\0';
                
                char temp[512];
                temp[0] = '\0';
                char *tp = temp;
                char *rp = repl;
                while (*rp) {
                    if (strncmp(rp, from, strlen(from)) == 0 && 
                        (rp[strlen(from)] == ' ' || rp[strlen(from)] == '\t' || 
                         rp[strlen(from)] == ')' || rp[strlen(from)] == '(' ||
                         rp[strlen(from)] == '\0' || rp[strlen(from)] == '*' ||
                         rp[strlen(from)] == '/' || rp[strlen(from)] == '+' ||
                         rp[strlen(from)] == '-' || rp[strlen(from)] == '=')) {
                        strcpy(tp, to);
                        tp += strlen(to);
                        rp += strlen(from);
                    } else {
                        *tp++ = *rp++;
                    }
                }
                *tp = '\0';
                strncpy(repl, temp, sizeof(repl) - 1);
                repl[sizeof(repl) - 1] = '\0';
            }
            
            strncpy(m->replacement, repl, sizeof(m->replacement) - 1);
            m->replacement[sizeof(m->replacement) - 1] = '\0';
        } else {
            m->is_function = 0;
            m->arg_count = 0;
            
            char *comment = strstr(p, "//");
            if (comment) *comment = '\0';
            
            strncpy(m->replacement, p, sizeof(m->replacement) - 1);
            m->replacement[sizeof(m->replacement) - 1] = '\0';
        }
    }
}

static void process_undef(char *line) {
    char *p = line + 6;
    while (*p == ' ' || *p == '\t') p++;
    
    char name[64];
    int i = 0;
    while (*p && (isalnum(*p) || *p == '_')) {
        if (i < 63) name[i++] = *p;
        p++;
    }
    name[i] = '\0';
    macro_undef(name);
}

static void expand_function_macro(char *output, Macro *m, char *args[], int arg_count) {
    char buf[512];
    strcpy(buf, m->replacement);
    output[0] = '\0';
    char *p = buf;
    while (*p) {
        if (*p == '$' && *(p+1) >= '1' && *(p+1) <= '9') {
            int idx = *(p+1) - '1';
            if (idx < arg_count && idx < 10) strcat(output, args[idx]);
            p += 2;
        } else {
            char c[2] = {*p, '\0'};
            strcat(output, c);
            p++;
        }
    }
}

static void expand_line(char *output, const char *input) {
    strcpy(output, input);
    int changed = 1;
    while (changed) {
        changed = 0;
        char temp[MAX_OUTPUT_SIZE];
        temp[0] = '\0';
        char *p = output;
        char *out = temp;
        while (*p) {
            if (isalpha(*p) || *p == '_') {
                char name[64];
                int len = 0;
                char *start = p;
                while (*p && (isalnum(*p) || *p == '_')) {
                    name[len++] = *p;
                    p++;
                }
                name[len] = '\0';
                Macro *m = find_macro(name);
                if (m && m->is_function && *p == '(') {
                    p++;
                    char *args[10];
                    char arg_bufs[10][256];
                    int arg_count = 0;
                    int depth = 1;
                    int pos = 0;
                    args[0] = arg_bufs[0];
                    while (*p && depth > 0) {
                        if (*p == '(') depth++;
                        else if (*p == ')') depth--;
                        if (depth == 1 && *p == ',') {
                            arg_bufs[arg_count][pos] = '\0';
                            arg_count++;
                            args[arg_count] = arg_bufs[arg_count];
                            pos = 0;
                            p++;
                            continue;
                        }
                        if (depth > 0 && pos < 255) arg_bufs[arg_count][pos++] = *p;
                        p++;
                    }
                    arg_bufs[arg_count][pos] = '\0';
                    arg_count++;
                    char expanded[512];
                    expand_function_macro(expanded, m, args, arg_count);
                    strcpy(out, expanded);
                    out += strlen(expanded);
                    changed = 1;
                } else if (m && !m->is_function && strlen(m->replacement) > 0) {
                    strcpy(out, m->replacement);
                    out += strlen(m->replacement);
                    changed = 1;
                } else {
                    strncpy(out, start, len);
                    out[len] = '\0';
                    out += len;
                }
            } else *out++ = *p++;
            *out = '\0';
        }
        strcpy(output, temp);
    }
}

char *macro_expand(const char *input) {
    char *output = malloc(MAX_OUTPUT_SIZE);
    char *lines = malloc(strlen(input) + 1);
    strcpy(lines, input);
    
    char *result = malloc(MAX_OUTPUT_SIZE);
    result[0] = '\0';
    
    char *line = strtok(lines, "\n");
    while (line) {
        char *orig_line = line;
        while (*line == ' ' || *line == '\t') line++;
        if (strncmp(line, "#define ", 8) == 0) {
            process_define(line);
        } else if (strncmp(line, "#undef ", 7) == 0) {
            process_undef(line);
        } else if (strncmp(line, "#ifdef ", 7) == 0) {
        } else if (strncmp(line, "#ifndef ", 8) == 0) {
        } else if (strncmp(line, "#else", 5) == 0) {
        } else if (strncmp(line, "#endif", 6) == 0) {
        } else if (strncmp(line, "#include ", 9) == 0) {
            char *start = strchr(line, '"');
            if (start) {
                char *end = strchr(start + 1, '"');
                if (end) {
                    *end = '\0';
                    int already = 0;
                    for(int i=0; i<included_count; i++) {
                        if(strcmp(included_files[i], start + 1) == 0) { already = 1; break; }
                    }
                    if (!already) {
                        strcpy(included_files[included_count++], start + 1);
                        char *inc = macro_expand_file(start + 1);
                        if (inc) {
                            strcat(result, inc);
                            free(inc);
                        }
                    }
                }
            } else if (strchr(line, '<')) {
                // Mock libc
                if (strstr(line, "<stdio.h>")) strcat(result, "typedef void FILE;\nextern void *stdin;\nextern void *stdout;\nextern void *stderr;\nextern void *fopen(char *name, char *mode);\nextern int fclose(void *f);\nextern int fseek(void *f, long offset, int whence);\nextern long ftell(void *f);\nextern int fread(void *ptr, int size, int nmemb, void *f);\nextern int fputs(char *s, void *f);\nextern int printf(char *fmt, ...);\nextern int sprintf(char *str, char *fmt, ...);\nextern int snprintf(char *str, int size, char *fmt, ...);\nextern int fprintf(void *f, char *fmt, ...);\nextern void perror(char *s);\n");
                else if (strstr(line, "<stdlib.h>")) strcat(result, "extern void *malloc(int size);\nextern void *calloc(int nmemb, int size);\nextern void free(void *ptr);\nextern void exit(int status);\nextern long strtol(char *nptr, char *endptr, int base);\nextern double strtod(char *nptr, char *endptr);\n");
                else if (strstr(line, "<string.h>")) strcat(result, "extern int strlen(char *s);\nextern char *strcpy(char *dest, char *src);\nextern char *strncpy(char *dest, char *src, int n);\nextern char *strcat(char *dest, char *src);\nextern int strcmp(char *s1, char *s2);\nextern int strncmp(char *s1, char *s2, int n);\nextern char *strchr(char *s, int c);\nextern char *strstr(char *haystack, char *needle);\nextern char *strtok(char *str, char *delim);\n");
                else if (strstr(line, "<ctype.h>")) strcat(result, "extern int isalpha(int c);\nextern int isalnum(int c);\nextern int isdigit(int c);\nextern int isspace(int c);\nextern int isxdigit(int c);\n");
                else if (strstr(line, "<stddef.h>")) strcat(result, "typedef int size_t;\n");
            }
        } else {
            char expanded[MAX_OUTPUT_SIZE];
            expand_line(expanded, orig_line); // expand from orig_line to preserve spaces if needed
            strcat(result, expanded);
            strcat(result, "\n");
        }
        line = strtok(NULL, "\n");
    }
    
    free(lines);
    free(output);
    return result;
}

char *macro_expand_file(const char *filename) {
    FILE *f = fopen((char*)filename, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *input = malloc(size + 1);
    fread(input, 1, size, f);
    input[size] = '\0';
    fclose(f);
    
    char *output = macro_expand(input);
    free(input);
    
    return output;
}

--- START OF FILE lexer.h ---

#ifndef C0C_LEXER_H
#define C0C_LEXER_H

#include <stddef.h>

typedef enum {
    TK_EOF, TK_INT, TK_CHAR, TK_VOID, TK_FLOAT, TK_DOUBLE, TK_UNSIGNED, TK_SHORT, TK_LONG, TK_CONST,
    TK_STRUCT, TK_TYPEDEF, TK_RETURN, TK_IF, TK_ELSE, TK_WHILE, TK_FOR, TK_DO,
    TK_SWITCH, TK_CASE, TK_DEFAULT, TK_BREAK, TK_CONTINUE,
    TK_SIZEOF, TK_IDENT, TK_NUM, TK_FLOAT_LIT, TK_STR, TK_CHAR_LIT,
    TK_EXTERN, TK_ENUM, TK_ELLIPSIS,
    TK_ASSIGN = '=', TK_PLUS = '+', TK_MINUS = '-', TK_MUL = '*', TK_DIV = '/',
    TK_MOD = '%', TK_LT = '<', TK_GT = '>', TK_NOT = '!', TK_LPAREN = '(', TK_RPAREN = ')',
    TK_LBRACE = '{', TK_RBRACE = '}', TK_SEMI = ';', TK_COMMA = ',',
    TK_EQ = 257, TK_NE, TK_LE, TK_GE, TK_ANDAND, TK_OROR, TK_PLUSPLUS, TK_MINUSMINUS,
    TK_PLUSEQ, TK_MINUSEQ, TK_MULEQ, TK_DIVEQ, TK_MODEQ, TK_ARROW
} TokenType;

typedef struct {
    TokenType type;
    int val;
    double fval;
    char name[64];
    char str_val[256];
} Token;

extern char *src;
extern char *p;
extern Token cur_tok;
extern int cur_line;
extern int cur_col;

void error(const char *msg);
void next_token(void);
void expect(TokenType type, const char *msg);

#endif

--- START OF FILE lexer.c ---

#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *src;
char *p;
Token cur_tok;
int cur_line = 1;
int cur_col = 1;

static void advance_char(void) {
    if (*p == '\0') return;
    if (*p == '\n') { cur_line++; cur_col = 1; }
    else cur_col++;
    p++;
}

void error(const char *msg) {
    fprintf(stderr, "編譯錯誤(%d:%d): %s\n", cur_line, cur_col, msg);
    exit(1);
}

static int hex_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int parse_escape_char(void) {
    if (*p == 'n') { advance_char(); return '\n'; }
    if (*p == 't') { advance_char(); return '\t'; }
    if (*p == 'r') { advance_char(); return '\r'; }
    if (*p == '\\') { advance_char(); return '\\'; }
    if (*p == '"') { advance_char(); return '"'; }
    if (*p == '\'') { advance_char(); return '\''; }
    if (*p == 'x' || *p == 'X') {
        advance_char();
        int val = 0;
        int cnt = 0;
        int hv;
        while ((hv = hex_val(*p)) >= 0 && cnt < 2) {
            val = (val << 4) | hv;
            advance_char();
            cnt++;
        }
        return val;
    }
    if (*p >= '0' && *p <= '7') {
        int val = 0;
        int cnt = 0;
        while (*p >= '0' && *p <= '7' && cnt < 3) {
            val = (val << 3) | (*p - '0');
            advance_char();
            cnt++;
        }
        return val;
    }
    int c = (unsigned char)*p;
    if (*p) advance_char();
    return c;
}

void next_token(void) {
    if (p == src && strncmp(p, "\xEF\xBB\xBF", 3) == 0) { advance_char(); advance_char(); advance_char(); }

    while (1) {
        while (isspace((unsigned char)*p)) advance_char();
        if (strncmp(p, "//", 2) == 0) { while (*p != '\0' && *p != '\n') advance_char(); continue; }
        if (strncmp(p, "/*", 2) == 0) {
            advance_char(); advance_char();
            while (*p != '\0' && strncmp(p, "*/", 2) != 0) advance_char();
            if (*p != '\0') { advance_char(); advance_char(); }
            continue;
        }
        if (*p == '#') { while (*p != '\0' && *p != '\n') advance_char(); continue; }
        break;
    }

    if (*p == '\0') { cur_tok.type = TK_EOF; return; }

    if (*p == '"') {
        advance_char();
        char *s = cur_tok.str_val;
        while (*p != '\0' && *p != '"') {
            if (*p == '\\') {
                advance_char();
                *s++ = (char)parse_escape_char();
            } else {
                *s++ = *p;
                advance_char();
            }
        }
        *s = '\0';
        if (*p == '"') advance_char();
        cur_tok.type = TK_STR;
        return;
    }

    if (isalpha((unsigned char)*p) || *p == '_') {
        char *start = p;
        while (isalnum((unsigned char)*p) || *p == '_') advance_char();
        int len = p - start;
        strncpy(cur_tok.name, start, len);
        cur_tok.name[len] = '\0';

        if (strcmp(cur_tok.name, "int") == 0) cur_tok.type = TK_INT;
        else if (strcmp(cur_tok.name, "char") == 0) cur_tok.type = TK_CHAR;
        else if (strcmp(cur_tok.name, "void") == 0) cur_tok.type = TK_VOID;
        else if (strcmp(cur_tok.name, "float") == 0) cur_tok.type = TK_FLOAT;
        else if (strcmp(cur_tok.name, "double") == 0) cur_tok.type = TK_DOUBLE;
        else if (strcmp(cur_tok.name, "unsigned") == 0) cur_tok.type = TK_UNSIGNED;
        else if (strcmp(cur_tok.name, "short") == 0) cur_tok.type = TK_SHORT;
        else if (strcmp(cur_tok.name, "long") == 0) cur_tok.type = TK_LONG;
        else if (strcmp(cur_tok.name, "const") == 0) cur_tok.type = TK_CONST;
        else if (strcmp(cur_tok.name, "struct") == 0) cur_tok.type = TK_STRUCT;
        else if (strcmp(cur_tok.name, "typedef") == 0) cur_tok.type = TK_TYPEDEF;
        else if (strcmp(cur_tok.name, "return") == 0) cur_tok.type = TK_RETURN;
        else if (strcmp(cur_tok.name, "if") == 0) cur_tok.type = TK_IF;
        else if (strcmp(cur_tok.name, "else") == 0) cur_tok.type = TK_ELSE;
        else if (strcmp(cur_tok.name, "while") == 0) cur_tok.type = TK_WHILE;
        else if (strcmp(cur_tok.name, "for") == 0) cur_tok.type = TK_FOR;
        else if (strcmp(cur_tok.name, "do") == 0) cur_tok.type = TK_DO;
        else if (strcmp(cur_tok.name, "switch") == 0) cur_tok.type = TK_SWITCH;
        else if (strcmp(cur_tok.name, "case") == 0) cur_tok.type = TK_CASE;
        else if (strcmp(cur_tok.name, "default") == 0) cur_tok.type = TK_DEFAULT;
        else if (strcmp(cur_tok.name, "break") == 0) cur_tok.type = TK_BREAK;
        else if (strcmp(cur_tok.name, "continue") == 0) cur_tok.type = TK_CONTINUE;
        else if (strcmp(cur_tok.name, "sizeof") == 0) cur_tok.type = TK_SIZEOF;
        else if (strcmp(cur_tok.name, "extern") == 0) cur_tok.type = TK_EXTERN;
        else if (strcmp(cur_tok.name, "enum") == 0) cur_tok.type = TK_ENUM;
        else cur_tok.type = TK_IDENT;
        return;
    }

    if (isdigit((unsigned char)*p) || (*p == '.' && isdigit((unsigned char)p[1]))) {
        char *start = p;
        int is_float = 0;
        if (*p == '.') {
            is_float = 1;
        } else {
            char *q = p;
            if (q[0] == '0' && (q[1] == 'x' || q[1] == 'X')) {
                q += 2;
                while (isxdigit((unsigned char)*q)) q++;
            } else {
                while (isdigit((unsigned char)*q)) q++;
                if (*q == '.') is_float = 1;
                if (*q == 'e' || *q == 'E') is_float = 1;
            }
        }

        if (is_float) {
            char *end = NULL;
            double f = strtod(p, &end);
            p = end;
            cur_col += (int)(p - start);
            if (*p == 'f' || *p == 'F') { advance_char(); cur_tok.val = 1; }
            else cur_tok.val = 0;
            cur_tok.fval = f;
            cur_tok.type = TK_FLOAT_LIT;
        } else {
            char *end = NULL;
            long v = strtol(p, &end, 0);
            p = end;
            cur_col += (int)(p - start);
            cur_tok.val = (int)v;
            cur_tok.type = TK_NUM;
        }
        return;
    }

    if (*p == '\'') {
        advance_char();
        int ch = 0;
        if (*p == '\\') {
            advance_char();
            ch = parse_escape_char();
        } else {
            ch = (unsigned char)*p; advance_char();
        }
        if (*p == '\'') advance_char();
        cur_tok.val = ch;
        cur_tok.type = TK_CHAR_LIT;
        return;
    }

    if (p[0] == '.' && p[1] == '.' && p[2] == '.') { cur_tok.type = TK_ELLIPSIS; advance_char(); advance_char(); advance_char(); return; }
    if (p[0] == '=' && p[1] == '=') { cur_tok.type = TK_EQ; advance_char(); advance_char(); return; }
    if (p[0] == '!' && p[1] == '=') { cur_tok.type = TK_NE; advance_char(); advance_char(); return; }
    if (p[0] == '<' && p[1] == '=') { cur_tok.type = TK_LE; advance_char(); advance_char(); return; }
    if (p[0] == '>' && p[1] == '=') { cur_tok.type = TK_GE; advance_char(); advance_char(); return; }
    if (p[0] == '&' && p[1] == '&') { cur_tok.type = TK_ANDAND; advance_char(); advance_char(); return; }
    if (p[0] == '|' && p[1] == '|') { cur_tok.type = TK_OROR; advance_char(); advance_char(); return; }
    if (p[0] == '+' && p[1] == '+') { cur_tok.type = TK_PLUSPLUS; advance_char(); advance_char(); return; }
    if (p[0] == '-' && p[1] == '-') { cur_tok.type = TK_MINUSMINUS; advance_char(); advance_char(); return; }
    if (p[0] == '-' && p[1] == '>') { cur_tok.type = TK_ARROW; advance_char(); advance_char(); return; }
    if (p[0] == '+' && p[1] == '=') { cur_tok.type = TK_PLUSEQ; advance_char(); advance_char(); return; }
    if (p[0] == '-' && p[1] == '=') { cur_tok.type = TK_MINUSEQ; advance_char(); advance_char(); return; }
    if (p[0] == '*' && p[1] == '=') { cur_tok.type = TK_MULEQ; advance_char(); advance_char(); return; }
    if (p[0] == '/' && p[1] == '=') { cur_tok.type = TK_DIVEQ; advance_char(); advance_char(); return; }
    if (p[0] == '%' && p[1] == '=') { cur_tok.type = TK_MODEQ; advance_char(); advance_char(); return; }

    cur_tok.type = (TokenType)(*p);
    advance_char();
}

void expect(TokenType type, const char *msg) {
    if (cur_tok.type == type) next_token();
    else error(msg);
}

--- START OF FILE parser.h ---

#ifndef C0C_PARSER_H
#define C0C_PARSER_H

#include "ast.h"

ASTNode* parse_program(void);

#endif

--- START OF FILE parser.c ---

#include "parser.h"
#include "lexer.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    char name[64];
    CType ty;
    int array_len;
    int struct_id;
} Sym;

static Sym symtab[256];
static int sym_cnt = 0;

typedef struct {
    char name[64];
    CType ret;
    int ret_struct_id;
    CType params[16];
    int param_struct_id[16];
    int param_cnt;
} FuncSym;

static FuncSym func_tab[128];
static int func_cnt = 0;
static CType current_func_ret = TY_INT;
static int last_struct_id = -1;

static FuncSym* func_find(const char *name);

typedef struct {
    char name[64];
    CType ty;
    int struct_id;
} TypedefSym;

static TypedefSym typedef_tab[128];
static int typedef_cnt = 0;

typedef struct {
    char name[64];
    int struct_id;
} StructSym;

static StructSym struct_tab[64];
static int struct_sym_cnt = 0;

typedef struct {
    char name[64];
    int val;
} EnumSym;

static EnumSym enum_tab[256];
static int enum_cnt = 0;

typedef struct {
    char name[64];
    CType ty;
    int array_len;
    int struct_id;
} GlobalSym;

static GlobalSym gsymtab[256];
static int gsym_cnt = 0;

static void sym_reset(void) { sym_cnt = 0; }

static void sym_add(const char *name, CType ty, int array_len, int struct_id) {
    if (sym_cnt >= 256) error("符號表已滿");
    strcpy(symtab[sym_cnt].name, name);
    symtab[sym_cnt].ty = ty;
    symtab[sym_cnt].array_len = array_len;
    symtab[sym_cnt].struct_id = struct_id;
    sym_cnt++;
}

static int global_find_index(const char *name) {
    for (int i = gsym_cnt - 1; i >= 0; i--) {
        if (strcmp(gsymtab[i].name, name) == 0) return i;
    }
    return -1;
}

static void global_add(const char *name, CType ty, int array_len, int struct_id) {
    if (gsym_cnt >= 256) error("全域符號表已滿");
    if (global_find_index(name) >= 0) return; // 容許多次宣告 (extern)
    strcpy(gsymtab[gsym_cnt].name, name);
    gsymtab[gsym_cnt].ty = ty;
    gsymtab[gsym_cnt].array_len = array_len;
    gsymtab[gsym_cnt].struct_id = struct_id;
    gsym_cnt++;
}

static void func_add(const char *name, CType ret, int ret_struct_id,
                     CType *params, int *param_struct_id, int param_cnt, int is_vararg) {
    if (func_cnt >= 128) error("函式表已滿");
    for (int i = 0; i < func_cnt; i++) {
        if (strcmp(func_tab[i].name, name) == 0) return; // 已宣告
    }
    strcpy(func_tab[func_cnt].name, name);
    func_tab[func_cnt].ret = ret;
    func_tab[func_cnt].ret_struct_id = ret_struct_id;
    func_tab[func_cnt].param_cnt = param_cnt;
    for (int i = 0; i < param_cnt; i++) {
        func_tab[func_cnt].params[i] = params[i];
        func_tab[func_cnt].param_struct_id[i] = param_struct_id[i];
    }
    func_cnt++;
}

static FuncSym* func_find(const char *name) {
    for (int i = 0; i < func_cnt; i++) {
        if (strcmp(func_tab[i].name, name) == 0) return &func_tab[i];
    }
    return NULL;
}

static void typedef_add(const char *name, CType ty, int struct_id) {
    if (typedef_cnt >= 128) error("typedef 表已滿");
    strcpy(typedef_tab[typedef_cnt].name, name);
    typedef_tab[typedef_cnt].ty = ty;
    typedef_tab[typedef_cnt].struct_id = struct_id;
    typedef_cnt++;
}

static int typedef_find(const char *name, CType *ty, int *struct_id) {
    for (int i = typedef_cnt - 1; i >= 0; i--) {
        if (strcmp(typedef_tab[i].name, name) == 0) {
            *ty = typedef_tab[i].ty;
            *struct_id = typedef_tab[i].struct_id;
            return 1;
        }
    }
    return 0;
}

static int struct_find(const char *name) {
    for (int i = 0; i < struct_sym_cnt; i++) {
        if (strcmp(struct_tab[i].name, name) == 0) return struct_tab[i].struct_id;
    }
    return -1;
}

static int add_struct_def(const char *name) {
    if (g_struct_def_cnt >= 64) error("struct 定義過多");
    int id = g_struct_def_cnt++;
    strcpy(g_struct_defs[id].name, name);
    g_struct_defs[id].size = 0;
    g_struct_defs[id].field_cnt = 0;
    if (struct_sym_cnt < 64) {
        strcpy(struct_tab[struct_sym_cnt].name, name);
        struct_tab[struct_sym_cnt].struct_id = id;
        struct_sym_cnt++;
    }
    return id;
}

static StructDef* get_struct_def(int id) {
    if (id < 0 || id >= g_struct_def_cnt) error("struct id 錯誤");
    return &g_struct_defs[id];
}

static int type_size(CType ty, int struct_id) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return 1;
    if (ty == TY_SHORT || ty == TY_USHORT) return 2;
    if (ty == TY_INT || ty == TY_UINT) return 4;
    if (ty == TY_LONG || ty == TY_ULONG) return 8;
    if (ty == TY_FLOAT) return 4;
    if (ty == TY_DOUBLE) return 8;
    if (ty == TY_INT_PTR || ty == TY_UINT_PTR || ty == TY_SHORT_PTR || ty == TY_USHORT_PTR ||
        ty == TY_LONG_PTR || ty == TY_ULONG_PTR || ty == TY_CHAR_PTR || ty == TY_UCHAR_PTR ||
        ty == TY_FLOAT_PTR || ty == TY_DOUBLE_PTR || ty == TY_STRUCT_PTR) return 8;
    if (ty == TY_STRUCT) {
        StructDef *d = get_struct_def(struct_id);
        return d->size;
    }
    return 0;
}

static Sym* sym_find(const char *name) {
    for (int i = sym_cnt - 1; i >= 0; i--) {
        if (strcmp(symtab[i].name, name) == 0) return &symtab[i];
    }
    int gi = global_find_index(name);
    if (gi >= 0) {
        static Sym gsym;
        strcpy(gsym.name, gsymtab[gi].name);
        gsym.ty = gsymtab[gi].ty;
        gsym.array_len = gsymtab[gi].array_len;
        gsym.struct_id = gsymtab[gi].struct_id;
        return &gsym;
    }
    error("找不到變數宣告");
    return NULL;
}

static int is_ptr(CType ty) {
    return ty == TY_INT_PTR || ty == TY_UINT_PTR || ty == TY_SHORT_PTR || ty == TY_USHORT_PTR ||
           ty == TY_LONG_PTR || ty == TY_ULONG_PTR || ty == TY_CHAR_PTR || ty == TY_UCHAR_PTR ||
           ty == TY_FLOAT_PTR || ty == TY_DOUBLE_PTR || ty == TY_STRUCT_PTR;
}
static int is_float(CType ty) { return ty == TY_FLOAT || ty == TY_DOUBLE; }
static int is_int(CType ty) {
    return ty == TY_CHAR || ty == TY_UCHAR || ty == TY_SHORT || ty == TY_USHORT ||
           ty == TY_INT || ty == TY_UINT || ty == TY_LONG || ty == TY_ULONG;
}
static int is_unsigned(CType ty) {
    return ty == TY_UCHAR || ty == TY_USHORT || ty == TY_UINT || ty == TY_ULONG;
}
static int int_rank(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return 1;
    if (ty == TY_SHORT || ty == TY_USHORT) return 2;
    if (ty == TY_INT || ty == TY_UINT) return 3;
    if (ty == TY_LONG || ty == TY_ULONG) return 4;
    return 0;
}
static CType int_promote(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR || ty == TY_SHORT || ty == TY_USHORT) return TY_INT;
    return ty;
}
static CType int_type_from_rank(int rank, int is_uns) {
    if (rank == 1) return is_uns ? TY_UCHAR : TY_CHAR;
    if (rank == 2) return is_uns ? TY_USHORT : TY_SHORT;
    if (rank == 3) return is_uns ? TY_UINT : TY_INT;
    if (rank == 4) return is_uns ? TY_ULONG : TY_LONG;
    return TY_INT;
}
static CType common_arith_type(CType a, CType b) {
    if (is_float(a) || is_float(b)) {
        if (a == TY_DOUBLE || b == TY_DOUBLE) return TY_DOUBLE;
        return TY_FLOAT;
    }
    a = int_promote(a);
    b = int_promote(b);
    int ra = int_rank(a);
    int rb = int_rank(b);
    int ua = is_unsigned(a);
    int ub = is_unsigned(b);
    if (ra == rb) return int_type_from_rank(ra, ua || ub);
    if (ra > rb) {
        if (ua) return int_type_from_rank(ra, 1);
        if (ub) return int_type_from_rank(ra, 0);
        return int_type_from_rank(ra, 0);
    } else {
        if (ub) return int_type_from_rank(rb, 1);
        if (ua) return int_type_from_rank(rb, 0);
        return int_type_from_rank(rb, 0);
    }
}
static CType ptr_of(CType ty) {
    if (ty == TY_CHAR) return TY_CHAR_PTR;
    if (ty == TY_UCHAR) return TY_UCHAR_PTR;
    if (ty == TY_SHORT) return TY_SHORT_PTR;
    if (ty == TY_USHORT) return TY_USHORT_PTR;
    if (ty == TY_INT) return TY_INT_PTR;
    if (ty == TY_UINT) return TY_UINT_PTR;
    if (ty == TY_LONG) return TY_LONG_PTR;
    if (ty == TY_ULONG) return TY_ULONG_PTR;
    if (ty == TY_FLOAT) return TY_FLOAT_PTR;
    if (ty == TY_DOUBLE) return TY_DOUBLE_PTR;
    if (ty == TY_STRUCT) return TY_STRUCT_PTR;
    return TY_LONG_PTR; // Any double pointer casted internally to 8 bytes long pointer
}
static CType base_of(CType ty) {
    if (ty == TY_CHAR_PTR) return TY_CHAR;
    if (ty == TY_UCHAR_PTR) return TY_UCHAR;
    if (ty == TY_SHORT_PTR) return TY_SHORT;
    if (ty == TY_USHORT_PTR) return TY_USHORT;
    if (ty == TY_INT_PTR) return TY_INT;
    if (ty == TY_UINT_PTR) return TY_UINT;
    if (ty == TY_LONG_PTR) return TY_LONG;
    if (ty == TY_ULONG_PTR) return TY_ULONG;
    if (ty == TY_FLOAT_PTR) return TY_FLOAT;
    if (ty == TY_DOUBLE_PTR) return TY_DOUBLE;
    if (ty == TY_STRUCT_PTR) return TY_STRUCT;
    return TY_INT;
}

static CType parse_type_allow_void(int allow_void) {
    CType base;
    last_struct_id = -1;
    while (cur_tok.type == TK_CONST) next_token();
    if (allow_void && cur_tok.type == TK_VOID) { next_token(); base = TY_VOID; goto ptr_check; }
    if (cur_tok.type == TK_STRUCT) {
        next_token();
        if (cur_tok.type != TK_IDENT) error("預期 struct 名稱");
        int sid = struct_find(cur_tok.name);
        if (sid < 0) error("未知的 struct");
        next_token();
        base = TY_STRUCT;
        last_struct_id = sid;
        goto ptr_check;
    }
    if (cur_tok.type == TK_IDENT) {
        CType t;
        int sid = -1;
        if (typedef_find(cur_tok.name, &t, &sid)) {
            next_token();
            last_struct_id = sid;
            base = t;
            goto ptr_check;
        }
    }

    int is_uns = 0;
    int is_short = 0;
    int is_long = 0;
    int has_type = 0;
    CType builtin = TY_INT;
    while (1) {
        if (cur_tok.type == TK_CONST) { next_token(); continue; }
        if (cur_tok.type == TK_UNSIGNED) { is_uns = 1; next_token(); continue; }
        if (cur_tok.type == TK_SHORT) { is_short = 1; next_token(); continue; }
        if (cur_tok.type == TK_LONG) { is_long = 1; next_token(); continue; }
        if (cur_tok.type == TK_INT) { has_type = 1; builtin = TY_INT; next_token(); continue; }
        if (cur_tok.type == TK_CHAR) { has_type = 1; builtin = TY_CHAR; next_token(); continue; }
        if (cur_tok.type == TK_FLOAT) { has_type = 1; builtin = TY_FLOAT; next_token(); continue; }
        if (cur_tok.type == TK_DOUBLE) { has_type = 1; builtin = TY_DOUBLE; next_token(); continue; }
        break;
    }
    if (!has_type && (is_uns || is_short || is_long)) {
        builtin = TY_INT;
        has_type = 1;
    }
    if (!has_type) {
        error("預期型別");
        return TY_INT;
    }
    if (builtin == TY_FLOAT || builtin == TY_DOUBLE) {
        base = builtin;
    } else if (builtin == TY_CHAR) {
        base = is_uns ? TY_UCHAR : TY_CHAR;
    } else {
        if (is_short) base = is_uns ? TY_USHORT : TY_SHORT;
        else if (is_long) base = is_uns ? TY_ULONG : TY_LONG;
        else base = is_uns ? TY_UINT : TY_INT;
    }

ptr_check:
    while (cur_tok.type == TK_MUL) {
        next_token();
        if (base == TY_VOID) base = TY_CHAR_PTR; // void* maps to char*
        else base = ptr_of(base); // double pointer maps to i64*
    }
    return base;
}

static CType parse_type(void) {
    return parse_type_allow_void(0);
}

static ASTNode* make_var_node(const char *name) {
    Sym *s = sym_find(name);
    ASTNode *n = new_node(AST_VAR);
    strcpy(n->name, name);
    n->ty = s->ty;
    n->array_len = s->array_len;
    n->struct_id = s->struct_id;
    return n;
}

static ASTNode* parse_expr();
static ASTNode* parse_stmt();
static ASTNode* parse_block();
static ASTNode* parse_decl_stmt(int expect_semi);

static int is_struct_def_ahead(void) {
    char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col;
    next_token();
    if (cur_tok.type != TK_IDENT) { p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col; return 0; }
    next_token();
    int is_def = (cur_tok.type == TK_LBRACE);
    p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
    return is_def;
}

static int is_typedef_name(void) {
    if (cur_tok.type != TK_IDENT) return 0;
    CType t; int sid;
    return typedef_find(cur_tok.name, &t, &sid);
}

static int is_type_start(void) {
    if (cur_tok.type == TK_INT || cur_tok.type == TK_CHAR || cur_tok.type == TK_FLOAT ||
        cur_tok.type == TK_DOUBLE || cur_tok.type == TK_UNSIGNED || cur_tok.type == TK_SHORT ||
        cur_tok.type == TK_LONG || cur_tok.type == TK_CONST || cur_tok.type == TK_STRUCT) return 1;
    return is_typedef_name();
}

static int is_func_def_ahead(void) {
    char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col; int saved_struct = last_struct_id;
    if (!is_type_start()) return 0;
    parse_type_allow_void(1);
    if (cur_tok.type != TK_IDENT) { p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col; last_struct_id = saved_struct; return 0; }
    next_token();
    int is_func = (cur_tok.type == TK_LPAREN);
    p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col; last_struct_id = saved_struct;
    return is_func;
}

static int is_lvalue_node(ASTNode *n) {
    if (!n) return 0;
    return n->type == AST_VAR || n->type == AST_INDEX || n->type == AST_DEREF || n->type == AST_MEMBER;
}

static ASTNode* parse_primary() {
    if (cur_tok.type == TK_NUM) {
        ASTNode *n = new_node(AST_NUM);
        n->val = cur_tok.val;
        n->ty = TY_INT;
        next_token();
        return n;
    } else if (cur_tok.type == TK_FLOAT_LIT) {
        ASTNode *n = new_node(AST_FLOAT);
        n->fval = cur_tok.fval;
        n->ty = (cur_tok.val == 1) ? TY_FLOAT : TY_DOUBLE;
        next_token();
        return n;
    } else if (cur_tok.type == TK_CHAR_LIT) {
        ASTNode *n = new_node(AST_NUM);
        n->val = cur_tok.val;
        n->ty = TY_CHAR;
        next_token();
        return n;
    } else if (cur_tok.type == TK_STR) {
        ASTNode *n = new_node(AST_STR);
        strcpy(n->str_val, cur_tok.str_val);
        n->ty = TY_CHAR_PTR;
        next_token();
        return n;
    } else if (cur_tok.type == TK_IDENT) {
        char name[64];
        strcpy(name, cur_tok.name);
        for (int i=0; i<enum_cnt; i++) {
            if (strcmp(enum_tab[i].name, name) == 0) {
                next_token();
                ASTNode *n = new_node(AST_NUM);
                n->val = enum_tab[i].val;
                n->ty = TY_INT;
                return n;
            }
        }
        next_token();
        if (cur_tok.type == TK_LPAREN) {
            next_token();
            ASTNode *n = new_node(AST_CALL);
            strcpy(n->name, name);
            FuncSym *fs = func_find(name);
            n->ty = fs ? fs->ret : TY_INT;
            n->struct_id = fs ? fs->ret_struct_id : -1;
            ASTNode *head = NULL, *tail = NULL;
            int arg_count = 0;
            if (cur_tok.type != TK_RPAREN) {
                head = tail = parse_expr();
                arg_count++;
                while (cur_tok.type == TK_COMMA) {
                    next_token();
                    tail->next = parse_expr();
                    tail = tail->next;
                    arg_count++;
                }
            }
            expect(TK_RPAREN, "預期 ')'");
            n->left = head;
            return n;
        } else {
            return make_var_node(name);
        }
    } else if (cur_tok.type == TK_LPAREN) {
        next_token();
        ASTNode *n = parse_expr();
        expect(TK_RPAREN, "預期 ')'");
        return n;
    }
    error("預期 表達式 (Expression)");
    return NULL;
}

static ASTNode* parse_postfix() {
    ASTNode *n = parse_primary();
    while (1) {
        if (cur_tok.type == '[') {
            next_token();
            ASTNode *idx = parse_expr();
            expect(']', "預期 ']'");
            ASTNode *nidx = new_node(AST_INDEX);
            nidx->left = n; nidx->right = idx;
            nidx->ty = base_of(n->ty);
            nidx->struct_id = n->struct_id;
            n = nidx;
            continue;
        }
        if (cur_tok.type == '.' || cur_tok.type == TK_ARROW) {
            int is_arrow = (cur_tok.type == TK_ARROW);
            next_token();
            ASTNode *m = new_node(AST_MEMBER);
            m->left = n; strcpy(m->name, cur_tok.name); m->op = is_arrow;
            StructDef *def = get_struct_def(n->struct_id);
            int found = 0;
            for (int i = 0; i < def->field_cnt; i++) {
                if (strcmp(def->fields[i].name, m->name) == 0) {
                    m->ty = def->fields[i].ty;
                    m->val = def->fields[i].offset;
                    m->struct_id = def->fields[i].struct_id;
                    found = 1; break;
                }
            }
            if (!found) error("找不到 struct 欄位");
            next_token();
            n = m;
            continue;
        }
        if (cur_tok.type == TK_PLUSPLUS || cur_tok.type == TK_MINUSMINUS) {
            ASTNode *inc = new_node(AST_INCDEC);
            inc->op = cur_tok.type; inc->is_prefix = 0; inc->left = n; inc->ty = n->ty;
            next_token(); n = inc;
            break;
        }
        break;
    }
    return n;
}

static ASTNode* parse_unary() {
    if (cur_tok.type == TK_SIZEOF) {
        next_token();
        ASTNode *n = new_node(AST_SIZEOF);
        n->ty = TY_INT;
        if (cur_tok.type == TK_LPAREN) {
            char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col;
            next_token();
            int is_type = 0;
            if (is_type_start()) is_type = 1;
            if (is_type) {
                p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
                expect(TK_LPAREN, "預期 '('");
                CType ty = parse_type_allow_void(0);
                expect(TK_RPAREN, "預期 ')'");
                n->val = type_size(ty, last_struct_id);
                return n;
            }
            p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
        }
        n->left = parse_unary();
        if (n->left && n->left->type == AST_VAR && n->left->array_len > 0) {
            int esz = type_size(base_of(n->left->ty), n->left->struct_id);
            n->val = n->left->array_len * esz;
        } else n->val = type_size(n->left->ty, n->left->struct_id);
        return n;
    }
    if (cur_tok.type == TK_LPAREN) {
        char *saved_p = p; Token saved_tok = cur_tok; int saved_line = cur_line; int saved_col = cur_col;
        next_token();
        if (is_type_start()) {
            p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
            expect(TK_LPAREN, "預期 '('");
            CType ty = parse_type_allow_void(0);
            expect(TK_RPAREN, "預期 ')'");
            ASTNode *n = new_node(AST_CAST);
            n->ty = ty; n->struct_id = last_struct_id;
            n->left = parse_unary();
            return n;
        }
        p = saved_p; cur_tok = saved_tok; cur_line = saved_line; cur_col = saved_col;
    }
    if (cur_tok.type == TK_MINUS || cur_tok.type == TK_NOT ||
        cur_tok.type == TK_PLUSPLUS || cur_tok.type == TK_MINUSMINUS ||
        cur_tok.type == TK_PLUS || cur_tok.type == TK_MUL || cur_tok.type == '&') {
        TokenType op = cur_tok.type; next_token();
        ASTNode *operand = parse_unary();
        if (op == '&') {
            ASTNode *n = new_node(AST_ADDR); n->left = operand;
            n->ty = ptr_of(operand->ty); n->struct_id = operand->struct_id; return n;
        }
        if (op == TK_MUL) {
            ASTNode *n = new_node(AST_DEREF); n->left = operand;
            n->ty = base_of(operand->ty); n->struct_id = operand->struct_id; return n;
        }
        if (op == TK_PLUSPLUS || op == TK_MINUSMINUS) {
            ASTNode *inc = new_node(AST_INCDEC); inc->op = op; inc->is_prefix = 1;
            inc->left = operand; inc->ty = operand->ty; return inc;
        }
        ASTNode *n = new_node(AST_UNARY); n->op = op; n->left = operand;
        n->ty = (op == TK_MINUS || op == TK_PLUS) ? operand->ty : TY_INT;
        return n;
    }
    return parse_postfix();
}

static ASTNode* parse_mul() {
    ASTNode *n = parse_unary();
    while (cur_tok.type == TK_MUL || cur_tok.type == TK_DIV || cur_tok.type == TK_MOD) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_unary(); n = p;
        p->ty = common_arith_type(p->left->ty, p->right->ty);
    }
    return n;
}

static ASTNode* parse_add() {
    ASTNode *n = parse_mul();
    while (cur_tok.type == TK_PLUS || cur_tok.type == TK_MINUS) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_mul(); n = p;
        if (is_ptr(p->left->ty) && is_ptr(p->right->ty) && p->op == TK_MINUS) p->ty = TY_INT;
        else if (is_ptr(p->left->ty)) { p->ty = p->left->ty; p->struct_id = p->left->struct_id; }
        else if (is_ptr(p->right->ty)) { p->ty = p->right->ty; p->struct_id = p->right->struct_id; }
        else p->ty = common_arith_type(p->left->ty, p->right->ty);
    }
    return n;
}

static ASTNode* parse_rel() {
    ASTNode *n = parse_add();
    while (cur_tok.type == TK_LT || cur_tok.type == TK_GT || cur_tok.type == TK_LE || cur_tok.type == TK_GE) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_add(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_eq() {
    ASTNode *n = parse_rel();
    while (cur_tok.type == TK_EQ || cur_tok.type == TK_NE) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_rel(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_and() {
    ASTNode *n = parse_eq();
    while (cur_tok.type == TK_ANDAND) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_eq(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_or() {
    ASTNode *n = parse_and();
    while (cur_tok.type == TK_OROR) {
        ASTNode *p = new_node(AST_BINOP); p->op = cur_tok.type; p->left = n;
        next_token(); p->right = parse_and(); n = p; p->ty = TY_INT;
    }
    return n;
}

static ASTNode* parse_expr() { return parse_or(); }

static ASTNode* parse_lvalue() {
    ASTNode *n = parse_unary();
    if (n->type == AST_VAR || n->type == AST_INDEX || n->type == AST_DEREF || n->type == AST_MEMBER) return n;
    error("預期左值");
    return NULL;
}

static ASTNode* parse_block() {
    expect(TK_LBRACE, "預期 '{'");
    int scope_mark = sym_cnt;
    ASTNode *head = NULL, *tail = NULL;
    while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
        ASTNode *stmt = parse_stmt();
        if (!head) head = tail = stmt; else { tail->next = stmt; tail = stmt; }
    }
    expect(TK_RBRACE, "預期 '}'");
    sym_cnt = scope_mark;
    return head;
}

static ASTNode* parse_struct_decl_or_def() {
    expect(TK_STRUCT, "預期 'struct'");
    char struct_name[64];
    strcpy(struct_name, cur_tok.name); next_token();
    if (cur_tok.type == TK_LBRACE) {
        int sid = struct_find(struct_name);
        if (sid < 0) sid = add_struct_def(struct_name);
        StructDef *def = get_struct_def(sid);
        next_token();
        while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
            CType fty = parse_type_allow_void(0);
            StructField *f = &def->fields[def->field_cnt++];
            strcpy(f->name, cur_tok.name);
            f->ty = fty; f->offset = def->size; f->struct_id = last_struct_id;
            def->size += type_size(fty, f->struct_id);
            next_token(); expect(TK_SEMI, "預期 ';'");
        }
        expect(TK_RBRACE, "預期 '}'"); expect(TK_SEMI, "預期 ';'");
        return NULL;
    }
    error("struct 宣告需有定義"); return NULL;
}

static ASTNode* parse_typedef_stmt() {
    expect(TK_TYPEDEF, "預期 'typedef'");
    if (cur_tok.type == TK_STRUCT) {
        expect(TK_STRUCT, "預期 'struct'");
        char struct_name[64] = {0};
        if (cur_tok.type == TK_IDENT) { strcpy(struct_name, cur_tok.name); next_token(); }
        int sid = -1;
        if (cur_tok.type == TK_LBRACE) {
            if (struct_name[0] == '\0') snprintf(struct_name, sizeof(struct_name), "__anon%d", g_struct_def_cnt);
            sid = struct_find(struct_name);
            if (sid < 0) sid = add_struct_def(struct_name);
            StructDef *def = get_struct_def(sid); next_token();
            while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
                CType fty = parse_type_allow_void(0);
                StructField *f = &def->fields[def->field_cnt++];
                strcpy(f->name, cur_tok.name); f->ty = fty; f->offset = def->size; f->struct_id = last_struct_id;
                def->size += type_size(fty, f->struct_id); next_token(); expect(TK_SEMI, "預期 ';'");
            }
            expect(TK_RBRACE, "預期 '}'");
        } else sid = struct_find(struct_name);
        typedef_add(cur_tok.name, TY_STRUCT, sid);
        next_token(); expect(TK_SEMI, "預期 ';'");
        return NULL;
    }
    if (cur_tok.type == TK_ENUM) {
        next_token();
        if (cur_tok.type == TK_IDENT) next_token();
        expect(TK_LBRACE, "預期 '{'");
        int enum_val = 0;
        while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
            strcpy(enum_tab[enum_cnt].name, cur_tok.name); next_token();
            if (cur_tok.type == TK_ASSIGN) {
                next_token();
                if (cur_tok.type == TK_MINUS) { next_token(); enum_val = -cur_tok.val; }
                else enum_val = cur_tok.val;
                next_token();
            }
            enum_tab[enum_cnt++].val = enum_val++;
            if (cur_tok.type == TK_COMMA) next_token();
        }
        expect(TK_RBRACE, "預期 '}'");
        typedef_add(cur_tok.name, TY_INT, -1);
        next_token(); expect(TK_SEMI, "預期 ';'");
        return NULL;
    }
    CType ty = parse_type_allow_void(0);
    typedef_add(cur_tok.name, ty, -1);
    next_token(); expect(TK_SEMI, "預期 ';'");
    return NULL;
}

static ASTNode* parse_decl_stmt(int expect_semi) {
    CType decl_ty = parse_type();
    ASTNode *n = new_node(AST_DECL);
    strcpy(n->name, cur_tok.name);
    expect(TK_IDENT, "預期變數名稱");
    n->ty = decl_ty; n->struct_id = last_struct_id; n->array_len = 0; n->init_kind = 0;

    int unsized = 0;
    if (cur_tok.type == '[') {
        next_token();
        if (cur_tok.type == ']') { unsized = 1; n->array_len = -1; next_token(); }
        else { n->array_len = cur_tok.val; next_token(); expect(']', "預期 ']'"); }
        n->ty = ptr_of(decl_ty);
    }

    if (cur_tok.type == TK_ASSIGN) {
        next_token();
        if (n->array_len != 0) {
            if (cur_tok.type == TK_STR) { n->init_kind = 3; strcpy(n->str_val, cur_tok.str_val); next_token(); }
            else if (cur_tok.type == '{') {
                n->init_kind = 2; next_token();
                ASTNode *head = NULL, *tail = NULL; int count = 0;
                if (cur_tok.type != '}') {
                    head = tail = parse_expr(); count++;
                    while (cur_tok.type == TK_COMMA) { next_token(); if (cur_tok.type == '}') break; tail->next = parse_expr(); tail = tail->next; count++; }
                }
                expect('}', "預期 '}'"); n->left = head;
                if (n->array_len == -1) n->array_len = count;
            }
            if (n->array_len == -1 && n->init_kind == 3) n->array_len = (int)strlen(n->str_val) + 1;
        } else {
            n->init_kind = 1; n->left = parse_expr();
        }
    }
    if (expect_semi) expect(TK_SEMI, "預期 ';'");
    return n;
}

static ASTNode* parse_if_stmt() {
    expect(TK_IF, "預期 'if'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'");
    ASTNode *then_body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt();
    ASTNode *else_body = NULL;
    if (cur_tok.type == TK_ELSE) { next_token(); else_body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt(); }
    ASTNode *n = new_node(AST_IF); n->cond = cond; n->then_body = then_body; n->else_body = else_body; return n;
}

static ASTNode* parse_while_stmt() {
    expect(TK_WHILE, "預期 'while'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'");
    ASTNode *n = new_node(AST_WHILE); n->cond = cond; n->body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt(); return n;
}

static ASTNode* parse_for_stmt() {
    expect(TK_FOR, "預期 'for'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *init = NULL, *cond = NULL, *update = NULL;
    if (cur_tok.type != TK_SEMI) {
        if (is_type_start()) { init = parse_decl_stmt(0); sym_add(init->name, init->ty, init->array_len, init->struct_id); }
        else { init = new_node(AST_EXPR_STMT); init->left = parse_expr(); }
    }
    expect(TK_SEMI, "預期 ';'");
    if (cur_tok.type != TK_SEMI) cond = parse_expr();
    expect(TK_SEMI, "預期 ';'");
    if (cur_tok.type != TK_RPAREN) { update = parse_expr(); }
    expect(TK_RPAREN, "預期 ')'");
    ASTNode *n = new_node(AST_FOR); n->init = init; n->cond = cond; n->update = update;
    n->body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt(); return n;
}

static ASTNode* parse_switch_stmt() {
    expect(TK_SWITCH, "預期 'switch'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'"); expect(TK_LBRACE, "預期 '{'");
    ASTNode *case_head = NULL, *case_tail = NULL;
    while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
        ASTNode *c = new_node(AST_CASE);
        if (cur_tok.type == TK_CASE) { next_token(); c->val = cur_tok.val; c->is_default = 0; next_token(); expect(':', "預期 ':'"); }
        else if (cur_tok.type == TK_DEFAULT) { next_token(); c->is_default = 1; expect(':', "預期 ':'"); }
        ASTNode *head = NULL, *tail = NULL;
        while (cur_tok.type != TK_CASE && cur_tok.type != TK_DEFAULT && cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
            ASTNode *stmt = parse_stmt();
            if (!head) head = tail = stmt; else { tail->next = stmt; tail = stmt; }
        }
        c->left = head;
        if (!case_head) case_head = case_tail = c; else { case_tail->next = c; case_tail = c; }
    }
    expect(TK_RBRACE, "預期 '}'");
    ASTNode *sw = new_node(AST_SWITCH); sw->cond = cond; sw->left = case_head; return sw;
}

static ASTNode* parse_do_stmt() {
    expect(TK_DO, "預期 'do'");
    ASTNode *body = (cur_tok.type == TK_LBRACE) ? parse_block() : parse_stmt();
    expect(TK_WHILE, "預期 'while'"); expect(TK_LPAREN, "預期 '('");
    ASTNode *cond = parse_expr(); expect(TK_RPAREN, "預期 ')'"); expect(TK_SEMI, "預期 ';'");
    ASTNode *n = new_node(AST_DO); n->cond = cond; n->body = body; return n;
}

static ASTNode* parse_stmt() {
    if (cur_tok.type == TK_TYPEDEF) { ASTNode *n = parse_typedef_stmt(); if (n) return n; ASTNode *e = new_node(AST_EXPR_STMT); e->left = NULL; return e; }
    if (cur_tok.type == TK_STRUCT && is_struct_def_ahead()) { parse_struct_decl_or_def(); ASTNode *e = new_node(AST_EXPR_STMT); e->left = NULL; return e; }
    if (is_type_start()) { ASTNode *n = parse_decl_stmt(1); sym_add(n->name, n->ty, n->array_len, n->struct_id); return n; }
    if (cur_tok.type == TK_LBRACE) { ASTNode *n = new_node(AST_BLOCK); n->left = parse_block(); return n; }
    if (cur_tok.type == TK_IF) return parse_if_stmt();
    if (cur_tok.type == TK_SWITCH) return parse_switch_stmt();
    if (cur_tok.type == TK_DO) return parse_do_stmt();
    if (cur_tok.type == TK_WHILE) return parse_while_stmt();
    if (cur_tok.type == TK_FOR) return parse_for_stmt();
    if (cur_tok.type == TK_BREAK) { next_token(); ASTNode *n = new_node(AST_BREAK); expect(TK_SEMI, "預期 ';'"); return n; }
    if (cur_tok.type == TK_CONTINUE) { next_token(); ASTNode *n = new_node(AST_CONTINUE); expect(TK_SEMI, "預期 ';'"); return n; }
    if (cur_tok.type == TK_RETURN) {
        next_token(); ASTNode *n = new_node(AST_RETURN);
        if (cur_tok.type == TK_SEMI) { n->left = NULL; n->ty = TY_VOID; next_token(); }
        else { n->left = parse_expr(); n->ty = current_func_ret; expect(TK_SEMI, "預期 ';'"); }
        return n;
    }
    if (cur_tok.type == TK_IDENT || cur_tok.type == TK_MUL) {
        char *s_p = p; Token s_t = cur_tok; int s_l = cur_line, s_c = cur_col;
        ASTNode *cand = parse_unary();
        int is_assign = is_lvalue_node(cand) && (cur_tok.type == TK_ASSIGN || cur_tok.type == TK_PLUSEQ || cur_tok.type == TK_MINUSEQ || cur_tok.type == TK_MULEQ || cur_tok.type == TK_DIVEQ || cur_tok.type == TK_MODEQ);
        p = s_p; cur_tok = s_t; cur_line = s_l; cur_col = s_c;
        if (is_assign) {
            ASTNode *lv = parse_lvalue(); ASTNode *n = new_node(AST_ASSIGN);
            n->left = lv; n->ty = lv->ty; n->op = cur_tok.type; next_token(); n->right = parse_expr(); expect(TK_SEMI, "預期 ';'"); return n;
        }
        ASTNode *n = new_node(AST_EXPR_STMT); n->left = parse_expr(); expect(TK_SEMI, "預期 ';'"); return n;
    }
    error("未知的陳述式"); return NULL;
}

static ASTNode* parse_func() {
    CType ret_ty = parse_type_allow_void(1);
    int ret_struct_id = last_struct_id;
    ASTNode *func = new_node(AST_FUNC);
    func->ty = ret_ty; strcpy(func->name, cur_tok.name);
    expect(TK_IDENT, "預期函數名稱"); expect(TK_LPAREN, "預期 '('");

    sym_reset();
    ASTNode *param_head = NULL, *param_tail = NULL;
    CType param_types[16]; int param_struct_ids[16]; int param_cnt = 0; int is_vararg = 0;
    if (cur_tok.type != TK_RPAREN && cur_tok.type != TK_VOID) {
        while (1) {
            if (cur_tok.type == TK_ELLIPSIS) { is_vararg = 1; next_token(); break; }
            CType pty = parse_type(); ASTNode *param = new_node(AST_DECL);
            if (cur_tok.type == TK_IDENT) { strcpy(param->name, cur_tok.name); next_token(); }
            else sprintf(param->name, ".unnamed%d", param_cnt);
            param->ty = pty; param->array_len = 0; param->struct_id = last_struct_id;
            param_types[param_cnt] = pty; param_struct_ids[param_cnt++] = last_struct_id;
            sym_add(param->name, pty, 0, param->struct_id);
            if (!param_head) param_head = param_tail = param; else { param_tail->next = param; param_tail = param; }
            if (cur_tok.type == TK_COMMA) { next_token(); continue; }
            break;
        }
    } else if (cur_tok.type == TK_VOID) next_token();
    expect(TK_RPAREN, "預期 ')'");
    func_add(func->name, ret_ty, ret_struct_id, param_types, param_struct_ids, param_cnt, is_vararg);
    func->val = is_vararg;
    if (cur_tok.type == TK_SEMI) { next_token(); func->left = param_head; func->is_decl = 1; return func; }
    current_func_ret = ret_ty; func->left = param_head; func->right = parse_block(); func->is_decl = 0; return func;
}

ASTNode* parse_program(void) {
    ASTNode *head = NULL, *tail = NULL;
    while (cur_tok.type != TK_EOF) {
        int is_extern = 0;
        if (cur_tok.type == TK_EXTERN) { is_extern = 1; next_token(); }
        if (cur_tok.type == TK_STRUCT && is_struct_def_ahead()) { parse_struct_decl_or_def(); continue; }
        if (cur_tok.type == TK_TYPEDEF) { parse_typedef_stmt(); continue; }
        if (cur_tok.type == TK_ENUM) {
            next_token();
            if (cur_tok.type == TK_IDENT) next_token();
            expect(TK_LBRACE, "預期 '{'");
            int enum_val = 0;
            while (cur_tok.type != TK_RBRACE && cur_tok.type != TK_EOF) {
                strcpy(enum_tab[enum_cnt].name, cur_tok.name); next_token();
                if (cur_tok.type == TK_ASSIGN) { next_token(); if (cur_tok.type == TK_MINUS) { next_token(); enum_val = -cur_tok.val; } else enum_val = cur_tok.val; next_token(); }
                enum_tab[enum_cnt++].val = enum_val++;
                if (cur_tok.type == TK_COMMA) next_token();
            }
            expect(TK_RBRACE, "預期 '}'"); expect(TK_SEMI, "預期 ';'");
            continue;
        }
        if (is_type_start() && !is_func_def_ahead()) {
            ASTNode *g = parse_decl_stmt(1);
            g->type = AST_GLOBAL; g->is_decl = is_extern;
            global_add(g->name, g->ty, g->array_len, g->struct_id);
            if (!head) head = tail = g; else { tail->next = g; tail = g; }
            continue;
        }
        ASTNode *func = parse_func();
        func->is_decl = func->is_decl || is_extern;
        if (!head) head = tail = func; else { tail->next = func; tail = func; }
    }
    return head;
}

--- START OF FILE codegen.h ---

#ifndef C0C_CODEGEN_H
#define C0C_CODEGEN_H

#include "ast.h"
#include <stdio.h>

void gen_llvm_ir(ASTNode *funcs, FILE *out);

#endif

--- START OF FILE codegen.c ---

#include "codegen.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    VT_I1, VT_I8, VT_I16, VT_I32, VT_I64, VT_F32, VT_F64, VT_PTR
} ValueType;

typedef struct {
    char *reg; ValueType vt; CType cty;
} Value;

typedef struct {
    char name[64]; CType ret; int ret_struct_id; CType params[16]; int param_struct_id[16]; int param_cnt; int is_vararg;
} FuncSig;

static int reg_cnt = 0, label_cnt = 0, string_cnt = 0;
static char string_table[100][256];
static ASTNode *current_params = NULL;
static CType current_ret_ty = TY_INT;
static int break_label_stack[128], continue_label_stack[128], loop_depth = 0;
static int switch_break_stack[128], switch_depth = 0;
static FILE *out_fp = NULL;
static FuncSig func_sigs[128];
static int func_sig_cnt = 0, var_id = 0;

typedef struct { char name[64], ir[64]; CType ty; int array_len, struct_id, is_global; } VarSlot;
static VarSlot var_slots[512];
static int var_cnt = 0;
static VarSlot global_slots[256];
static int global_cnt = 0;
static int scope_marks[128], scope_depth = 0;

static int add_string_literal(const char *s) {
    if (string_cnt >= 100) error("字串常數過多");
    strcpy(string_table[string_cnt], s); return string_cnt++;
}

static void build_llvm_string(const char *src, char *dst, int *out_len) {
    int out = 0, len = 0; const unsigned char *p = (const unsigned char*)src;
    while (*p) {
        unsigned char c = *p++;
        if (c == '\n') out += sprintf(dst + out, "\\0A");
        else if (c == '\t') out += sprintf(dst + out, "\\09");
        else if (c == '\r') out += sprintf(dst + out, "\\0D");
        else if (c == '\\') out += sprintf(dst + out, "\\5C");
        else if (c == '\"') out += sprintf(dst + out, "\\22");
        else if (c < 32 || c >= 127) out += sprintf(dst + out, "\\%02X", c);
        else { dst[out++] = (char)c; dst[out] = '\0'; }
        len++;
    }
    strcat(dst, "\\00"); len++; if (out_len) *out_len = len;
}

static int is_ptr(CType ty) {
    return ty == TY_INT_PTR || ty == TY_UINT_PTR || ty == TY_SHORT_PTR || ty == TY_USHORT_PTR ||
           ty == TY_LONG_PTR || ty == TY_ULONG_PTR || ty == TY_CHAR_PTR || ty == TY_UCHAR_PTR ||
           ty == TY_FLOAT_PTR || ty == TY_DOUBLE_PTR || ty == TY_STRUCT_PTR;
}
static int is_float(CType ty) { return ty == TY_FLOAT || ty == TY_DOUBLE; }
static int is_unsigned(CType ty) { return ty == TY_UCHAR || ty == TY_USHORT || ty == TY_UINT || ty == TY_ULONG; }
static CType base_of(CType ty) {
    if (ty == TY_CHAR_PTR) return TY_CHAR; if (ty == TY_UCHAR_PTR) return TY_UCHAR;
    if (ty == TY_SHORT_PTR) return TY_SHORT; if (ty == TY_USHORT_PTR) return TY_USHORT;
    if (ty == TY_INT_PTR) return TY_INT; if (ty == TY_UINT_PTR) return TY_UINT;
    if (ty == TY_LONG_PTR) return TY_LONG; if (ty == TY_ULONG_PTR) return TY_ULONG;
    if (ty == TY_FLOAT_PTR) return TY_FLOAT; if (ty == TY_DOUBLE_PTR) return TY_DOUBLE;
    if (ty == TY_STRUCT_PTR) return TY_STRUCT; return TY_INT;
}
static const char* llvm_type(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return "i8"; if (ty == TY_SHORT || ty == TY_USHORT) return "i16";
    if (ty == TY_INT || ty == TY_UINT) return "i32"; if (ty == TY_LONG || ty == TY_ULONG) return "i64";
    if (ty == TY_FLOAT) return "float"; if (ty == TY_DOUBLE) return "double";
    if (ty == TY_VOID) return "void"; return "ptr";
}
static const char* llvm_elem_type(CType ty) {
    CType base = is_ptr(ty) ? base_of(ty) : ty;
    if (base == TY_CHAR || base == TY_UCHAR) return "i8"; if (base == TY_SHORT || base == TY_USHORT) return "i16";
    if (base == TY_INT || base == TY_UINT) return "i32"; if (base == TY_LONG || base == TY_ULONG) return "i64";
    if (base == TY_FLOAT) return "float"; if (base == TY_DOUBLE) return "double"; return "i8";
}

static Value gen_cond(ASTNode *node); static Value gen_expr(ASTNode *node); static Value gen_lvalue_addr(ASTNode *node);
static int struct_size(int struct_id); static void global_add(const char *name, CType ty, int array_len, int struct_id);
static int is_zero_literal(ASTNode *n) { return n && n->type == AST_NUM && n->val == 0; }
static void emit_int_const(CType ty, long long v) { fprintf(out_fp, "%s %lld", llvm_type(ty), v); }
static void emit_float_const(CType ty, double v) { fprintf(out_fp, "%s %.17g", (ty == TY_DOUBLE) ? "double" : "float", v); }

static void emit_global_scalar(ASTNode *g) {
    if (g->ty == TY_STRUCT) { fprintf(out_fp, "@%s = global[%d x i8] zeroinitializer\n", g->name, struct_size(g->struct_id)); return; }
    fprintf(out_fp, "@%s = global %s ", g->name, llvm_type(g->ty));
    if (g->init_kind == 0) {
        if (is_ptr(g->ty)) fprintf(out_fp, "null\n"); else if (is_float(g->ty)) fprintf(out_fp, "0.0\n"); else fprintf(out_fp, "0\n"); return;
    }
    ASTNode *v = g->left;
    if (is_ptr(g->ty)) {
        if (is_zero_literal(v)) fprintf(out_fp, "null\n");
        else if ((g->ty == TY_CHAR_PTR || g->ty == TY_UCHAR_PTR) && v->type == AST_STR) {
            int id = add_string_literal(v->str_val), len = (int)strlen(v->str_val) + 1;
            fprintf(out_fp, "getelementptr ([%d x i8], ptr @.str.%d, i32 0, i32 0)\n", len, id);
        } else error("全域指標初始化只支援 0 或字串");
        return;
    }
    if (is_float(g->ty)) emit_float_const(g->ty, (v->type == AST_FLOAT) ? v->fval : (double)v->val);
    else emit_int_const(g->ty, (long long)(v->type == AST_FLOAT ? (long long)v->fval : v->val));
    fprintf(out_fp, "\n");
}

static void emit_global_array(ASTNode *g) {
    CType elem = base_of(g->ty);
    if (elem == TY_STRUCT) { fprintf(out_fp, "@%s = global[%d x i8] zeroinitializer\n", g->name, g->array_len * struct_size(g->struct_id)); return; }
    if (g->init_kind == 3 && (elem == TY_CHAR || elem == TY_UCHAR)) {
        char llvm_str[2048] = {0}; int len = 0; build_llvm_string(g->str_val, llvm_str, &len);
        while (len < g->array_len) { strcat(llvm_str, "\\00"); len++; }
        fprintf(out_fp, "@%s = global [%d x i8] c\"%s\"\n", g->name, g->array_len, llvm_str); return;
    }
    fprintf(out_fp, "@%s = global [%d x %s] ", g->name, g->array_len, llvm_elem_type(g->ty));
    if (g->init_kind == 0) { fprintf(out_fp, "zeroinitializer\n"); return; }
    fprintf(out_fp, "["); ASTNode *cur = g->left;
    for (int i = 0; i < g->array_len; i++) {
        if (i > 0) fprintf(out_fp, ", ");
        if (cur) {
            if (is_float(elem)) emit_float_const(elem, (cur->type == AST_FLOAT) ? cur->fval : (double)cur->val);
            else emit_int_const(elem, (long long)(cur->type == AST_FLOAT ? (long long)cur->fval : cur->val));
            cur = cur->next;
        } else {
            if (is_float(elem)) emit_float_const(elem, 0.0); else emit_int_const(elem, 0);
        }
    }
    fprintf(out_fp, "]\n");
}

static void emit_globals(ASTNode *nodes) {
    for (ASTNode *n = nodes; n; n = n->next) {
        if (n->type != AST_GLOBAL) continue;
        global_add(n->name, n->ty, n->array_len, n->struct_id);
        if (n->is_decl) fprintf(out_fp, "@%s = external global %s\n", n->name, llvm_type(n->ty));
        else if (n->array_len > 0) emit_global_array(n); else emit_global_scalar(n);
    }
}

static int is_param(ASTNode *params, const char *name) {
    for (ASTNode *pnode = params; pnode; pnode = pnode->next) if (strcmp(pnode->name, name) == 0) return 1; return 0;
}
static FuncSig* find_func_sig(const char *name) {
    for (int i = 0; i < func_sig_cnt; i++) if (strcmp(func_sigs[i].name, name) == 0) return &func_sigs[i]; return NULL;
}
static void var_push_scope(void) { scope_marks[scope_depth++] = var_cnt; }
static void var_pop_scope(void) { if (scope_depth > 0) var_cnt = scope_marks[--scope_depth]; }

static void var_add(const char *name, const char *ir, CType ty, int array_len, int struct_id, int is_global) {
    if (var_cnt >= 512) error("變數表已滿");
    strcpy(var_slots[var_cnt].name, name); strcpy(var_slots[var_cnt].ir, ir);
    var_slots[var_cnt].ty = ty; var_slots[var_cnt].array_len = array_len;
    var_slots[var_cnt].struct_id = struct_id; var_slots[var_cnt].is_global = is_global; var_cnt++;
}

static VarSlot* var_find(const char *name) {
    for (int i = var_cnt - 1; i >= 0; i--) if (strcmp(var_slots[i].name, name) == 0) return &var_slots[i];
    for (int i = global_cnt - 1; i >= 0; i--) if (strcmp(global_slots[i].name, name) == 0) return &global_slots[i];
    error("找不到變數宣告"); return NULL;
}

static void global_add(const char *name, CType ty, int array_len, int struct_id) {
    if (global_cnt >= 256) error("全域變數表已滿");
    strcpy(global_slots[global_cnt].name, name); strcpy(global_slots[global_cnt].ir, name);
    global_slots[global_cnt].ty = ty; global_slots[global_cnt].array_len = array_len;
    global_slots[global_cnt].struct_id = struct_id; global_slots[global_cnt].is_global = 1; global_cnt++;
}

static const char* slot_prefix(VarSlot *slot) { return slot->is_global ? "@" : "%"; }
static char* slot_ref(VarSlot *slot) { char *res = malloc(64); sprintf(res, "%s%s", slot_prefix(slot), slot->ir); return res; }
static int struct_size(int struct_id) { return (struct_id >= 0 && struct_id < g_struct_def_cnt) ? g_struct_defs[struct_id].size : 0; }
static int elem_size(CType ptr_ty, int struct_id) {
    if (!is_ptr(ptr_ty)) return 0; CType b = base_of(ptr_ty);
    if (b == TY_CHAR || b == TY_UCHAR) return 1; if (b == TY_SHORT || b == TY_USHORT) return 2;
    if (b == TY_INT || b == TY_UINT || b == TY_FLOAT) return 4;
    if (b == TY_LONG || b == TY_ULONG || b == TY_DOUBLE || is_ptr(b)) return 8;
    if (b == TY_STRUCT) return struct_size(struct_id); return 0;
}

static void build_func_sigs(ASTNode *funcs) {
    func_sig_cnt = 0;
    for (ASTNode *f = funcs; f; f = f->next) {
        if (f->type != AST_FUNC) continue;
        FuncSig *sig = &func_sigs[func_sig_cnt++];
        strcpy(sig->name, f->name); sig->ret = f->ty; sig->ret_struct_id = f->struct_id;
        sig->is_vararg = f->val; sig->param_cnt = 0;
        for (ASTNode *p = f->left; p && sig->param_cnt < 16; p = p->next) {
            sig->params[sig->param_cnt] = p->ty; sig->param_struct_id[sig->param_cnt++] = p->struct_id;
        }
    }
}

static int func_has_def(ASTNode *funcs, const char *name) {
    for (ASTNode *f = funcs; f; f = f->next) if (f->type == AST_FUNC && !f->is_decl && strcmp(f->name, name) == 0) return 1; return 0;
}

static ValueType vt_from_ctype(CType ty) {
    if (ty == TY_CHAR || ty == TY_UCHAR) return VT_I8; if (ty == TY_SHORT || ty == TY_USHORT) return VT_I16;
    if (ty == TY_INT || ty == TY_UINT) return VT_I32; if (ty == TY_LONG || ty == TY_ULONG) return VT_I64;
    if (ty == TY_FLOAT) return VT_F32; if (ty == TY_DOUBLE) return VT_F64; if (is_ptr(ty)) return VT_PTR; return VT_I32;
}

static const char* llvm_type_from_vt(ValueType vt) {
    if (vt == VT_I1) return "i1"; if (vt == VT_I8) return "i8"; if (vt == VT_I16) return "i16";
    if (vt == VT_I32) return "i32"; if (vt == VT_I64) return "i64";
    if (vt == VT_F32) return "float"; if (vt == VT_F64) return "double"; return "ptr";
}

static Value value_from_raw(char *reg, ValueType vt, CType cty) { Value v = {reg, vt, cty}; return v; }
static Value value_from_ctype(char *reg, CType ty) { return value_from_raw(reg, vt_from_ctype(ty), ty); }

static Value cast_value(Value v, CType to) {
    if (v.reg == NULL) error("使用了 void 表達式");
    ValueType from_vt = v.vt; ValueType to_vt = vt_from_ctype(to);
    
    if (from_vt == VT_PTR && to_vt != VT_PTR) {
        int r = reg_cnt++;
        fprintf(out_fp, "  %%%d = ptrtoint ptr %s to %s\n", r, v.reg, llvm_type_from_vt(to_vt));
        free(v.reg);
        char *res = malloc(64); sprintf(res, "%%%d", r);
        v = value_from_raw(res, to_vt, to);
        from_vt = to_vt;
    }
    if (v.cty == to && v.vt != VT_I1) return v;
    
    if (is_ptr(to)) {
        if (v.vt == VT_PTR) { v.cty = to; return v; }
        if (v.vt == VT_I32 || v.vt == VT_I64) {
            int r = reg_cnt++;
            fprintf(out_fp, "  %%%d = inttoptr %s %s to ptr\n", r, llvm_type_from_vt(v.vt), v.reg);
            free(v.reg);
            char *res = malloc(64); sprintf(res, "%%%d", r);
            return value_from_ctype(res, to);
        }
        error("不支援轉換為指標");
    }
    
    int r = reg_cnt++;
    if (to_vt == VT_F32 || to_vt == VT_F64) {
        const char *to_ty = (to_vt == VT_F64) ? "double" : "float";
        if (from_vt == VT_F32 || from_vt == VT_F64) fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, (to_vt == VT_F64 && from_vt == VT_F32) ? "fpext" : "fptrunc", (from_vt == VT_F64) ? "double" : "float", v.reg, to_ty);
        else fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, is_unsigned(v.cty) ? "uitofp" : "sitofp", llvm_type_from_vt(from_vt), v.reg, to_ty);
    } else {
        const char *to_ty = llvm_type(to);
        if (from_vt == VT_F32 || from_vt == VT_F64) fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, is_unsigned(to) ? "fptoui" : "fptosi", (from_vt == VT_F64) ? "double" : "float", v.reg, to_ty);
        else {
            int fb = (from_vt == VT_I1) ? 1 : (from_vt == VT_I8) ? 8 : (from_vt == VT_I16) ? 16 : (from_vt == VT_I64) ? 64 : 32;
            int tb = (to_vt == VT_I8) ? 8 : (to_vt == VT_I16) ? 16 : (to_vt == VT_I64) ? 64 : 32;
            if (fb == tb) { v.cty = to; return v; }
            else if (fb < tb) fprintf(out_fp, "  %%%d = %s %s %s to %s\n", r, (from_vt == VT_I1 || is_unsigned(v.cty)) ? "zext" : "sext", llvm_type_from_vt(from_vt), v.reg, to_ty);
            else fprintf(out_fp, "  %%%d = trunc %s %s to %s\n", r, llvm_type_from_vt(from_vt), v.reg, to_ty);
        }
    }
    free(v.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, to);
}

static Value to_i1(Value v) {
    if (v.vt == VT_I1) return v; if (v.reg == NULL) error("使用了 void 表達式");
    int r = reg_cnt++;
    if (v.vt == VT_PTR) fprintf(out_fp, "  %%%d = icmp ne ptr %s, null\n", r, v.reg);
    else if (v.vt == VT_F32 || v.vt == VT_F64) fprintf(out_fp, "  %%%d = fcmp one %s %s, 0.0\n", r, (v.vt == VT_F64) ? "double" : "float", v.reg);
    else fprintf(out_fp, "  %%%d = icmp ne %s %s, 0\n", r, llvm_type(v.cty), v.reg);
    free(v.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_I1, TY_INT);
}

static Value to_i32(Value v) { return cast_value(v, TY_INT); }

static Value gen_lvalue_addr(ASTNode *node) {
    if (node->type == AST_VAR) {
        VarSlot *slot = var_find(node->name); const char *pref = slot_prefix(slot);
        if (slot->array_len > 0) {
            int r = reg_cnt++;
            if (base_of(slot->ty) == TY_STRUCT) fprintf(out_fp, "  %%%d = getelementptr [%d x i8], ptr %s%s, i32 0, i32 0\n", r, slot->array_len * struct_size(slot->struct_id), pref, slot->ir);
            else fprintf(out_fp, "  %%%d = getelementptr[%d x %s], ptr %s%s, i32 0, i32 0\n", r, slot->array_len, llvm_elem_type(slot->ty), pref, slot->ir);
            char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, slot->ty);
        }
        return value_from_raw(slot_ref(slot), VT_PTR, slot->ty);
    }
    if (node->type == AST_DEREF) return gen_expr(node->left);
    if (node->type == AST_INDEX) {
        ASTNode *base = node->left; Value base_ptr;
        if (base->type == AST_VAR) {
            VarSlot *slot = var_find(base->name); const char *pref = slot_prefix(slot);
            if (slot->array_len > 0) {
                int r = reg_cnt++;
                if (base_of(slot->ty) == TY_STRUCT) fprintf(out_fp, "  %%%d = getelementptr[%d x i8], ptr %s%s, i32 0, i32 0\n", r, slot->array_len * struct_size(slot->struct_id), pref, slot->ir);
                else fprintf(out_fp, "  %%%d = getelementptr [%d x %s], ptr %s%s, i32 0, i32 0\n", r, slot->array_len, llvm_elem_type(slot->ty), pref, slot->ir);
                char *res = malloc(64); sprintf(res, "%%%d", r); base_ptr = value_from_ctype(res, slot->ty);
            } else base_ptr = gen_expr(base);
        } else base_ptr = gen_expr(base);
        Value idx = to_i32(gen_expr(node->right));
        int r = reg_cnt++;
        fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r, llvm_elem_type(base->ty), base_ptr.reg, idx.reg);
        free(base_ptr.reg); free(idx.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_INT_PTR);
    }
    if (node->type == AST_MEMBER) {
        Value base_ptr = node->op ? gen_expr(node->left) : gen_lvalue_addr(node->left);
        int r = reg_cnt++;
        fprintf(out_fp, "  %%%d = getelementptr i8, ptr %s, i32 %d\n", r, base_ptr.reg, node->val);
        free(base_ptr.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_INT_PTR);
    }
    error("不支援的取址"); return value_from_raw(NULL, VT_PTR, TY_INT_PTR);
}

static Value gen_expr(ASTNode *node) {
    if (node->type == AST_NUM) { char *res = malloc(64); sprintf(res, "%d", node->val); return value_from_ctype(res, node->ty); }
    if (node->type == AST_FLOAT) { char *res = malloc(64); if (node->ty == TY_FLOAT) snprintf(res, 64, "%.9f", node->fval); else snprintf(res, 64, "%.17f", node->fval); return value_from_ctype(res, node->ty); }
    if (node->type == AST_STR) { int id = add_string_literal(node->str_val), len = (int)strlen(node->str_val) + 1, r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr ([%d x i8], ptr @.str.%d, i32 0, i32 0)\n", r, len, id); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_CHAR_PTR); }
    if (node->type == AST_VAR) {
        VarSlot *slot = var_find(node->name); const char *pref = slot_prefix(slot);
        if (slot->array_len > 0) {
            int r = reg_cnt++;
            if (base_of(slot->ty) == TY_STRUCT) fprintf(out_fp, "  %%%d = getelementptr [%d x i8], ptr %s%s, i32 0, i32 0\n", r, slot->array_len * struct_size(slot->struct_id), pref, slot->ir);
            else fprintf(out_fp, "  %%%d = getelementptr[%d x %s], ptr %s%s, i32 0, i32 0\n", r, slot->array_len, llvm_elem_type(slot->ty), pref, slot->ir);
            char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, slot->ty);
        }
        if (slot->ty == TY_STRUCT) return value_from_raw(slot_ref(slot), VT_PTR, TY_STRUCT_PTR);
        int r = reg_cnt++; fprintf(out_fp, "  %%%d = load %s, ptr %s%s\n", r, llvm_type(slot->ty), pref, slot->ir);
        char *res = malloc(64); sprintf(res, "%%%d", r); return is_ptr(slot->ty) ? value_from_raw(res, VT_PTR, slot->ty) : value_from_ctype(res, slot->ty);
    }
    if (node->type == AST_CALL) {
        Value raw_vals[16], final_vals[16]; CType call_types[16]; int arg_count = 0;
        for (ASTNode *arg = node->left; arg; arg = arg->next) { raw_vals[arg_count] = gen_expr(arg); call_types[arg_count] = arg->ty; arg_count++; }
        FuncSig *sig = find_func_sig(node->name);
        int is_vararg = (sig && sig->is_vararg) || (strcmp(node->name, "printf") == 0);
        const char *ret_ty = (sig && sig->ret == TY_VOID) ? "void" : llvm_type(sig ? sig->ret : TY_INT);
        for (int i = 0; i < arg_count; i++) {
            if (is_vararg && i >= (sig ? sig->param_cnt : 0)) {
                CType ct = call_types[i];
                if (ct == TY_FLOAT) ct = TY_DOUBLE;
                if (ct == TY_CHAR || ct == TY_UCHAR || ct == TY_SHORT || ct == TY_USHORT) ct = TY_INT;
                final_vals[i] = is_ptr(ct) ? raw_vals[i] : cast_value(raw_vals[i], ct); call_types[i] = ct;
            } else if (sig && i < sig->param_cnt) {
                CType pt = sig->params[i];
                final_vals[i] = is_ptr(pt) ? (is_ptr(call_types[i]) ? raw_vals[i] : cast_value(raw_vals[i], pt)) : cast_value(raw_vals[i], pt);
                call_types[i] = pt;
            } else final_vals[i] = raw_vals[i];
        }
        int r = -1;
        if (is_vararg) {
            if (!sig || sig->ret != TY_VOID) { r = reg_cnt++; fprintf(out_fp, "  %%%d = call %s (", r, ret_ty); }
            else fprintf(out_fp, "  call void (");
            for (int i = 0; i < (sig ? sig->param_cnt : 0); i++) fprintf(out_fp, "%s%s", (i > 0) ? ", " : "", llvm_type(sig->params[i]));
            fprintf(out_fp, "%s...) @%s(", (sig && sig->param_cnt > 0) ? ", " : "", node->name);
        } else {
            if (sig && sig->ret == TY_VOID) fprintf(out_fp, "  call void @%s(", node->name);
            else { r = reg_cnt++; fprintf(out_fp, "  %%%d = call %s @%s(", r, ret_ty, node->name); }
        }
        for (int i = 0; i < arg_count; i++) {
            if (i > 0) fprintf(out_fp, ", ");
            fprintf(out_fp, "%s %s", is_ptr(call_types[i]) ? "ptr" : llvm_type(call_types[i]), final_vals[i].reg); free(final_vals[i].reg);
        }
        fprintf(out_fp, ")\n");
        if ((!is_vararg && sig && sig->ret == TY_VOID) || (is_vararg && sig && sig->ret == TY_VOID)) return value_from_raw(NULL, VT_I32, TY_INT);
        char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, sig ? sig->ret : TY_INT);
    }
    if (node->type == AST_ADDR) return gen_lvalue_addr(node->left);
    if (node->type == AST_DEREF || node->type == AST_INDEX || node->type == AST_MEMBER) {
        Value addr = (node->type == AST_DEREF) ? gen_expr(node->left) : gen_lvalue_addr(node);
        if (node->ty == TY_STRUCT) return addr;
        int r = reg_cnt++; fprintf(out_fp, "  %%%d = load %s, ptr %s\n", r, llvm_elem_type(node->ty), addr.reg);
        free(addr.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, node->ty);
    }
    if (node->type == AST_BINOP) {
        if (node->op == TK_ANDAND || node->op == TK_OROR) {
            int tmp = reg_cnt++, rhs_label = label_cnt++, short_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  %%%d = alloca i1\n", tmp); Value left_cond = gen_cond(node->left);
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\n", left_cond.reg, (node->op == TK_ANDAND) ? rhs_label : short_label, (node->op == TK_ANDAND) ? short_label : rhs_label);
            free(left_cond.reg); fprintf(out_fp, "L%d:\n", rhs_label); Value right_cond = gen_cond(node->right);
            fprintf(out_fp, "  store i1 %s, ptr %%%d\n  br label %%L%d\nL%d:\n  store i1 %s, ptr %%%d\n  br label %%L%d\nL%d:\n", right_cond.reg, tmp, end_label, short_label, (node->op == TK_ANDAND) ? "0" : "1", tmp, end_label, end_label);
            free(right_cond.reg); int r = reg_cnt++; fprintf(out_fp, "  %%%d = load i1, ptr %%%d\n", r, tmp); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_I1, TY_INT);
        }
        if ((node->op == TK_EQ || node->op == TK_NE) && (is_ptr(node->left->ty) || is_ptr(node->right->ty))) {
            Value l = is_ptr(node->left->ty) ? gen_expr(node->left) : cast_value(gen_expr(node->left), TY_INT_PTR);
            Value r = is_ptr(node->right->ty) ? gen_expr(node->right) : cast_value(gen_expr(node->right), TY_INT_PTR);
            int rcmp = reg_cnt++; fprintf(out_fp, "  %%%d = icmp %s ptr %s, %s\n", rcmp, (node->op == TK_EQ) ? "eq" : "ne", l.reg, r.reg);
            free(l.reg); free(r.reg); char *res = malloc(64); sprintf(res, "%%%d", rcmp); return value_from_raw(res, VT_I1, TY_INT);
        }
        if ((node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE) && (is_ptr(node->left->ty) || is_ptr(node->right->ty))) {
            Value l = is_ptr(node->left->ty) ? gen_expr(node->left) : cast_value(gen_expr(node->left), TY_INT_PTR);
            Value r = is_ptr(node->right->ty) ? gen_expr(node->right) : cast_value(gen_expr(node->right), TY_INT_PTR);
            int rl = reg_cnt++, rr = reg_cnt++, rcmp = reg_cnt++;
            fprintf(out_fp, "  %%%d = ptrtoint ptr %s to i64\n  %%%d = ptrtoint ptr %s to i64\n", rl, l.reg, rr, r.reg);
            fprintf(out_fp, "  %%%d = icmp %s i64 %%%d, %%%d\n", rcmp, (node->op == TK_LT) ? "slt" : (node->op == TK_LE) ? "sle" : (node->op == TK_GT) ? "sgt" : "sge", rl, rr);
            free(l.reg); free(r.reg); char *res = malloc(64); sprintf(res, "%%%d", rcmp); return value_from_raw(res, VT_I1, TY_INT);
        }
        if ((node->op == TK_PLUS || node->op == TK_MINUS) && (is_ptr(node->left->ty) || is_ptr(node->right->ty))) {
            if (is_ptr(node->left->ty) && is_ptr(node->right->ty)) {
                Value l = gen_expr(node->left), r = gen_expr(node->right);
                int rl = reg_cnt++, rr = reg_cnt++, rdiff = reg_cnt++, esz = elem_size(node->left->ty, node->left->struct_id), rdiv = rdiff, rtr = reg_cnt++;
                fprintf(out_fp, "  %%%d = ptrtoint ptr %s to i64\n  %%%d = ptrtoint ptr %s to i64\n  %%%d = sub i64 %%%d, %%%d\n", rl, l.reg, rr, r.reg, rdiff, rl, rr);
                if (esz > 1) { rdiv = reg_cnt++; fprintf(out_fp, "  %%%d = sdiv i64 %%%d, %d\n", rdiv, rdiff, esz); }
                fprintf(out_fp, "  %%%d = trunc i64 %%%d to i32\n", rtr, rdiv); free(l.reg); free(r.reg); char *res = malloc(64); sprintf(res, "%%%d", rtr); return value_from_ctype(res, TY_INT);
            } else {
                ASTNode *ptr_node = is_ptr(node->left->ty) ? node->left : node->right, *int_node = is_ptr(node->left->ty) ? node->right : node->left;
                Value base = gen_expr(ptr_node), idx = to_i32(gen_expr(int_node));
                if (node->op == TK_MINUS && is_ptr(node->left->ty)) {
                    int rneg = reg_cnt++; fprintf(out_fp, "  %%%d = sub i32 0, %s\n", rneg, idx.reg); free(idx.reg); char *neg = malloc(64); sprintf(neg, "%%%d", rneg); idx = value_from_raw(neg, VT_I32, TY_INT);
                }
                int r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r, llvm_elem_type(ptr_node->ty), base.reg, idx.reg);
                free(base.reg); free(idx.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_PTR, TY_INT_PTR);
            }
        }
        CType ct = common_arith_type(node->left->ty, node->right->ty);
        Value left = cast_value(gen_expr(node->left), ct), right = cast_value(gen_expr(node->right), ct);
        int r = reg_cnt++;
        if (is_float(ct)) {
            const char *fty = (ct == TY_DOUBLE) ? "double" : "float";
            if (node->op == '+') fprintf(out_fp, "  %%%d = fadd %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == '-') fprintf(out_fp, "  %%%d = fsub %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == '*') fprintf(out_fp, "  %%%d = fmul %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == '/') fprintf(out_fp, "  %%%d = fdiv %s %s, %s\n", r, fty, left.reg, right.reg);
            if (node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE || node->op == TK_EQ || node->op == TK_NE) {
                fprintf(out_fp, "  %%%d = fcmp %s %s %s, %s\n", r, (node->op == TK_LT) ? "olt" : (node->op == TK_LE) ? "ole" : (node->op == TK_GT) ? "ogt" : (node->op == TK_GE) ? "oge" : (node->op == TK_EQ) ? "oeq" : "one", fty, left.reg, right.reg);
            }
        } else {
            const char *ity = llvm_type(ct); int uns = is_unsigned(ct);
            if (node->op == '+') fprintf(out_fp, "  %%%d = add %s %s, %s\n", r, ity, left.reg, right.reg);
            if (node->op == '-') fprintf(out_fp, "  %%%d = sub %s %s, %s\n", r, ity, left.reg, right.reg);
            if (node->op == '*') fprintf(out_fp, "  %%%d = mul %s %s, %s\n", r, ity, left.reg, right.reg);
            if (node->op == '/') fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r, uns ? "udiv" : "sdiv", ity, left.reg, right.reg);
            if (node->op == TK_MOD) fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r, uns ? "urem" : "srem", ity, left.reg, right.reg);
            if (node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE || node->op == TK_EQ || node->op == TK_NE) {
                fprintf(out_fp, "  %%%d = icmp %s %s %s, %s\n", r, (node->op == TK_LT) ? (uns ? "ult" : "slt") : (node->op == TK_GT) ? (uns ? "ugt" : "sgt") : (node->op == TK_LE) ? (uns ? "ule" : "sle") : (node->op == TK_GE) ? (uns ? "uge" : "sge") : (node->op == TK_EQ) ? "eq" : "ne", ity, left.reg, right.reg);
            }
        }
        free(left.reg); free(right.reg); char *res = malloc(64); sprintf(res, "%%%d", r);
        if (node->op == TK_LT || node->op == TK_GT || node->op == TK_LE || node->op == TK_GE || node->op == TK_EQ || node->op == TK_NE) return value_from_raw(res, VT_I1, TY_INT);
        return value_from_ctype(res, ct);
    }
    if (node->type == AST_UNARY) {
        if (node->op == TK_NOT) { Value val = gen_cond(node->left); int r = reg_cnt++; fprintf(out_fp, "  %%%d = xor i1 %s, 1\n", r, val.reg); free(val.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_raw(res, VT_I1, TY_INT); }
        if (node->op == TK_MINUS || node->op == TK_PLUS) {
            Value val = cast_value(gen_expr(node->left), node->ty);
            if (node->op == TK_PLUS) return val;
            int r = reg_cnt++;
            if (is_float(node->ty)) fprintf(out_fp, "  %%%d = fsub %s 0.0, %s\n", r, (node->ty == TY_DOUBLE) ? "double" : "float", val.reg);
            else fprintf(out_fp, "  %%%d = sub %s 0, %s\n", r, llvm_type(node->ty), val.reg);
            free(val.reg); char *res = malloc(64); sprintf(res, "%%%d", r); return value_from_ctype(res, node->ty);
        }
    }
    if (node->type == AST_CAST) {
        Value val = gen_expr(node->left);
        if (is_ptr(node->ty)) return cast_value(val, node->ty); // cast_value supports pointer casts properly now
        return cast_value(val, node->ty);
    }
    if (node->type == AST_SIZEOF) { char *res = malloc(64); sprintf(res, "%d", node->val); return value_from_ctype(res, TY_INT); }
    if (node->type == AST_INCDEC) {
        int is_inc = (node->op == TK_PLUSPLUS); Value addr = gen_lvalue_addr(node->left);
        const char *ty = llvm_type(node->ty); int r_old = reg_cnt++;
        fprintf(out_fp, "  %%%d = load %s, ptr %s\n", r_old, ty, addr.reg);
        char *old_reg = malloc(64); sprintf(old_reg, "%%%d", r_old); Value old_val = value_from_ctype(old_reg, node->ty);
        int r_new = reg_cnt++;
        if (is_ptr(node->ty)) fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r_new, llvm_elem_type(node->ty), old_val.reg, is_inc ? "1" : "-1");
        else if (is_float(node->ty)) fprintf(out_fp, "  %%%d = %s %s %s, 1.0\n", r_new, is_inc ? "fadd" : "fsub", (node->ty == TY_DOUBLE) ? "double" : "float", old_val.reg);
        else fprintf(out_fp, "  %%%d = %s %s %s, 1\n", r_new, is_inc ? "add" : "sub", ty, old_val.reg);
        char *new_reg = malloc(64); sprintf(new_reg, "%%%d", r_new); Value new_val = value_from_ctype(new_reg, node->ty);
        fprintf(out_fp, "  store %s %s, ptr %s\n", ty, new_val.reg, addr.reg); free(addr.reg);
        if (node->is_prefix) return value_from_ctype(new_val.reg, node->ty);
        free(new_val.reg); return value_from_ctype(old_val.reg, node->ty);
    }
    error("未知的表達式"); return value_from_raw(NULL, VT_I32, TY_INT);
}

static Value gen_cond(ASTNode *node) { Value val = gen_expr(node); if (val.vt == VT_I1) return val; return to_i1(val); }

static void gen_stmt(ASTNode *node) {
    while (node) {
        if (node->type == AST_DECL) {
            char ir_name[64]; snprintf(ir_name, sizeof(ir_name), "v%d", var_id++);
            var_add(node->name, ir_name, node->ty, node->array_len, node->struct_id, 0);
            if (node->array_len > 0) {
                if (base_of(node->ty) == TY_STRUCT) fprintf(out_fp, "  %%%s = alloca [%d x i8]\n", ir_name, node->array_len * struct_size(node->struct_id));
                else {
                    fprintf(out_fp, "  %%%s = alloca [%d x %s]\n", ir_name, node->array_len, llvm_elem_type(node->ty));
                    if (node->init_kind == 2) {
                        int idx = 0;
                        for (ASTNode *cur = node->left; cur && idx < node->array_len; cur = cur->next, idx++) {
                            Value val = gen_expr(cur), store_val = cast_value(val, base_of(node->ty)); int r = reg_cnt++;
                            fprintf(out_fp, "  %%%d = getelementptr [%d x %s], ptr %%%s, i32 0, i32 %d\n  store %s %s, ptr %%%d\n", r, node->array_len, llvm_elem_type(node->ty), ir_name, idx, llvm_elem_type(node->ty), store_val.reg, r); free(store_val.reg);
                        }
                        for (int i = idx; i < node->array_len; i++) {
                            int r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr [%d x %s], ptr %%%s, i32 0, i32 %d\n  store %s 0, ptr %%%d\n", r, node->array_len, llvm_elem_type(node->ty), ir_name, i, llvm_elem_type(node->ty), r);
                        }
                    } else if (node->init_kind == 3) {
                        int len = (int)strlen(node->str_val);
                        for (int i = 0; i < node->array_len; i++) {
                            int r = reg_cnt++; fprintf(out_fp, "  %%%d = getelementptr [%d x i8], ptr %%%s, i32 0, i32 %d\n  store i8 %d, ptr %%%d\n", r, node->array_len, ir_name, i, (i < len) ? (unsigned char)node->str_val[i] : 0, r);
                        }
                    }
                }
            } else if (node->ty == TY_STRUCT) fprintf(out_fp, "  %%%s = alloca [%d x i8]\n", ir_name, struct_size(node->struct_id));
            else if (is_param(current_params, node->name)) {
                fprintf(out_fp, "  %%%s = alloca %s\n  store %s %%%s, ptr %%%s\n", ir_name, llvm_type(node->ty), llvm_type(node->ty), node->name, ir_name);
            } else {
                fprintf(out_fp, "  %%%s = alloca %s\n", ir_name, llvm_type(node->ty));
                if (node->left) { Value val = gen_expr(node->left), store_val = is_ptr(node->ty) ? cast_value(val, node->ty) : cast_value(val, node->ty); fprintf(out_fp, "  store %s %s, ptr %%%s\n", llvm_type(node->ty), store_val.reg, ir_name); free(store_val.reg); }
            }
        } else if (node->type == AST_ASSIGN) {
            Value addr = gen_lvalue_addr(node->left);
            if (node->op == TK_ASSIGN) {
                Value val = gen_expr(node->right), store_val = cast_value(val, node->ty);
                fprintf(out_fp, "  store %s %s, ptr %s\n", llvm_type(node->ty), store_val.reg, addr.reg); free(store_val.reg);
            } else {
                int r_old = reg_cnt++; fprintf(out_fp, "  %%%d = load %s, ptr %s\n", r_old, llvm_type(node->ty), addr.reg);
                char *old_reg = malloc(64); sprintf(old_reg, "%%%d", r_old); Value old_val = value_from_ctype(old_reg, node->ty);
                Value rhs = cast_value(gen_expr(node->right), is_ptr(node->ty) ? TY_INT : node->ty); int r_new = reg_cnt++;
                if (is_ptr(node->ty)) {
                    if (node->op == TK_PLUSEQ) fprintf(out_fp, "  %%%d = getelementptr %s, ptr %s, i32 %s\n", r_new, llvm_elem_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MINUSEQ) { int r_neg = reg_cnt++; fprintf(out_fp, "  %%%d = sub i32 0, %s\n  %%%d = getelementptr %s, ptr %s, i32 %%%d\n", r_neg, rhs.reg, r_new, llvm_elem_type(node->ty), old_val.reg, r_neg); }
                    else error("指標不支援此複合指定");
                } else if (is_float(node->ty)) {
                    const char *fty = (node->ty == TY_DOUBLE) ? "double" : "float";
                    if (node->op == TK_PLUSEQ) fprintf(out_fp, "  %%%d = fadd %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                    else if (node->op == TK_MINUSEQ) fprintf(out_fp, "  %%%d = fsub %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                    else if (node->op == TK_MULEQ) fprintf(out_fp, "  %%%d = fmul %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                    else if (node->op == TK_DIVEQ) fprintf(out_fp, "  %%%d = fdiv %s %s, %s\n", r_new, fty, old_val.reg, rhs.reg);
                } else {
                    if (node->op == TK_PLUSEQ) fprintf(out_fp, "  %%%d = add %s %s, %s\n", r_new, llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MINUSEQ) fprintf(out_fp, "  %%%d = sub %s %s, %s\n", r_new, llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MULEQ) fprintf(out_fp, "  %%%d = mul %s %s, %s\n", r_new, llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_DIVEQ) fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r_new, is_unsigned(node->ty) ? "udiv" : "sdiv", llvm_type(node->ty), old_val.reg, rhs.reg);
                    else if (node->op == TK_MODEQ) fprintf(out_fp, "  %%%d = %s %s %s, %s\n", r_new, is_unsigned(node->ty) ? "urem" : "srem", llvm_type(node->ty), old_val.reg, rhs.reg);
                }
                free(old_val.reg); free(rhs.reg); char *new_reg = malloc(64); sprintf(new_reg, "%%%d", r_new); Value new_val = value_from_ctype(new_reg, node->ty);
                fprintf(out_fp, "  store %s %s, ptr %s\n", llvm_type(node->ty), new_val.reg, addr.reg); free(new_val.reg);
            }
            free(addr.reg);
        } else if (node->type == AST_EXPR_STMT) { if (node->left) { Value val = gen_expr(node->left); if (val.reg) free(val.reg); } }
        else if (node->type == AST_BLOCK) { var_push_scope(); gen_stmt(node->left); var_pop_scope(); }
        else if (node->type == AST_IF) {
            Value cond_val = gen_cond(node->cond);
            int then_label = label_cnt++, else_label = node->else_body ? label_cnt++ : label_cnt++, end_label = node->else_body ? label_cnt++ : else_label;
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\n", cond_val.reg, then_label, else_label); free(cond_val.reg);
            fprintf(out_fp, "L%d:\n", then_label); gen_stmt(node->then_body); fprintf(out_fp, "  br label %%L%d\n", end_label);
            if (node->else_body) { fprintf(out_fp, "L%d:\n", else_label); gen_stmt(node->else_body); fprintf(out_fp, "  br label %%L%d\n", end_label); }
            fprintf(out_fp, "L%d:\n", end_label);
        } else if (node->type == AST_WHILE) {
            int cond_label = label_cnt++, body_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  br label %%L%d\nL%d:\n", cond_label, cond_label); Value cond_val = gen_cond(node->cond);
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\nL%d:\n", cond_val.reg, body_label, end_label, body_label); free(cond_val.reg);
            break_label_stack[loop_depth] = end_label; continue_label_stack[loop_depth] = cond_label; loop_depth++;
            gen_stmt(node->body); fprintf(out_fp, "  br label %%L%d\n", cond_label); loop_depth--;
            fprintf(out_fp, "L%d:\n", end_label);
        } else if (node->type == AST_FOR) {
            if (node->init) gen_stmt(node->init);
            int cond_label = label_cnt++, body_label = label_cnt++, update_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  br label %%L%d\nL%d:\n", cond_label, cond_label);
            if (node->cond) { Value cond_val = gen_cond(node->cond); fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\n", cond_val.reg, body_label, end_label); free(cond_val.reg); }
            else fprintf(out_fp, "  br label %%L%d\n", body_label);
            fprintf(out_fp, "L%d:\n", body_label);
            break_label_stack[loop_depth] = end_label; continue_label_stack[loop_depth] = update_label; loop_depth++;
            gen_stmt(node->body); fprintf(out_fp, "  br label %%L%d\n", update_label); loop_depth--;
            fprintf(out_fp, "L%d:\n", update_label); if (node->update) gen_stmt(node->update); fprintf(out_fp, "  br label %%L%d\nL%d:\n", cond_label, end_label);
        } else if (node->type == AST_DO) {
            int body_label = label_cnt++, cond_label = label_cnt++, end_label = label_cnt++;
            fprintf(out_fp, "  br label %%L%d\nL%d:\n", body_label, body_label);
            break_label_stack[loop_depth] = end_label; continue_label_stack[loop_depth] = cond_label; loop_depth++;
            gen_stmt(node->body); fprintf(out_fp, "  br label %%L%d\n", cond_label); loop_depth--;
            fprintf(out_fp, "L%d:\n", cond_label); Value cond_val = gen_cond(node->cond);
            fprintf(out_fp, "  br i1 %s, label %%L%d, label %%L%d\nL%d:\n", cond_val.reg, body_label, end_label, end_label); free(cond_val.reg);
        } else if (node->type == AST_SWITCH) {
            Value cond_val = cast_value(gen_expr(node->cond), TY_INT);
            int end_label = label_cnt++, case_count = 0; for (ASTNode *c = node->left; c; c = c->next) case_count++;
            int *case_labels = malloc(sizeof(int) * case_count), *case_is_default = malloc(sizeof(int) * case_count), default_label