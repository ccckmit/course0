#ifndef LEXER_H
#define LEXER_H

typedef enum {
    /* Literals */
    TOK_INT_LIT, TOK_FLOAT_LIT, TOK_CHAR_LIT, TOK_STRING_LIT,
    /* Identifiers & keywords */
    TOK_IDENT,
    TOK_INT, TOK_CHAR, TOK_FLOAT, TOK_DOUBLE, TOK_VOID,
    TOK_LONG, TOK_SHORT, TOK_UNSIGNED, TOK_SIGNED,
    TOK_IF, TOK_ELSE, TOK_WHILE, TOK_FOR, TOK_DO,
    TOK_RETURN, TOK_BREAK, TOK_CONTINUE,
    TOK_STRUCT, TOK_UNION, TOK_ENUM, TOK_TYPEDEF,
    TOK_STATIC, TOK_EXTERN, TOK_CONST, TOK_VOLATILE,
    TOK_SIZEOF,
    /* Operators */
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_AMP, TOK_PIPE, TOK_CARET, TOK_TILDE,
    TOK_LSHIFT, TOK_RSHIFT,
    TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LEQ, TOK_GEQ,
    TOK_AND, TOK_OR, TOK_BANG,
    TOK_ASSIGN,
    TOK_PLUS_ASSIGN, TOK_MINUS_ASSIGN, TOK_STAR_ASSIGN, TOK_SLASH_ASSIGN,
    TOK_AMP_ASSIGN, TOK_PIPE_ASSIGN, TOK_CARET_ASSIGN,
    TOK_LSHIFT_ASSIGN, TOK_RSHIFT_ASSIGN, TOK_PERCENT_ASSIGN,
    TOK_INC, TOK_DEC,
    TOK_ARROW, TOK_DOT,
    TOK_QUESTION, TOK_COLON,
    /* Delimiters */
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET,
    TOK_SEMICOLON, TOK_COMMA,
    TOK_ELLIPSIS,
    /* Special */
    TOK_EOF,
    TOK_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char     *text;      /* heap-allocated, NUL-terminated */
    int       line;
    int       col;
} Token;

typedef struct Lexer Lexer;

Lexer  *lexer_new(const char *src, const char *filename);
void    lexer_free(Lexer *l);
Token   lexer_next(Lexer *l);
Token   lexer_peek(Lexer *l);
void    token_free(Token t);
const char *token_type_name(TokenType t);

#endif /* LEXER_H */