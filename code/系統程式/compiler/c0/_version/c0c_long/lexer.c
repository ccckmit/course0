#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

struct Lexer {
    const char *src;
    const char *filename;
    size_t      pos;
    int         line;
    int         col;
    /* one-token look-ahead */
    int         has_peek;
    Token       peeked;
};

/* ------------------------------------------------------------------ helpers */

static char cur(Lexer *l)  { return l->src[l->pos]; }
static char peek1(Lexer *l){ return l->src[l->pos + 1]; }

static char advance(Lexer *l) {
    char c = l->src[l->pos++];
    if (c == '\n') { l->line++; l->col = 1; }
    else            l->col++;
    return c;
}

static char *strndup_local(const char *s, size_t n) {
    char *p = malloc(n + 1);
    if (!p) { perror("malloc"); exit(1); }
    memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

/* ---------------------------------------------------------------- keywords */

static TokenType keyword_lookup(const char *s) {
    /* Direct strcmp chain — avoids global struct array initialization issues */
    if (strcmp(s, "int")      == 0) return TOK_INT;
    if (strcmp(s, "char")     == 0) return TOK_CHAR;
    if (strcmp(s, "float")    == 0) return TOK_FLOAT;
    if (strcmp(s, "double")   == 0) return TOK_DOUBLE;
    if (strcmp(s, "void")     == 0) return TOK_VOID;
    if (strcmp(s, "long")     == 0) return TOK_LONG;
    if (strcmp(s, "short")    == 0) return TOK_SHORT;
    if (strcmp(s, "unsigned") == 0) return TOK_UNSIGNED;
    if (strcmp(s, "signed")   == 0) return TOK_SIGNED;
    if (strcmp(s, "if")       == 0) return TOK_IF;
    if (strcmp(s, "else")     == 0) return TOK_ELSE;
    if (strcmp(s, "while")    == 0) return TOK_WHILE;
    if (strcmp(s, "for")      == 0) return TOK_FOR;
    if (strcmp(s, "do")       == 0) return TOK_DO;
    if (strcmp(s, "return")   == 0) return TOK_RETURN;
    if (strcmp(s, "break")    == 0) return TOK_BREAK;
    if (strcmp(s, "continue") == 0) return TOK_CONTINUE;
    if (strcmp(s, "switch")   == 0) return TOK_SWITCH;
    if (strcmp(s, "case")     == 0) return TOK_CASE;
    if (strcmp(s, "default")  == 0) return TOK_DEFAULT;
    if (strcmp(s, "goto")     == 0) return TOK_GOTO;
    if (strcmp(s, "struct")   == 0) return TOK_STRUCT;
    if (strcmp(s, "union")    == 0) return TOK_UNION;
    if (strcmp(s, "enum")     == 0) return TOK_ENUM;
    if (strcmp(s, "typedef")  == 0) return TOK_TYPEDEF;
    if (strcmp(s, "static")   == 0) return TOK_STATIC;
    if (strcmp(s, "extern")   == 0) return TOK_EXTERN;
    if (strcmp(s, "const")    == 0) return TOK_CONST;
    if (strcmp(s, "volatile") == 0) return TOK_VOLATILE;
    if (strcmp(s, "sizeof")   == 0) return TOK_SIZEOF;
    return TOK_IDENT;
}

/* ------------------------------------------------------------------ public */

Lexer *lexer_new(const char *src, const char *filename) {
    Lexer *l = calloc(1, sizeof(Lexer));
    if (!l) { perror("calloc"); exit(1); }
    l->src      = src;
    l->filename = filename;
    l->line     = 1;
    l->col      = 1;
    return l;
}

void lexer_free(Lexer *l) {
    if (l->has_peek) token_free(l->peeked);
    free(l);
}

void token_free(Token t) { free(t.text); }

/* ---------------------------------------------------- skip whitespace & comments */
static void skip_ws(Lexer *l) {
    for (;;) {
        /* whitespace */
        while (cur(l) && isspace((unsigned char)cur(l))) advance(l);
        /* line comment */
        if (cur(l) == '/' && peek1(l) == '/') {
            while (cur(l) && cur(l) != '\n') advance(l);
            continue;
        }
        /* block comment */
        if (cur(l) == '/' && peek1(l) == '*') {
            advance(l); advance(l);
            while (cur(l) && !(cur(l) == '*' && peek1(l) == '/'))
                advance(l);
            if (cur(l)) { advance(l); advance(l); }
            continue;
        }
        break;
    }
}

/* ---------------------------------------------------------- read one token */
static Token read_token(Lexer *l) {
    skip_ws(l);

    Token t;
    t.line = l->line;
    t.col  = l->col;
    t.text = NULL;

    if (!cur(l)) {
        t.type = TOK_EOF;
        t.text = strdup("");
        return t;
    }

    /* --- numbers --- */
    if (isdigit((unsigned char)cur(l)) ||
        (cur(l) == '.' && isdigit((unsigned char)peek1(l)))) {
        size_t start = l->pos;
        int is_float = 0;
        /* hex */
        if (cur(l) == '0' && (peek1(l) == 'x' || peek1(l) == 'X')) {
            advance(l); advance(l);
            while (isxdigit((unsigned char)cur(l))) advance(l);
        } else {
            while (isdigit((unsigned char)cur(l))) advance(l);
            if (cur(l) == '.') { is_float = 1; advance(l); }
            while (isdigit((unsigned char)cur(l))) advance(l);
            if (cur(l) == 'e' || cur(l) == 'E') {
                is_float = 1; advance(l);
                if (cur(l) == '+' || cur(l) == '-') advance(l);
                while (isdigit((unsigned char)cur(l))) advance(l);
            }
        }
        /* suffix */
        while (cur(l) == 'u' || cur(l) == 'U' || cur(l) == 'l' ||
               cur(l) == 'L' || cur(l) == 'f' || cur(l) == 'F') advance(l);
        t.type = is_float ? TOK_FLOAT_LIT : TOK_INT_LIT;
        t.text = strndup_local(l->src + start, l->pos - start);
        return t;
    }

    /* --- char literal --- */
    if (cur(l) == '\'') {
        size_t start = l->pos;
        advance(l);
        if (cur(l) == '\\') { advance(l); advance(l); }
        else advance(l);
        if (cur(l) == '\'') advance(l);
        t.type = TOK_CHAR_LIT;
        t.text = strndup_local(l->src + start, l->pos - start);
        return t;
    }

    /* --- string literal --- */
    if (cur(l) == '"') {
        size_t start = l->pos;
        advance(l);
        while (cur(l) && cur(l) != '"') {
            if (cur(l) == '\\') advance(l);
            if (cur(l)) advance(l);
        }
        if (cur(l) == '"') advance(l);
        t.type = TOK_STRING_LIT;
        t.text = strndup_local(l->src + start, l->pos - start);
        return t;
    }

    /* --- identifiers / keywords --- */
    if (isalpha((unsigned char)cur(l)) || cur(l) == '_') {
        size_t start = l->pos;
        while (isalnum((unsigned char)cur(l)) || cur(l) == '_') advance(l);
        t.text = strndup_local(l->src + start, l->pos - start);
        t.type = keyword_lookup(t.text);
        return t;
    }

    /* --- operators & punctuation --- */
    char c  = advance(l);
    char c2 = cur(l);

#define MAKE(tp, txt) do { t.type = (tp); t.text = strdup(txt); return t; } while(0)
#define ADVANCE_MAKE(tp, txt) do { advance(l); MAKE(tp, txt); } while(0)

    switch (c) {
    case '+':
        if (c2 == '+') ADVANCE_MAKE(TOK_INC, "++");
        if (c2 == '=') ADVANCE_MAKE(TOK_PLUS_ASSIGN, "+=");
        MAKE(TOK_PLUS, "+");
    case '-':
        if (c2 == '-') ADVANCE_MAKE(TOK_DEC, "--");
        if (c2 == '=') ADVANCE_MAKE(TOK_MINUS_ASSIGN, "-=");
        if (c2 == '>') ADVANCE_MAKE(TOK_ARROW, "->");
        MAKE(TOK_MINUS, "-");
    case '*':
        if (c2 == '=') ADVANCE_MAKE(TOK_STAR_ASSIGN, "*=");
        MAKE(TOK_STAR, "*");
    case '/':
        if (c2 == '=') ADVANCE_MAKE(TOK_SLASH_ASSIGN, "/=");
        MAKE(TOK_SLASH, "/");
    case '%':
        if (c2 == '=') ADVANCE_MAKE(TOK_PERCENT_ASSIGN, "%=");
        MAKE(TOK_PERCENT, "%");
    case '&':
        if (c2 == '&') ADVANCE_MAKE(TOK_AND, "&&");
        if (c2 == '=') ADVANCE_MAKE(TOK_AMP_ASSIGN, "&=");
        MAKE(TOK_AMP, "&");
    case '|':
        if (c2 == '|') ADVANCE_MAKE(TOK_OR, "||");
        if (c2 == '=') ADVANCE_MAKE(TOK_PIPE_ASSIGN, "|=");
        MAKE(TOK_PIPE, "|");
    case '^':
        if (c2 == '=') ADVANCE_MAKE(TOK_CARET_ASSIGN, "^=");
        MAKE(TOK_CARET, "^");
    case '~': MAKE(TOK_TILDE, "~");
    case '<':
        if (c2 == '<') {
            advance(l);
            if (cur(l) == '=') ADVANCE_MAKE(TOK_LSHIFT_ASSIGN, "<<=");
            MAKE(TOK_LSHIFT, "<<");
        }
        if (c2 == '=') ADVANCE_MAKE(TOK_LEQ, "<=");
        MAKE(TOK_LT, "<");
    case '>':
        if (c2 == '>') {
            advance(l);
            if (cur(l) == '=') ADVANCE_MAKE(TOK_RSHIFT_ASSIGN, ">>=");
            MAKE(TOK_RSHIFT, ">>");
        }
        if (c2 == '=') ADVANCE_MAKE(TOK_GEQ, ">=");
        MAKE(TOK_GT, ">");
    case '=':
        if (c2 == '=') ADVANCE_MAKE(TOK_EQ, "==");
        MAKE(TOK_ASSIGN, "=");
    case '!':
        if (c2 == '=') ADVANCE_MAKE(TOK_NEQ, "!=");
        MAKE(TOK_BANG, "!");
    case '.':
        if (c2 == '.' && l->src[l->pos+1] == '.') {
            advance(l); advance(l); MAKE(TOK_ELLIPSIS, "...");
        }
        MAKE(TOK_DOT, ".");
    case '(': MAKE(TOK_LPAREN, "(");
    case ')': MAKE(TOK_RPAREN, ")");
    case '{': MAKE(TOK_LBRACE, "{");
    case '}': MAKE(TOK_RBRACE, "}");
    case '[': MAKE(TOK_LBRACKET, "[");
    case ']': MAKE(TOK_RBRACKET, "]");
    case ';': MAKE(TOK_SEMICOLON, ";");
    case ',': MAKE(TOK_COMMA, ",");
    case '?': MAKE(TOK_QUESTION, "?");
    case ':': MAKE(TOK_COLON, ":");
    default:
        t.type = TOK_UNKNOWN;
        t.text = malloc(2); t.text[0] = c; t.text[1] = '\0';
        return t;
    }
#undef MAKE
#undef ADVANCE_MAKE
}

Token lexer_next(Lexer *l) {
    if (l->has_peek) {
        l->has_peek = 0;
        return l->peeked;
    }
    return read_token(l);
}

Token lexer_peek(Lexer *l) {
    if (!l->has_peek) {
        l->peeked   = read_token(l);
        l->has_peek = 1;
    }
    return l->peeked;
}

const char *token_type_name(TokenType t) {
    switch (t) {
#define C(x) case x: return #x
        C(TOK_INT_LIT);   C(TOK_FLOAT_LIT); C(TOK_CHAR_LIT);  C(TOK_STRING_LIT);
        C(TOK_IDENT);     C(TOK_INT);       C(TOK_CHAR);       C(TOK_FLOAT);
        C(TOK_DOUBLE);    C(TOK_VOID);      C(TOK_LONG);       C(TOK_SHORT);
        C(TOK_UNSIGNED);  C(TOK_SIGNED);    C(TOK_IF);         C(TOK_ELSE);
        C(TOK_WHILE);     C(TOK_FOR);       C(TOK_DO);         C(TOK_RETURN);
        C(TOK_BREAK);     C(TOK_CONTINUE);  C(TOK_STRUCT);     C(TOK_UNION);
        C(TOK_SWITCH);    C(TOK_CASE);      C(TOK_DEFAULT);    C(TOK_GOTO);
        C(TOK_ENUM);      C(TOK_TYPEDEF);   C(TOK_STATIC);     C(TOK_EXTERN);
        C(TOK_CONST);     C(TOK_VOLATILE);  C(TOK_SIZEOF);
        C(TOK_PLUS);      C(TOK_MINUS);     C(TOK_STAR);       C(TOK_SLASH);
        C(TOK_PERCENT);   C(TOK_AMP);       C(TOK_PIPE);       C(TOK_CARET);
        C(TOK_TILDE);     C(TOK_LSHIFT);    C(TOK_RSHIFT);
        C(TOK_EQ);        C(TOK_NEQ);       C(TOK_LT);         C(TOK_GT);
        C(TOK_LEQ);       C(TOK_GEQ);       C(TOK_AND);        C(TOK_OR);
        C(TOK_BANG);      C(TOK_ASSIGN);
        C(TOK_PLUS_ASSIGN); C(TOK_MINUS_ASSIGN);
        C(TOK_STAR_ASSIGN); C(TOK_SLASH_ASSIGN);
        C(TOK_AMP_ASSIGN);  C(TOK_PIPE_ASSIGN);  C(TOK_CARET_ASSIGN);
        C(TOK_LSHIFT_ASSIGN); C(TOK_RSHIFT_ASSIGN); C(TOK_PERCENT_ASSIGN);
        C(TOK_INC);       C(TOK_DEC);
        C(TOK_ARROW);     C(TOK_DOT);
        C(TOK_QUESTION);  C(TOK_COLON);
        C(TOK_LPAREN);    C(TOK_RPAREN);
        C(TOK_LBRACE);    C(TOK_RBRACE);
        C(TOK_LBRACKET);  C(TOK_RBRACKET);
        C(TOK_SEMICOLON); C(TOK_COMMA);     C(TOK_ELLIPSIS);
        C(TOK_EOF);       C(TOK_UNKNOWN);
#undef C
        default: return "???";
    }
}
