#ifndef C0C_MACRO_H
#define C0C_MACRO_H

#include <stddef.h>

void macro_init(void);
void macro_free(void);

int macro_parse_line(char *line, int line_num);
char *macro_expand(const char *input);
char *macro_expand_file(const char *filename);

void macro_define(const char *name, const char *replacement);
void macro_undef(const char *name);
int macro_defined(const char *name);

void macro_enable_expand(void);
void macro_disable_expand(void);

#endif
