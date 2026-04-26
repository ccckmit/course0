
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