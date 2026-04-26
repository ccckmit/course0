#include "macro.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_MACROS 256
#define MAX_OUTPUT_SIZE (1 << 20)

typedef struct {
    char name[64];
    char replacement[512];
    int is_function;
    int arg_count;
} Macro;

static Macro macros[MAX_MACROS];
static int macro_count = 0;

void macro_init(void) {
    macro_count = 0;
}

void macro_free(void) {
    macro_count = 0;
}

static Macro *find_macro(const char *name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0) return &macros[i];
    }
    return NULL;
}

static void add_macro(const char *name, const char *replacement) {
    Macro *m = find_macro(name);
    if (!m && macro_count < MAX_MACROS) {
        m = &macros[macro_count++];
        strncpy(m->name, name, sizeof(m->name) - 1);
        m->name[sizeof(m->name) - 1] = '\0';
    }
    if (m) {
        strncpy(m->replacement, replacement, sizeof(m->replacement) - 1);
        m->replacement[sizeof(m->replacement) - 1] = '\0';
    }
}

void macro_define(const char *name, const char *replacement) {
    add_macro(name, replacement);
}

void macro_undef(const char *name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0) {
            for (int j = i; j < macro_count - 1; j++) {
                macros[j] = macros[j + 1];
            }
            macro_count--;
            return;
        }
    }
}

int macro_defined(const char *name) {
    return find_macro(name) != NULL;
}

void macro_enable_expand(void) {}
void macro_disable_expand(void) {}

static void process_define(char *line) {
    char *p = line + 7;
    while (*p == ' ' || *p == '\t') p++;
    
    char name[64];
    int i = 0;
    while (*p && (isalnum(*p) || *p == '_')) {
        if (i < 63) name[i++] = *p;
        p++;
    }
    name[i] = '\0';
    
    Macro *m = find_macro(name);
    if (!m && macro_count < MAX_MACROS) {
        m = &macros[macro_count++];
        strncpy(m->name, name, sizeof(m->name) - 1);
        m->name[sizeof(m->name) - 1] = '\0';
        m->is_function = 0;
        m->arg_count = 0;
    }
    
    if (m) {
        if (*p == '(') {
            m->is_function = 1;
            p++;
            char arg_names[10][32];
            int argc = 0;
            while (*p && *p != ')') {
                while (*p == ' ' || *p == '\t') p++;
                if (*p && *p != ',' && *p != ')') {
                    int j = 0;
                    while (*p && (isalnum(*p) || *p == '_')) {
                        if (j < 31) arg_names[argc][j++] = *p;
                        p++;
                    }
                    arg_names[argc][j] = '\0';
                    argc++;
                }
                if (*p == ',') p++;
                while (*p == ' ' || *p == '\t') p++;
            }
            m->arg_count = argc;
            if (*p == ')') p++;
            while (*p == ' ' || *p == '\t') p++;
            
            char *comment = strstr(p, "//");
            if (comment) *comment = '\0';
            
            char repl[512];
            strncpy(repl, p, sizeof(repl) - 1);
            repl[sizeof(repl) - 1] = '\0';
            
            for (int a = 0; a < argc; a++) {
                char from[32], to[32];
                sprintf(to, "$%d", a + 1);
                strncpy(from, arg_names[a], sizeof(from) - 1);
                from[sizeof(from) - 1] = '\0';
                
                char temp[512];
                temp[0] = '\0';
                char *tp = temp;
                char *rp = repl;
                while (*rp) {
                    if (strncmp(rp, from, strlen(from)) == 0 && 
                        (rp[strlen(from)] == ' ' || rp[strlen(from)] == '\t' || 
                         rp[strlen(from)] == ')' || rp[strlen(from)] == '(' ||
                         rp[strlen(from)] == '\0' || rp[strlen(from)] == '*' ||
                         rp[strlen(from)] == '/' || rp[strlen(from)] == '+' ||
                         rp[strlen(from)] == '-' || rp[strlen(from)] == '=')) {
                        strcpy(tp, to);
                        tp += strlen(to);
                        rp += strlen(from);
                    } else {
                        *tp++ = *rp++;
                    }
                }
                *tp = '\0';
                strncpy(repl, temp, sizeof(repl) - 1);
                repl[sizeof(repl) - 1] = '\0';
            }
            
            strncpy(m->replacement, repl, sizeof(m->replacement) - 1);
            m->replacement[sizeof(m->replacement) - 1] = '\0';
        } else {
            m->is_function = 0;
            m->arg_count = 0;
            
            char *comment = strstr(p, "//");
            if (comment) *comment = '\0';
            
            strncpy(m->replacement, p, sizeof(m->replacement) - 1);
            m->replacement[sizeof(m->replacement) - 1] = '\0';
        }
    }
}

