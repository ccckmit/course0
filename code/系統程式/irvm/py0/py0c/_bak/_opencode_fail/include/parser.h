#ifndef PARSER_H
#define PARSER_H

#include "../include/lexer.h"
#include "../include/ast.h"

typedef struct {
    Lexer *lexer;
    Token *current;
} Parser;

void parser_init(Parser *parser, Lexer *lexer);
Program *parse_program(Parser *parser);
Expr *parse_expr(Parser *parser);
Suite *parse_suite(Parser *parser);

#endif
