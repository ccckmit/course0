#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct Parser Parser;

Parser *parser_new(Lexer *lexer);
void    parser_free(Parser *p);
Node   *parser_parse(Parser *p);  /* returns ND_TRANSLATION_UNIT */

#endif /* PARSER_H */
