#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>

#define MAX_LINES 1000
#define MAX_COLS 256
#define MAX_FILENAME 256

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int kbhit() {
    struct timeval tv = {0, 0};
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

#define MODE_COMMAND 0
#define MODE_INSERT 1

typedef struct {
    char *lines[MAX_LINES];
    int line_count;
    int cursor_row;
    int cursor_col;
    char filename[MAX_FILENAME];
    int modified;
    int mode;
} Editor;

void editor_init(Editor *ed) {
    ed->line_count = 0;
    ed->lines[0] = malloc(MAX_COLS);
    ed->lines[0][0] = '\0';
    ed->line_count = 1;
    ed->cursor_row = 0;
    ed->cursor_col = 0;
    ed->filename[0] = '\0';
    ed->modified = 0;
    ed->mode = MODE_COMMAND;
}

void editor_free(Editor *ed) {
    for (int i = 0; i < ed->line_count; i++) {
        free(ed->lines[i]);
    }
}

void editor_insert_char(Editor *ed, char c) {
    if (ed->cursor_row >= MAX_LINES) return;
    char *line = ed->lines[ed->cursor_row];
    int len = strlen(line);
    if (len >= MAX_COLS - 1) return;
    
    memmove(line + ed->cursor_col + 1, line + ed->cursor_col, len - ed->cursor_col + 1);
    line[ed->cursor_col] = c;
    ed->cursor_col++;
    ed->modified = 1;
}

void editor_backspace(Editor *ed) {
    if (ed->cursor_col > 0) {
        char *line = ed->lines[ed->cursor_row];
        memmove(line + ed->cursor_col - 1, line + ed->cursor_col, strlen(line) - ed->cursor_col + 1);
        ed->cursor_col--;
        ed->modified = 1;
    } else if (ed->cursor_row > 0) {
        int prev_len = strlen(ed->lines[ed->cursor_row - 1]);
        int curr_len = strlen(ed->lines[ed->cursor_row]);
        if (prev_len + curr_len < MAX_COLS) {
            strcat(ed->lines[ed->cursor_row - 1], ed->lines[ed->cursor_row]);
            free(ed->lines[ed->cursor_row]);
            for (int i = ed->cursor_row; i < ed->line_count - 1; i++) {
                ed->lines[i] = ed->lines[i + 1];
            }
            ed->line_count--;
            ed->cursor_row--;
            ed->cursor_col = prev_len;
            ed->modified = 1;
        }
    }
}

void editor_newline(Editor *ed) {
    if (ed->cursor_row >= MAX_LINES - 1) return;
    char *line = ed->lines[ed->cursor_row];
    int len = strlen(line);
    
    if (ed->cursor_col < len) {
        char *new_line = malloc(MAX_COLS);
        strcpy(new_line, line + ed->cursor_col);
        line[ed->cursor_col] = '\0';
        
        for (int i = ed->line_count; i > ed->cursor_row + 1; i--) {
            ed->lines[i] = ed->lines[i - 1];
        }
        ed->lines[ed->cursor_row + 1] = new_line;
        ed->line_count++;
    } else {
        for (int i = ed->line_count; i > ed->cursor_row + 1; i--) {
            ed->lines[i] = ed->lines[i - 1];
        }
        ed->lines[ed->cursor_row + 1] = malloc(MAX_COLS);
        ed->lines[ed->cursor_row + 1][0] = '\0';
        ed->line_count++;
    }
    ed->cursor_row++;
    ed->cursor_col = 0;
    ed->modified = 1;
}

void editor_move_cursor(Editor *ed, int direction) {
    switch (direction) {
        case 0:
            if (ed->cursor_col > 0) ed->cursor_col--;
            break;
        case 1:
            if (ed->cursor_col < (int)strlen(ed->lines[ed->cursor_row])) ed->cursor_col++;
            break;
        case 2:
            if (ed->cursor_row > 0) {
                ed->cursor_row--;
                int max_col = strlen(ed->lines[ed->cursor_row]);
                if (ed->cursor_col > max_col) ed->cursor_col = max_col;
            }
            break;
        case 3:
            if (ed->cursor_row < ed->line_count - 1) {
                ed->cursor_row++;
                int max_col = strlen(ed->lines[ed->cursor_row]);
                if (ed->cursor_col > max_col) ed->cursor_col = max_col;
            }
            break;
    }
}

int editor_save(Editor *ed) {
    if (ed->filename[0] == '\0') return 0;
    FILE *fp = fopen(ed->filename, "w");
    if (!fp) return 0;
    for (int i = 0; i < ed->line_count; i++) {
        fprintf(fp, "%s\n", ed->lines[i]);
    }
    fclose(fp);
    ed->modified = 0;
    return 1;
}

void editor_open(Editor *ed, const char *filename) {
    strncpy(ed->filename, filename, MAX_FILENAME - 1);
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        ed->line_count = 0;
        ed->lines[0] = malloc(MAX_COLS);
        ed->lines[0][0] = '\0';
        ed->line_count = 1;
        return;
    }
    
    char buffer[MAX_COLS];
    ed->line_count = 0;
    while (fgets(buffer, MAX_COLS, fp) && ed->line_count < MAX_LINES) {
        int len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') buffer[len - 1] = '\0';
        ed->lines[ed->line_count] = malloc(MAX_COLS);
        strcpy(ed->lines[ed->line_count], buffer);
        ed->line_count++;
    }
    fclose(fp);
    
    if (ed->line_count == 0) {
        ed->lines[0] = malloc(MAX_COLS);
        ed->lines[0][0] = '\0';
        ed->line_count = 1;
    }
    ed->cursor_row = 0;
    ed->cursor_col = 0;
}