static void process_undef(char *line) {
    char *p = line + 6;
    while (*p == ' ' || *p == '\t') p++;
    
    char name[64];
    int i = 0;
    while (*p && (isalnum(*p) || *p == '_')) {
        if (i < 63) name[i++] = *p;
        p++;
    }
    name[i] = '\0';
    
    macro_undef(name);
}

static void expand_function_macro(char *output, Macro *m, char *args[], int arg_count) {
    char buf[512];
    strcpy(buf, m->replacement);
    
    output[0] = '\0';
    char *p = buf;
    
    while (*p) {
        if (*p == '$' && *(p+1) >= '1' && *(p+1) <= '9') {
            int idx = *(p+1) - '1';
            if (idx < arg_count && idx < 10) {
                strcat(output, args[idx]);
            }
            p += 2;
        } else {
            char c[2] = {*p, '\0'};
            strcat(output, c);
            p++;
        }
    }
}

static void expand_line(char *output, const char *input) {
    strcpy(output, input);
    
    int changed = 1;
    while (changed) {
        changed = 0;
        char temp[MAX_OUTPUT_SIZE];
        temp[0] = '\0';
        
        char *p = output;
        char *out = temp;
        
        while (*p) {
            if (isalpha(*p) || *p == '_') {
                char name[64];
                int len = 0;
                char *start = p;
                while (*p && (isalnum(*p) || *p == '_')) {
                    name[len++] = *p;
                    p++;
                }
                name[len] = '\0';
                
                Macro *m = find_macro(name);
                if (m && m->is_function && *p == '(') {
                    p++;
                    
                    char *args[10];
                    char arg_bufs[10][256];
                    int arg_count = 0;
                    int depth = 1;
                    int pos = 0;
                    
                    args[0] = arg_bufs[0];
                    
                    while (*p && depth > 0) {
                        if (*p == '(') depth++;
                        else if (*p == ')') depth--;
                        
                        if (depth == 1 && *p == ',') {
                            arg_bufs[arg_count][pos] = '\0';
                            arg_count++;
                            args[arg_count] = arg_bufs[arg_count];
                            pos = 0;
                            p++;
                            continue;
                        }
                        
                        if (depth > 0 && pos < 255) {
                            arg_bufs[arg_count][pos++] = *p;
                        }
                        p++;
                    }
                    arg_bufs[arg_count][pos] = '\0';
                    arg_count++;
                    
                    char expanded[512];
                    expand_function_macro(expanded, m, args, arg_count);
                    strcpy(out, expanded);
                    out += strlen(expanded);
                    changed = 1;
                } else if (m && !m->is_function && strlen(m->replacement) > 0) {
                    strcpy(out, m->replacement);
                    out += strlen(m->replacement);
                    changed = 1;
                } else {
                    strncpy(out, start, len);
                    out[len] = '\0';
                    out += len;
                }
            } else {
                *out++ = *p++;
            }
            *out = '\0';
        }
        
        strcpy(output, temp);
    }
}

char *macro_expand(const char *input) {
    char *output = malloc(MAX_OUTPUT_SIZE);
    char *lines = malloc(strlen(input) + 1);
    strcpy(lines, input);
    
    macro_init();
    
    char *result = malloc(MAX_OUTPUT_SIZE);
    result[0] = '\0';
    
    char *line = strtok(lines, "\n");
    while (line) {
        while (*line == ' ' || *line == '\t') line++;
        
        if (strncmp(line, "#define ", 8) == 0) {
            process_define(line);
        } else if (strncmp(line, "#undef ", 7) == 0) {
            process_undef(line);
        } else if (strncmp(line, "#ifdef ", 7) == 0) {
        } else if (strncmp(line, "#ifndef ", 8) == 0) {
        } else if (strncmp(line, "#else", 5) == 0) {
        } else if (strncmp(line, "#endif", 6) == 0) {
        } else {
            char expanded[MAX_OUTPUT_SIZE];
            expand_line(expanded, line);
            strcat(result, expanded);
            strcat(result, "\n");
        }
        
        line = strtok(NULL, "\n");
    }
    
    free(lines);
    free(output);
    macro_free();
    
    return result;
}

char *macro_expand_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *input = malloc(size + 1);
    fread(input, 1, size, f);
    input[size] = '\0';
    fclose(f);
    
    char *output = macro_expand(input);
    free(input);
    
    return output;
}
