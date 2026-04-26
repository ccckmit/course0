#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/lexer.h"
#include "../include/parser.h"
#include "../include/quad.h"

static char *read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        exit(1);
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = (char *)malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    return content;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.py>\n", argv[0]);
        return 1;
    }
    
    char *source = read_file(argv[1]);
    
    Lexer lex;
    lexer_init(&lex, source);
    
    Parser parser;
    parser_init(&parser, &lex);
    
    Program *prog = parse_program(&parser);
    
    Quadruple *quads = generate_quadruples(prog);
    print_quadruples(quads);
    
    free_quadruples(quads);
    free_program(prog);
    free(source);
    
    return 0;
}
