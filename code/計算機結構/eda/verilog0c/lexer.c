#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

char *src = NULL;
int src_idx = 0;
char token[256];

void init_lexer(char *source) {
    src = source;
    src_idx = 0;
    next_token();
}

void skip_whitespace_and_comments() {
    while (src[src_idx] != '\0') {
        if (isspace(src[src_idx])) {
            src_idx++;
        } else if (src[src_idx] == '/' && src[src_idx+1] == '/') {
            src_idx += 2;
            while (src[src_idx] != '\n' && src[src_idx] != '\0') src_idx++;
        } else {
            break;
        }
    }
}

void next_token() {
    skip_whitespace_and_comments();
    token[0] = '\0';
    
    if (src[src_idx] == '\0') return;

    char c = src[src_idx];
    int len = 0;

    // String literal
    if (c == '"') {
        token[len++] = src[src_idx++];
        while (src[src_idx] != '"' && src[src_idx] != '\0') {
            token[len++] = src[src_idx++];
        }
        if (src[src_idx] == '"') token[len++] = src[src_idx++];
        token[len] = '\0';
        return;
    }

    // Identifiers, system tasks ($display), macros (`define)
    if (isalpha(c) || c == '_' || c == '$' || c == '`') {
        while (isalnum(src[src_idx]) || src[src_idx] == '_' || src[src_idx] == '$' || src[src_idx] == '`') {
            token[len++] = src[src_idx++];
        }
        token[len] = '\0';
        return;
    }

    // Numbers (including sized numbers like 4'h0)
    if (isdigit(c)) {
        while (isalnum(src[src_idx]) || src[src_idx] == '\'') {
            token[len++] = src[src_idx++];
        }
        token[len] = '\0';
        return;
    }

    // Operators and Punctuations
    char c2 = src[src_idx+1];
    if ((c == '=' && c2 == '=') || (c == '!' && c2 == '=') ||
        (c == '<' && c2 == '=') || (c == '>' && c2 == '=') ||
        (c == '<' && c2 == '<') || (c == '>' && c2 == '>') ||
        (c == '&' && c2 == '&') || (c == '|' && c2 == '|')) {
        token[0] = c; token[1] = c2; token[2] = '\0';
        src_idx += 2;
        return;
    }

    // Single character
    token[0] = src[src_idx++];
    token[1] = '\0';
}

int accept(const char *t) {
    if (strcmp(token, t) == 0) {
        next_token();
        return 1;
    }
    return 0;
}

void expect(const char *t) {
    if (accept(t)) return;
    printf("Syntax Error: expected '%s' but got '%s'\n", t, token);
    exit(1);
}

int is_eof() {
    return token[0] == '\0';
}
