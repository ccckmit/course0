#ifndef LEXER_H
#define LEXER_H

extern char *src;
extern int src_idx;
extern char token[256];

void init_lexer(char *source);
void next_token();
int accept(const char *t);
void expect(const char *t);
int is_eof();

#endif
