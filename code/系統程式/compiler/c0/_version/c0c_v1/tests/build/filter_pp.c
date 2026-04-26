/*
 * filter_pp – keep lines from our own source files after gcc -E
 * Matches filenames containing "src/" but not "stub_include".
 */
#include <stdio.h>
#include <string.h>
static int is_our_src(const char *fname) {
    if (!strstr(fname, "src/")) return 0;
    if (strstr(fname, "stub_include")) return 0;
    return 1;
}
int main(void) {
    char line[65536];
    int in_src = 0;
    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '#' && line[1] == ' ') {
            char *q1 = strchr(line, '"');
            if (q1) {
                char *q2 = strchr(q1+1, '"');
                if (q2) { *q2 = '\0'; in_src = is_our_src(q1+1); }
            }
            continue;
        }
        if (in_src) fputs(line, stdout);
    }
    return 0;
}