void editor_status(Editor *ed, char *status) {
    if (ed->mode == MODE_INSERT) {
        snprintf(status, 256, "-- INSERT --");
    } else {
        snprintf(status, 256, "\"%s\"%s %d/%d", 
            ed->filename[0] ? ed->filename : "[No Name]",
            ed->modified ? " [Modified]" : "",
            ed->cursor_row + 1,
            ed->line_count);
    }
}

void editor_status_cmd(Editor *ed, char *status) {
    snprintf(status, 256, "\"%s\"%s %d/%d -- COMMAND --", 
        ed->filename[0] ? ed->filename : "[No Name]",
        ed->modified ? " [Modified]" : "",
        ed->cursor_row + 1,
        ed->line_count);
}

void editor_render(Editor *ed) {
    printf("\033[2J\033[H");
    printf("\n");
    for (int i = 0; i < ed->line_count; i++) {
        printf("%s\n", ed->lines[i]);
    }
    
    for (int i = ed->line_count; i < 20; i++) {
        printf("~\n");
    }
    
    char status[256];
    if (ed->mode == MODE_COMMAND) {
        editor_status_cmd(ed, status);
    } else {
        editor_status(ed, status);
    }
    printf("\033[21;1H\033[7m%s\033[0m", status);
    
    printf("\033[%d;%dH", ed->cursor_row + 2, ed->cursor_col + 1);
    fflush(stdout);
}

int editor_execute_command(Editor *ed, char *cmd) {
    if (cmd[0] == 'w') {
        if (cmd[1] == 'q') {
            if (editor_save(ed)) {
                return 1;
            }
        } else if (cmd[1] == ' ') {
            char filename[MAX_FILENAME];
            sscanf(cmd + 2, "%s", filename);
            if (filename[0]) {
                strncpy(ed->filename, filename, MAX_FILENAME - 1);
            }
            editor_save(ed);
        } else {
            editor_save(ed);
        }
    } else if (cmd[0] == 'q') {
        if (!ed->modified) {
            return 1;
        }
    } else if (cmd[0] == 'q' && cmd[1] == '!') {
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    Editor ed;
    editor_init(&ed);
    
    if (argc > 1) {
        editor_open(&ed, argv[1]);
    }
    
    enable_raw_mode();
    
    while (1) {
        editor_render(&ed);
        
        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) break;
        
        if (ed.mode == MODE_COMMAND) {
            if (c == 'i') {
                ed.mode = MODE_INSERT;
            } else if (c == 27) {
                usleep(10000);
                if (kbhit()) {
                    char seq[2];
                    if (read(STDIN_FILENO, &seq[0], 1) > 0 && read(STDIN_FILENO, &seq[1], 1) > 0) {
                        if (seq[0] == '[') {
                            if (seq[1] == 'A') {
                                editor_move_cursor(&ed, 2);
                            } else if (seq[1] == 'B') {
                                editor_move_cursor(&ed, 3);
                            } else if (seq[1] == 'C') {
                                editor_move_cursor(&ed, 1);
                            } else if (seq[1] == 'D') {
                                editor_move_cursor(&ed, 0);
                            }
                        }
                    }
                }
            } else if (c == 'h') {
                editor_move_cursor(&ed, 0);
            } else if (c == 'l') {
                editor_move_cursor(&ed, 1);
            } else if (c == 'k') {
                editor_move_cursor(&ed, 2);
            } else if (c == 'j') {
                editor_move_cursor(&ed, 3);
            } else if (c == ':') {
                char cmd[256] = {0};
                int i = 0;
                printf("\033[21;1H:\033[K");
                fflush(stdout);
                while (read(STDIN_FILENO, &c, 1) > 0) {
                    if (c == '\n') break;
                    if (c == 27) {
                        break;
                    }
                    cmd[i++] = c;
                    putchar(c);
                    fflush(stdout);
                }
                if (c == 27) {
                    continue;
                }
                if (editor_execute_command(&ed, cmd)) {
                    break;
                }
            }
        } else {
            if (c == 27) {
                usleep(10000);
                if (kbhit()) {
                    char seq[2];
                    if (read(STDIN_FILENO, &seq[0], 1) > 0 && read(STDIN_FILENO, &seq[1], 1) > 0) {
                        if (seq[1] == 'A') {
                            editor_move_cursor(&ed, 2);
                        } else if (seq[1] == 'B') {
                            editor_move_cursor(&ed, 3);
                        } else if (seq[1] == 'C') {
                            editor_move_cursor(&ed, 1);
                        } else if (seq[1] == 'D') {
                            editor_move_cursor(&ed, 0);
                        }
                    }
                }
                ed.mode = MODE_COMMAND;
            } else if (c == 127 || c == 8 || c == 0x7f) {
                editor_backspace(&ed);
            } else if (c == '\n') {
                editor_newline(&ed);
            } else if (c >= 32) {
                editor_insert_char(&ed, c);
            }
        }
    }
    
    editor_free(&ed);
    printf("\033[2J\033[H");
    return 0;
}
