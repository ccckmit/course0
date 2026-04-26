#include "../include/lexer.h"
#include <stdlib.h>
#include <string.h>

static int is_keyword(const char *s) {
    if (strcmp(s, "def") == 0) return TOKEN_DEF;
    if (strcmp(s, "if") == 0) return TOKEN_IF;
    if (strcmp(s, "else") == 0) return TOKEN_ELSE;
    if (strcmp(s, "while") == 0) return TOKEN_WHILE;
    if (strcmp(s, "return") == 0) return TOKEN_RETURN;
    if (strcmp(s, "pass") == 0) return TOKEN_PASS;
    return TOKEN_IDENT;
}

static char peek(Lexer *lex) {
    if (lex->pos < lex->len) return lex->text[lex->pos];
    return '\0';
}

static char peek_n(Lexer *lex, int n) {
    if (lex->pos + n < lex->len) return lex->text[lex->pos + n];
    return '\0';
}

static char advance(Lexer *lex) {
    char ch = lex->text[lex->pos++];
    if (ch == '\n') {
        lex->line++;
        lex->column = 1;
    } else {
        lex->column++;
    }
    return ch;
}

static void skip_whitespace(Lexer *lex) {
    while (peek(lex) == ' ' || peek(lex) == '\t') {
        advance(lex);
    }
}

static void skip_comment(Lexer *lex) {
    if (peek(lex) == '#') {
        while (peek(lex) && peek(lex) != '\n') {
            advance(lex);
        }
    }
}

static int read_number(Lexer *lex) {
    int value = 0;
    while (isdigit(peek(lex))) {
        value = value * 10 + (advance(lex) - '0');
    }
    return value;
}

static char *read_identifier(Lexer *lex) {
    size_t start = lex->pos;
    while (isalnum(peek(lex)) || peek(lex) == '_') {
        advance(lex);
    }
    size_t len = lex->pos - start;
    char *s = (char *)malloc(len + 1);
    strncpy(s, lex->text + start, len);
    s[len] = '\0';
    return s;
}

static void handle_newline(Lexer *lex) {
    while (peek(lex) == '\n') {
        advance(lex);
    }
    if (lex->token.value) {
        free(lex->token.value);
        lex->token.value = NULL;
    }
    lex->token.type = TOKEN_NEWLINE;
}

static void handle_indent(Lexer *lex) {
    int indent = 0;
    while (peek(lex) == ' ' || peek(lex) == '\t') {
        if (peek(lex) == ' ') indent++;
        else indent += 8;
        advance(lex);
    }
    if (!peek(lex) || peek(lex) == '#') return;
    
    int top = lex->indent_stack[lex->indent_top];
    if (indent > top) {
        lex->indent_stack[++lex->indent_top] = indent;
        lex->token.type = TOKEN_INDENT;
    } else if (indent < top) {
        while (lex->indent_stack[lex->indent_top] > indent) {
            lex->indent_top--;
            lex->token.type = TOKEN_DEDENT;
            return;
        }
    }
}

void lexer_init(Lexer *lex, const char *text) {
    lex->text = text;
    lex->pos = 0;
    lex->len = strlen(text);
    lex->line = 1;
    lex->column = 1;
    lex->indent_top = 0;
    lex->indent_stack[0] = 0;
    lex->token.type = TOKEN_EOF;
    lex->token.value = NULL;
}

Token *lexer_next(Lexer *lex) {
    skip_whitespace(lex);
    
    if (lex->pos >= lex->len) {
        while (lex->indent_top > 0) {
            lex->indent_top--;
            lex->token.type = TOKEN_DEDENT;
            return &lex->token;
        }
        lex->token.type = TOKEN_EOF;
        return &lex->token;
    }
    
    char ch = peek(lex);
    
    if (ch == '\n') {
        handle_newline(lex);
        handle_indent(lex);
        return &lex->token;
    }
    
    if (ch == '#') {
        skip_comment(lex);
        return lexer_next(lex);
    }
    
    if (isdigit(ch)) {
        if (lex->token.value) free(lex->token.value);
        lex->token.value = NULL;
        lex->token.type = TOKEN_NUMBER;
        lex->token.num_value = read_number(lex);
        return &lex->token;
    }
    
    if (isalpha(ch) || ch == '_') {
        char *ident = read_identifier(lex);
        lex->token.type = is_keyword(ident);
        if (lex->token.type == TOKEN_IDENT) {
            lex->token.value = ident;
        } else {
            free(ident);
            lex->token.value = NULL;
        }
        return &lex->token;
    }
    
    if (peek_n(lex, 1) && peek_n(lex, 1) != '\0') {
        char op2[3] = {peek(lex), peek_n(lex, 1), '\0'};
        if (strcmp(op2, "==") == 0) { 
            if (lex->token.value) free(lex->token.value);
            lex->token.value = NULL;
            advance(lex); advance(lex); 
            lex->token.type = TOKEN_EQ; 
            return &lex->token; 
        }
        if (strcmp(op2, "!=") == 0) { 
            if (lex->token.value) free(lex->token.value);
            lex->token.value = NULL;
            advance(lex); advance(lex); 
            lex->token.type = TOKEN_NE; 
            return &lex->token; 
        }
        if (strcmp(op2, "<=") == 0) { 
            if (lex->token.value) free(lex->token.value);
            lex->token.value = NULL;
            advance(lex); advance(lex); 
            lex->token.type = TOKEN_LE; 
            return &lex->token; 
        }
        if (strcmp(op2, ">=") == 0) { 
            if (lex->token.value) free(lex->token.value);
            lex->token.value = NULL;
            advance(lex); advance(lex); 
            lex->token.type = TOKEN_GE; 
            return &lex->token; 
        }
    }
    
    if (lex->token.value) free(lex->token.value);
    lex->token.value = NULL;
    advance(lex);
    switch (ch) {
        case '(': lex->token.type = TOKEN_LPAREN; break;
        case ')': lex->token.type = TOKEN_RPAREN; break;
        case ':': lex->token.type = TOKEN_COLON; break;
        case ',': lex->token.type = TOKEN_COMMA; break;
        case '+': lex->token.type = TOKEN_PLUS; break;
        case '-': lex->token.type = TOKEN_MINUS; break;
        case '*': lex->token.type = TOKEN_MUL; break;
        case '/': lex->token.type = TOKEN_DIV; break;
        case '%': lex->token.type = TOKEN_MOD; break;
        case '=': lex->token.type = TOKEN_ASSIGN; break;
        case '<': lex->token.type = TOKEN_LT; break;
        case '>': lex->token.type = TOKEN_GT; break;
        default: lex->token.type = TOKEN_ERROR; break;
    }
    return &lex->token;
}

void lexer_free(Lexer *lex) {
    if (lex->token.value) free(lex->token.value);
}
