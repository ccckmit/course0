#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

/* ── Token kinds ─────────────────────────────────────────────────────────── */
typedef enum {
    /* Literals */
    TK_IDENT = 1,
    TK_INT_LIT,
    TK_FLOAT_LIT,
    TK_CHAR_LIT,
    TK_STR_LIT,

    /* Keywords (must stay contiguous – used for range checks) */
    TK_AUTO, TK_BREAK, TK_CASE, TK_CHAR, TK_CONST,
    TK_CONTINUE, TK_DEFAULT, TK_DO, TK_DOUBLE, TK_ELSE,
    TK_ENUM, TK_EXTERN, TK_FLOAT, TK_FOR, TK_GOTO,
    TK_IF, TK_INT, TK_LONG, TK_REGISTER, TK_RETURN,
    TK_SHORT, TK_SIGNED, TK_SIZEOF, TK_STATIC, TK_STRUCT,
    TK_SWITCH, TK_TYPEDEF, TK_UNION, TK_UNSIGNED, TK_VOID,
    TK_VOLATILE, TK_WHILE,

    /* Punctuation / operators (single-char) */
    TK_LPAREN,    /* ( */
    TK_RPAREN,    /* ) */
    TK_LBRACE,    /* { */
    TK_RBRACE,    /* } */
    TK_LBRACKET,  /* [ */
    TK_RBRACKET,  /* ] */
    TK_SEMICOLON, /* ; */
    TK_COMMA,     /* , */
    TK_COLON,     /* : */
    TK_QUESTION,  /* ? */
    TK_TILDE,     /* ~ */

    /* Operators (possibly multi-char) */
    TK_DOT,       /* . */
    TK_ELLIPSIS,  /* ... */

    TK_PLUS,      /* + */
    TK_PLUS_PLUS, /* ++ */
    TK_PLUS_EQ,   /* += */

    TK_MINUS,     /* - */
    TK_MINUS_MINUS, /* -- */
    TK_MINUS_EQ,  /* -= */
    TK_ARROW,     /* -> */

    TK_STAR,      /* * */
    TK_STAR_EQ,   /* *= */

    TK_SLASH,     /* / */
    TK_SLASH_EQ,  /* /= */

    TK_PERCENT,   /* % */
    TK_PERCENT_EQ, /* %= */

    TK_AMP,       /* & */
    TK_AMP_AMP,   /* && */
    TK_AMP_EQ,    /* &= */

    TK_PIPE,      /* | */
    TK_PIPE_PIPE, /* || */
    TK_PIPE_EQ,   /* |= */

    TK_CARET,     /* ^ */
    TK_CARET_EQ,  /* ^= */

    TK_BANG,      /* ! */
    TK_BANG_EQ,   /* != */

    TK_EQ,        /* = */
    TK_EQ_EQ,     /* == */

    TK_LT,        /* < */
    TK_LT_LT,     /* << */
    TK_LT_LT_EQ,  /* <<= */
    TK_LT_EQ,     /* <= */

    TK_GT,        /* > */
    TK_GT_GT,     /* >> */
    TK_GT_GT_EQ,  /* >>= */
    TK_GT_EQ,     /* >= */

    TK_HASH,      /* # (preprocessor, kept for completeness) */

    /* Special */
    TK_EOF,
    TK_UNKNOWN,

    TK_COUNT  /* total number of token kinds */
} TokenKind;

/* ── Token ───────────────────────────────────────────────────────────────── */
typedef struct {
    TokenKind kind;
    char     *text;     /* NUL-terminated lexeme (owned by Lexer arena) */
    int       line;
    int       col;
} Token;

/* ── Lexer ───────────────────────────────────────────────────────────────── */
typedef struct Lexer Lexer;

/* Create a lexer that reads from an in-memory buffer (src, len).
   filename is used for error messages only. */
Lexer *lexer_new_buf(const char *filename, const char *src, int len);

/* Create a lexer that reads from a file.  Returns NULL on open error. */
Lexer *lexer_new_file(const char *filename);

/* Advance and return the next token.  Returns TK_EOF repeatedly at end. */
Token  lexer_next(Lexer *lx);

/* Peek at the next token without consuming it. */
Token  lexer_peek(Lexer *lx);

/* Free all resources owned by the lexer. */
void   lexer_free(Lexer *lx);

/* Human-readable name for a token kind. */
const char *token_kind_name(TokenKind k);

/* Run self-test: lex the given file, print tokens, return 0 on success. */
int lexer_test(const char *filename);

#endif /* LEXER_H */
