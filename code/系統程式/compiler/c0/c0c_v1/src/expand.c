/*
 * expand.c – Minimal local-include expander for c0c self-parse
 *
 * Reads a .c file and expands only #include "..." directives
 * (searching in a given include directory).
 * All other # directives are suppressed (treated as comments).
 * System includes <...> are silently dropped.
 *
 * Usage: expand <include_dir> <file.c>
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEPTH 16
#define MAX_PATH  512

static int expand(const char *incdir, const char *filepath, int depth);

static int expand_file(FILE *fp, const char *incdir, int depth) {
    char line[4096];
    while (fgets(line, sizeof(line), fp)) {
        /* Check for #include "..." */
        const char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (p[0] == '#') {
            /* skip all preprocessor directives by default */
            const char *q = p + 1;
            while (*q == ' ' || *q == '\t') q++;
            if (strncmp(q, "include", 7) == 0) {
                q += 7;
                while (*q == ' ' || *q == '\t') q++;
                if (*q == '"') {
                    q++;
                    char name[256];
                    int ni = 0;
                    while (*q && *q != '"' && ni < (int)sizeof(name)-1)
                        name[ni++] = *q++;
                    name[ni] = '\0';
                    /* build path */
                    char path[MAX_PATH];
                    snprintf(path, sizeof(path), "%s/%s", incdir, name);
                    if (depth < MAX_DEPTH) {
                        expand(incdir, path, depth + 1);
                    }
                }
                /* system includes or other: silently drop */
            }
            /* all other # directives: drop */
            continue;
        }
        fputs(line, stdout);
    }
    return 0;
}

static int expand(const char *incdir, const char *filepath, int depth) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "expand: cannot open '%s'\n", filepath);
        return 1;
    }
    int rc = expand_file(fp, incdir, depth);
    fclose(fp);
    return rc;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: expand <include_dir> <file.c>\n");
        return 1;
    }
    return expand(argv[1], argv[2], 0);
}
