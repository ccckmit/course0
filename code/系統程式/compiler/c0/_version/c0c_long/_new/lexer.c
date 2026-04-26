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