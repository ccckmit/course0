#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char op;
    char *line;
} DiffOp;

char** split_lines(const char *content, int *line_count) {
    int count = 0;
    int len = strlen(content);
    for (int i = 0; i < len; i++) {
        if (content[i] == '\n') count++;
    }
    if (len > 0 && content[len-1] != '\n') count++;
    
    char **lines = malloc(count * sizeof(char*));
    char *copy = strdup(content);
    
    int idx = 0;
    char *start = copy;
    for (int i = 0; i < len; i++) {
        if (content[i] == '\n') {
            copy[i] = '\0';
            lines[idx++] = start;
            start = copy + i + 1;
        }
    }
    if (idx < count) lines[idx++] = start;
    *line_count = count;
    return lines;
}

void free_lines(char **lines, int count, char *buffer) {
    free(buffer);
    free(lines);
}

void diff_dp(char **A, int N, char **B, int M) {
    int **dp = malloc((N + 1) * sizeof(int*));
    for (int i = 0; i <= N; i++) {
        dp[i] = calloc(M + 1, sizeof(int));
    }

    for (int i = 1; i <= N; i++) {
        for (int j = 1; j <= M; j++) {
            if (strcmp(A[i-1], B[j-1]) == 0) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = dp[i-1][j] > dp[i][j-1] ? dp[i-1][j] : dp[i][j-1];
            }
        }
    }

    int i = N, j = M;
    DiffOp *ops = malloc((N + M) * sizeof(DiffOp));
    int op_count = 0;

    while (i > 0 || j > 0) {
        if (i > 0 && j > 0 && strcmp(A[i-1], B[j-1]) == 0) {
            ops[op_count].op = ' ';
            ops[op_count].line = A[i-1];
            op_count++;
            i--; j--;
        } else if (i > 0 && (j == 0 || dp[i][j-1] < dp[i-1][j])) {
            ops[op_count].op = '-';
            ops[op_count].line = A[i-1];
            op_count++;
            i--;
        } else {
            ops[op_count].op = '+';
            ops[op_count].line = B[j-1];
            op_count++;
            j--;
        }
    }

    for (int k = op_count - 1; k >= 0; k--) {
        printf("%c %s\n", ops[k].op, ops[k].line);
    }

    free(ops);
    for (int i = 0; i <= N; i++) free(dp[i]);
    free(dp);
}

char* read_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return NULL;
    }
    
    fseek(f, 0, SEEK_END);
    int len = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(len + 1);
    fread(content, 1, len, f);
    content[len] = '\0';
    fclose(f);
    return content;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <fileA> <fileB>\n", argv[0]);
        return 1;
    }

    char *contentA = read_file(argv[1]);
    char *contentB = read_file(argv[2]);
    
    if (!contentA || !contentB) {
        free(contentA);
        free(contentB);
        return 1;
    }

    int lineA, lineB;
    char **linesA = split_lines(contentA, &lineA);
    char **linesB = split_lines(contentB, &lineB);

    diff_dp(linesA, lineA, linesB, lineB);

    free_lines(linesA, lineA, contentA);
    free_lines(linesB, lineB, contentB);
    return 0;
}
