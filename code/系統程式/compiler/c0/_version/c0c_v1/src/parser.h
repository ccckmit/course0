#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"

typedef struct Parser Parser;

/* Create a parser for the given source file.
   Returns NULL on open error. */
Parser *parser_new_file(const char *filename);

/* Parse the whole translation unit.  Returns the root ND_TRANSLATION_UNIT
   node, or NULL on fatal error. */
Node *parser_parse(Parser *p);

/* Release parser resources (arena, lexer). */
void parser_free(Parser *p);

/* Run round-trip test: parse file → ast_to_c → compare.
   Returns 0 on success. */
int parser_test(const char *filename);

/* Return number of parse errors encountered. */
int parser_errors(Parser *p);

#endif /* PARSER_H */
