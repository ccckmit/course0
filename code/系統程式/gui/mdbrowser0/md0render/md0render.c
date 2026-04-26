#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "md0render.h"
#include "../net/http.h"

static int utf8_char_len(const char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

void join_path(char* result, const char* root, const char* dir, const char* filename) {
    if (is_url(filename)) {
        strncpy(result, filename, MAX_PATH - 1);
        result[MAX_PATH - 1] = '\0';
    } else if (filename[0] == '/') {
        strncpy(result, filename, MAX_PATH - 1);
        result[MAX_PATH - 1] = '\0';
    } else {
        if (dir && dir[0]) {
            snprintf(result, MAX_PATH, "%s/%s/%s", root, dir, filename);
        } else {
            snprintf(result, MAX_PATH, "%s/%s", root, filename);
        }
    }
}

static char* read_file(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char* buf = malloc(size + 1);
    fread(buf, 1, size, f);
    buf[size] = '\0';
    fclose(f);
    return buf;
}

int get_margin_px(SDL_Window* win) {
    int idx = SDL_GetWindowDisplayIndex(win);
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(idx, &ddpi, &hdpi, &vdpi) == 0) {
        return (int)((hdpi / 2.54f) * MARGIN_CM);
    }
    return (int)((96.0f / 2.54f) * MARGIN_CM);
}

static void draw_text_segment(App* app, int* x, int* y, const char* text, int len, SDL_Color color, const char* url) {
    if (len <= 0) return;

    int line_height = TTF_FontHeight(app->font);
    int max_content_width = WINDOW_WIDTH - 2 * app->margin;
    
    int ptr = 0;
    while (ptr < len) {
        char line_buf[MAX_LINE_BUF] = {0};
        int line_len = 0;
        int line_w = 0;
        
        while (ptr < len) {
            int char_len = utf8_char_len(text[ptr]);
            if (ptr + char_len > len) char_len = len - ptr;
            
            char tmp_char[5] = {0};
            strncpy(tmp_char, &text[ptr], char_len);
            
            int test_w, test_h;
            char test_buf[MAX_LINE_BUF] = {0};
            strncpy(test_buf, line_buf, line_len);
            strncat(test_buf, tmp_char, char_len);
            TTF_SizeUTF8(app->font, test_buf, &test_w, &test_h);
            
            if (*x + test_w > max_content_width) {
                if (line_len == 0) {
                    strncpy(line_buf, tmp_char, char_len);
                    line_len += char_len;
                    line_w = test_w;
                    ptr += char_len;
                }
                break;
            }
            
            strncpy(&line_buf[line_len], tmp_char, char_len);
            line_len += char_len;
            line_w = test_w;
            ptr += char_len;
        }
        
        if (line_len > 0) {
            SDL_Surface* surf = TTF_RenderUTF8_Blended(app->font, line_buf, color);
            SDL_Texture* tex = SDL_CreateTextureFromSurface(app->ren, surf);
            
            SDL_Rect dst = {*x, *y - app->scroll_y, surf->w, surf->h};
            SDL_RenderCopy(app->ren, tex, NULL, &dst);
            
            if (url && app->link_count < MAX_LINKS) {
                strncpy(app->links[app->link_count].url, url, MAX_PATH-1);
                app->links[app->link_count].rect = dst;
                app->link_count++;
            }
            
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
            *x += line_w;
        }
        
        if (ptr < len) {
            *x = app->margin;
            *y += line_height + 4;
        }
    }
}

void rerender(App* app) {
    if (app->current_file[0] == '\0') return;
    
    app->link_count = 0;
    
    if (!app->is_url) {
        if (app->content) {
            free(app->content);
            app->content = NULL;
        }
        char tmp_path[MAX_PATH];
        strncpy(tmp_path, app->current_file, MAX_PATH);
        char* dir = dirname(tmp_path);
        strncpy(app->current_dir, dir, MAX_PATH);
        
        app->content = read_file(app->current_file);
        if (!app->content) {
            printf("Cannot open file: %s\n", app->current_file);
            return;
        }
    }
    
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_RenderClear(app->ren);
    
    draw_nav_bar(app);
    
    int x = app->margin;
    int y = NAV_BAR_HEIGHT + 10;
    int i = 0;
    int len = strlen(app->content);
    int line_height = TTF_FontHeight(app->font);
    
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color blue  = {0, 0, 255, 255};
    
    while (i < len) {
        if (app->content[i] == '\n') {
            x = app->margin;
            y += line_height + 8;
            i++;
            continue;
        }
        
        if (app->content[i] == '[') {
            i++;
            int start_text = i;
            while (i < len && app->content[i] != ']') i++;
            int text_len = i - start_text;
            
            i++;
            if (i < len && app->content[i] == '(') {
                i++;
                int start_url = i;
                while (i < len && app->content[i] != ')') i++;
                int url_len = i - start_url;
                
                char url[MAX_PATH];
                strncpy(url, &app->content[start_url], url_len);
                url[url_len] = '\0';
                
                draw_text_segment(app, &x, &y, &app->content[start_text], text_len, blue, url);
                
                i++;
                continue;
            }
        }
        
        int start = i;
        while (i < len && app->content[i] != '[' && app->content[i] != '\n') i++;
        draw_text_segment(app, &x, &y, &app->content[start], i - start, black, NULL);
    }
    
    app->content_total_height = y;
    
    SDL_RenderPresent(app->ren);
}

void load_url(App* app, const char* url) {
    printf("Fetching URL: %s\n", url);
    
    HttpResponse resp = http_get(url);
    
    if (resp.data && resp.size > 0) {
        if (app->content) free(app->content);
        app->content = strdup(resp.data);
        
        memset(app->current_file, 0, MAX_PATH);
        strcpy(app->current_file, url);
        
        app->scroll_y = 0;
        app->is_url = 1;
        strcpy(app->current_dir, "");
        
        rerender(app);
    } else {
        printf("Failed to fetch URL: %s\n", resp.error);
    }
    
    http_response_free(&resp);
}

void load_file(App* app, const char* filepath) {
    app->scroll_y = 0;
    app->is_url = 0;
    
    if (is_url(filepath)) {
        load_url(app, filepath);
        return;
    }
    
    if (filepath[0] == '/') {
        strncpy(app->current_file, filepath, MAX_PATH - 1);
        app->current_file[MAX_PATH - 1] = '\0';
    } else {
        char full_path[MAX_PATH];
        join_path(full_path, app->root_dir, "", filepath);
        strncpy(app->current_file, full_path, MAX_PATH - 1);
        app->current_file[MAX_PATH - 1] = '\0';
    }
    
    history_push(app, app->current_file, app->is_url);
    rerender(app);
}

void history_push(App* app, const char* path, int is_url) {
    if (app->history_pos < app->history_count - 1) {
        app->history_count = app->history_pos + 1;
    }
    
    if (app->history_count >= MAX_HISTORY) {
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            app->history[i] = app->history[i + 1];
        }
        app->history_count = MAX_HISTORY - 1;
        app->history_pos = app->history_count - 1;
    }
    
    strncpy(app->history[app->history_count].path, path, MAX_PATH - 1);
    app->history[app->history_count].path[MAX_PATH - 1] = '\0';
    app->history[app->history_count].is_url = is_url;
    app->history_count++;
    app->history_pos = app->history_count - 1;
}

void history_back(App* app) {
    if (app->history_pos > 0) {
        app->history_pos--;
        const char* path = app->history[app->history_pos].path;
        app->is_url = app->history[app->history_pos].is_url;
        strncpy(app->current_file, path, MAX_PATH - 1);
        app->current_file[MAX_PATH - 1] = '\0';
        app->scroll_y = 0;
        rerender(app);
    }
}

void history_forward(App* app) {
    if (app->history_pos < app->history_count - 1) {
        app->history_pos++;
        const char* path = app->history[app->history_pos].path;
        app->is_url = app->history[app->history_pos].is_url;
        strncpy(app->current_file, path, MAX_PATH - 1);
        app->current_file[MAX_PATH - 1] = '\0';
        app->scroll_y = 0;
        rerender(app);
    }
}

int history_can_back(App* app) {
    return app->history_pos > 0;
}

int history_can_forward(App* app) {
    return app->history_pos < app->history_count - 1;
}

static SDL_Rect back_btn = {5, 5, 30, 20};
static SDL_Rect forward_btn = {40, 5, 30, 20};
static SDL_Rect edit_btn = {75, 5, 40, 20};
SDL_Rect done_btn = {WINDOW_WIDTH - 60, 5, 50, 20};
static SDL_Rect go_btn = {WINDOW_WIDTH - 40, 30, 35, 22};
static SDL_Rect url_bar_rect = {5, 30, WINDOW_WIDTH - 50, 22};

void draw_nav_bar(App* app) {
    SDL_SetRenderDrawColor(app->ren, 220, 220, 220, 255);
    SDL_Rect nav_bg = {0, 0, WINDOW_WIDTH, NAV_BAR_HEIGHT};
    SDL_RenderFillRect(app->ren, &nav_bg);
    
    SDL_SetRenderDrawColor(app->ren, 180, 180, 180, 255);
    SDL_RenderDrawRect(app->ren, &back_btn);
    SDL_RenderDrawRect(app->ren, &forward_btn);
    SDL_RenderDrawRect(app->ren, &edit_btn);
    
    SDL_Color gray = {100, 100, 100, 255};
    
    SDL_Surface* back_surf = TTF_RenderUTF8_Blended(app->font, "<", gray);
    SDL_Texture* back_tex = SDL_CreateTextureFromSurface(app->ren, back_surf);
    SDL_Rect back_dst = {back_btn.x + 8, back_btn.y + 2, back_surf->w, back_surf->h};
    SDL_RenderCopy(app->ren, back_tex, NULL, &back_dst);
    SDL_FreeSurface(back_surf);
    SDL_DestroyTexture(back_tex);
    
    SDL_Surface* fwd_surf = TTF_RenderUTF8_Blended(app->font, ">", gray);
    SDL_Texture* fwd_tex = SDL_CreateTextureFromSurface(app->ren, fwd_surf);
    SDL_Rect fwd_dst = {forward_btn.x + 8, forward_btn.y + 2, fwd_surf->w, fwd_surf->h};
    SDL_RenderCopy(app->ren, fwd_tex, NULL, &fwd_dst);
    SDL_FreeSurface(fwd_surf);
    SDL_DestroyTexture(fwd_tex);
    
    SDL_Surface* edit_surf = TTF_RenderUTF8_Blended(app->font, "Edit", gray);
    SDL_Texture* edit_tex = SDL_CreateTextureFromSurface(app->ren, edit_surf);
    SDL_Rect edit_dst = {edit_btn.x + 4, edit_btn.y + 2, edit_surf->w, edit_surf->h};
    SDL_RenderCopy(app->ren, edit_tex, NULL, &edit_dst);
    SDL_FreeSurface(edit_surf);
    SDL_DestroyTexture(edit_tex);
    
    // --- URL bar (second row) ---
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_RenderFillRect(app->ren, &url_bar_rect);
    if (app->url_bar_focused) {
        SDL_SetRenderDrawColor(app->ren, 60, 120, 200, 255);
    } else {
        SDL_SetRenderDrawColor(app->ren, 160, 160, 160, 255);
    }
    SDL_RenderDrawRect(app->ren, &url_bar_rect);
    
    if (app->url_bar[0] != '\0') {
        SDL_Color black = {0, 0, 0, 255};
        SDL_Surface* url_surf = TTF_RenderUTF8_Blended(app->font, app->url_bar, black);
        if (url_surf) {
            SDL_Texture* url_tex = SDL_CreateTextureFromSurface(app->ren, url_surf);
            SDL_Rect url_dst = {url_bar_rect.x + 4, url_bar_rect.y + 3, url_surf->w, url_surf->h};
            if (url_dst.w > url_bar_rect.w - 8) url_dst.w = url_bar_rect.w - 8;
            SDL_RenderCopy(app->ren, url_tex, NULL, &url_dst);
            SDL_FreeSurface(url_surf);
            SDL_DestroyTexture(url_tex);
        }
    }
    
    if (app->url_bar_focused) {
        int cursor_x = url_bar_rect.x + 4;
        if (app->url_bar[0] != '\0') {
            char tmp[MAX_PATH] = {0};
            strncpy(tmp, app->url_bar, app->url_bar_cursor);
            tmp[app->url_bar_cursor] = '\0';
            int w, h;
            TTF_SizeUTF8(app->font, tmp, &w, &h);
            cursor_x += w;
        }
        SDL_SetRenderDrawColor(app->ren, 0, 0, 0, 255);
        SDL_RenderDrawLine(app->ren, cursor_x, url_bar_rect.y + 3, cursor_x, url_bar_rect.y + 19);
    }
    
    // Go button
    SDL_SetRenderDrawColor(app->ren, 80, 160, 230, 255);
    SDL_RenderFillRect(app->ren, &go_btn);
    SDL_SetRenderDrawColor(app->ren, 50, 120, 190, 255);
    SDL_RenderDrawRect(app->ren, &go_btn);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* go_surf = TTF_RenderUTF8_Blended(app->font, "Go", white);
    SDL_Texture* go_tex = SDL_CreateTextureFromSurface(app->ren, go_surf);
    SDL_Rect go_dst = {go_btn.x + 8, go_btn.y + 3, go_surf->w, go_surf->h};
    SDL_RenderCopy(app->ren, go_tex, NULL, &go_dst);
    SDL_FreeSurface(go_surf);
    SDL_DestroyTexture(go_tex);
}

int is_nav_button_click(App* app, int x, int y) {
    if (x >= back_btn.x && x <= back_btn.x + back_btn.w &&
        y >= back_btn.y && y <= back_btn.y + back_btn.h) {
        return 1;
    }
    if (x >= forward_btn.x && x <= forward_btn.x + forward_btn.w &&
        y >= forward_btn.y && y <= forward_btn.y + forward_btn.h) {
        return 2;
    }
    return 0;
}

int is_edit_button_click(App* app, int x, int y) {
    if (x >= edit_btn.x && x <= edit_btn.x + edit_btn.w &&
        y >= edit_btn.y && y <= edit_btn.y + edit_btn.h) {
        return 1;
    }
    return 0;
}

int is_done_button_click(int x, int y) {
    if (x >= done_btn.x && x <= done_btn.x + done_btn.w &&
        y >= done_btn.y && y <= done_btn.y + done_btn.h) {
        return 1;
    }
    return 0;
}

int is_go_button_click(int x, int y) {
    return (x >= go_btn.x && x <= go_btn.x + go_btn.w &&
            y >= go_btn.y && y <= go_btn.y + go_btn.h);
}

int is_url_bar_click(int x, int y) {
    return (x >= url_bar_rect.x && x <= url_bar_rect.x + url_bar_rect.w &&
            y >= url_bar_rect.y && y <= url_bar_rect.y + url_bar_rect.h);
}

static int is_markdown_path(const char* path) {
    if (!path) return 0;
    int len = strlen(path);
    if (len >= 3 && strcasecmp(path + len - 3, ".md") == 0) return 1;
    if (len >= 9 && strcasecmp(path + len - 9, ".markdown") == 0) return 1;
    const char* q = strchr(path, '?');
    if (q) {
        int plen = q - path;
        if (plen >= 3 && strncasecmp(q - 3, ".md", 3) == 0) return 1;
    }
    return 0;
}

void navigate_url_bar(App* app) {
    if (app->url_bar[0] == '\0') return;
    
    if (is_url(app->url_bar)) {
        if (!is_markdown_path(app->url_bar)) {
            if (app->content) free(app->content);
            app->content = strdup("本網頁非 markdown，無法顯示");
            memset(app->current_file, 0, MAX_PATH);
            strcpy(app->current_file, app->url_bar);
            app->scroll_y = 0;
            app->is_url = 1;
            strcpy(app->current_dir, "");
            rerender(app);
            return;
        }
        load_url(app, app->url_bar);
        history_push(app, app->url_bar, 1);
    } else {
        load_file(app, app->url_bar);
    }
}

static char keysym_to_char(SDL_Keycode sym, int shift) {
    if (sym >= SDLK_a && sym <= SDLK_z) {
        return shift ? (char)(sym - 32) : (char)sym;
    }
    if (sym >= SDLK_0 && sym <= SDLK_9) {
        if (shift) {
            const char map[] = ")!@#$%^&*(";
            return map[sym - SDLK_0];
        }
        return (char)sym;
    }
    if (!shift) {
        switch (sym) {
            case SDLK_SPACE: return ' ';
            case SDLK_PERIOD: return '.';
            case SDLK_SLASH: return '/';
            case SDLK_MINUS: return '-';
            case SDLK_EQUALS: return '=';
            case SDLK_SEMICOLON: return ';';
            case SDLK_QUOTE: return '\'';
            case SDLK_COMMA: return ',';
            case SDLK_LEFTBRACKET: return '[';
            case SDLK_RIGHTBRACKET: return ']';
            case SDLK_BACKSLASH: return '\\';
            case SDLK_BACKQUOTE: return '`';
        }
    } else {
        switch (sym) {
            case SDLK_PERIOD: return '>';
            case SDLK_SLASH: return '?';
            case SDLK_MINUS: return '_';
            case SDLK_EQUALS: return '+';
            case SDLK_SEMICOLON: return ':';
            case SDLK_QUOTE: return '"';
            case SDLK_COMMA: return '<';
            case SDLK_LEFTBRACKET: return '{';
            case SDLK_RIGHTBRACKET: return '}';
            case SDLK_BACKSLASH: return '|';
            case SDLK_BACKQUOTE: return '~';
        }
    }
    return 0;
}

static void url_bar_insert_char(App* app, char c) {
    int cur_len = strlen(app->url_bar);
    if (cur_len + 1 < MAX_PATH - 1) {
        memmove(app->url_bar + app->url_bar_cursor + 1,
                app->url_bar + app->url_bar_cursor,
                cur_len - app->url_bar_cursor + 1);
        app->url_bar[app->url_bar_cursor] = c;
        app->url_bar_cursor++;
    }
}

void url_bar_handle_key(App* app, SDL_Event* e) {
    if (e->type != SDL_KEYDOWN) return;

    int sym = e->key.keysym.sym;
    int shift = e->key.keysym.mod & KMOD_SHIFT;

    if (sym == SDLK_RETURN) {
        navigate_url_bar(app);
        app->url_bar_focused = 0;
        return;
    }
    if (sym == SDLK_ESCAPE) {
        app->url_bar_focused = 0;
        return;
    }
    if (sym == SDLK_BACKSPACE) {
        if (app->url_bar_cursor > 0) {
            int len = strlen(app->url_bar);
            app->url_bar_cursor--;
            memmove(app->url_bar + app->url_bar_cursor,
                    app->url_bar + app->url_bar_cursor + 1,
                    len - app->url_bar_cursor);
        }
        return;
    }
    if (sym == SDLK_LEFT) {
        if (app->url_bar_cursor > 0) app->url_bar_cursor--;
        return;
    }
    if (sym == SDLK_RIGHT) {
        int len = strlen(app->url_bar);
        if (app->url_bar_cursor < len) app->url_bar_cursor++;
        return;
    }
    if (sym == SDLK_v && (e->key.keysym.mod & (KMOD_GUI | KMOD_CTRL))) {
        char* clipboard = SDL_GetClipboardText();
        if (clipboard) {
            int clen = strlen(clipboard);
            int cur_len = strlen(app->url_bar);
            if (cur_len + clen < MAX_PATH - 1) {
                memmove(app->url_bar + app->url_bar_cursor + clen,
                        app->url_bar + app->url_bar_cursor,
                        cur_len - app->url_bar_cursor + 1);
                memcpy(app->url_bar + app->url_bar_cursor, clipboard, clen);
                app->url_bar_cursor += clen;
            }
            SDL_free(clipboard);
        }
        return;
    }

    char c = keysym_to_char(sym, shift);
    if (c != 0) {
        url_bar_insert_char(app, c);
    }
}

void enter_edit_mode(App* app, int type) {
    app->edit_mode = 1;
    app->edit_type = type;
    app->edit_buf[0] = '\0';
    app->edit_cursor = 0;
    app->edit_scroll_y = 0;
    
    SDL_StartTextInput();
    
    if (type == 0 && app->content) {
        strncpy(app->edit_buf, app->content, MAX_EDIT_BUF - 1);
        app->edit_buf[MAX_EDIT_BUF - 1] = '\0';
        app->edit_cursor = strlen(app->edit_buf);
    }
}

void exit_edit_mode(App* app) {
    SDL_StopTextInput();
    app->edit_mode = 0;
}

void save_current_file(App* app) {
    if (app->is_url) return;
    
    FILE* f = fopen(app->current_file, "w");
    if (f) {
        fwrite(app->edit_buf, 1, strlen(app->edit_buf), f);
        fclose(f);
    }
}

static int utf8_char_len_at(const char* s, int pos) {
    if ((s[pos] & 0x80) == 0) return 1;
    if ((s[pos] & 0xE0) == 0xC0) return 2;
    if ((s[pos] & 0xF0) == 0xE0) return 3;
    if ((s[pos] & 0xF8) == 0xF0) return 4;
    return 1;
}

void set_cursor_from_mouse(App* app, int mouse_x, int mouse_y) {
    int top_margin = NAV_BAR_HEIGHT + 10;
    int left_margin = app->margin;
    int right_margin = WINDOW_WIDTH - app->margin;
    int line_height = TTF_FontHeight(app->font) + 4;
    
    if (mouse_y < top_margin || mouse_x < left_margin) {
        app->edit_cursor = 0;
        return;
    }
    
    int len = strlen(app->edit_buf);
    int target_screen_y = mouse_y + app->edit_scroll_y;
    int y = top_margin + 5;
    
    int i = 0;
    
    while (i < len) {
        int line_start = i;
        int x = left_margin + 5;
        int pos_in_line = 0;
        int on_this_line = (y <= target_screen_y && target_screen_y < y + line_height);
        
        while (i < len && app->edit_buf[i] != '\n') {
            int char_len = utf8_char_len_at(app->edit_buf, i);
            char tmp[5] = {0};
            for (int k = 0; k < char_len && i + k < len; k++) {
                tmp[k] = app->edit_buf[i + k];
            }
            
            int char_w, char_h;
            TTF_SizeUTF8(app->font, tmp, &char_w, &char_h);
            
            if (x + char_w > right_margin - 5 && pos_in_line > 0) {
                break;
            }
            
            if (on_this_line) {
                if (mouse_x < x + char_w / 2) {
                    app->edit_cursor = i;
                    return;
                }
            }
            
            x += char_w;
            i += char_len;
            pos_in_line++;
        }
        
        if (on_this_line) {
            app->edit_cursor = i;
            return;
        }
        
        y += line_height;
        
        if (i < len && app->edit_buf[i] == '\n') {
            i++;
        }
        
        if (y > target_screen_y) {
            app->edit_cursor = i;
            return;
        }
    }
    
    app->edit_cursor = len;
}

static void ensure_cursor_visible(App* app) {
    int top_margin = NAV_BAR_HEIGHT + 10;
    int left_margin = app->margin;
    int right_margin = WINDOW_WIDTH - app->margin;
    int bottom_margin = WINDOW_HEIGHT - app->margin;
    int line_height = TTF_FontHeight(app->font) + 4;
    
    int len = strlen(app->edit_buf);
    int cursor_pos = app->edit_cursor;
    if (cursor_pos > len) cursor_pos = len;
    
    int y = top_margin + 5;
    int i = 0;
    int cursor_y = y;
    int found = 0;
    
    while (i < len) {
        int line_start = i;
        int x = left_margin + 5;
        
        while (i < len && app->edit_buf[i] != '\n') {
            int char_len = utf8_char_len_at(app->edit_buf, i);
            char tmp[5] = {0};
            for (int k = 0; k < char_len && i + k < len; k++)
                tmp[k] = app->edit_buf[i + k];
            int char_w, char_h;
            TTF_SizeUTF8(app->font, tmp, &char_w, &char_h);
            
            if (x + char_w > right_margin - 5) {
                break;
            }
            x += char_w;
            i += char_len;
        }
        
        if (!found && cursor_pos >= line_start && cursor_pos <= i) {
            cursor_y = y;
            found = 1;
            break;
        }
        
        y += line_height;
        
        if (i < len && app->edit_buf[i] == '\n') {
            i++;
        }
    }
    
    if (!found) {
        cursor_y = y;
    }
    
    int screen_y = cursor_y - app->edit_scroll_y;
    
    if (screen_y + line_height > bottom_margin) {
        app->edit_scroll_y = cursor_y + line_height - bottom_margin;
    }
    if (screen_y < top_margin + 5) {
        app->edit_scroll_y = cursor_y - (top_margin + 5);
    }
    if (app->edit_scroll_y < 0) app->edit_scroll_y = 0;
}

void draw_edit_mode(App* app) {
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_RenderClear(app->ren);
    
    int top_margin = NAV_BAR_HEIGHT + 10;
    int left_margin = app->margin;
    int right_margin = WINDOW_WIDTH - app->margin;
    int bottom_margin = WINDOW_HEIGHT - app->margin;
    int max_width = right_margin - left_margin - 10;
    
    SDL_SetRenderDrawColor(app->ren, 220, 220, 220, 255);
    SDL_Rect nav_bg = {0, 0, WINDOW_WIDTH, NAV_BAR_HEIGHT};
    SDL_RenderFillRect(app->ren, &nav_bg);
    
    done_btn = (SDL_Rect){WINDOW_WIDTH - 60, 5, 50, 20};
    SDL_SetRenderDrawColor(app->ren, 100, 200, 100, 255);
    SDL_RenderFillRect(app->ren, &done_btn);
    SDL_SetRenderDrawColor(app->ren, 50, 150, 50, 255);
    SDL_RenderDrawRect(app->ren, &done_btn);
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* done_surf = TTF_RenderUTF8_Blended(app->font, "Done", white);
    SDL_Texture* done_tex = SDL_CreateTextureFromSurface(app->ren, done_surf);
    SDL_Rect done_dst = {done_btn.x + 8, done_btn.y + 2, done_surf->w, done_surf->h};
    SDL_RenderCopy(app->ren, done_tex, NULL, &done_dst);
    SDL_FreeSurface(done_surf);
    SDL_DestroyTexture(done_tex);
    
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_Rect edit_area = {left_margin, top_margin, right_margin - left_margin, bottom_margin - top_margin};
    SDL_RenderFillRect(app->ren, &edit_area);
    
    SDL_SetRenderDrawColor(app->ren, 180, 180, 180, 255);
    SDL_RenderDrawRect(app->ren, &edit_area);
    
    int line_height = TTF_FontHeight(app->font) + 4;
    int font_height = TTF_FontHeight(app->font);
    
    if (app->edit_type == 1) {
        SDL_Color blue = {0, 0, 200, 255};
        SDL_Surface* label_surf = TTF_RenderUTF8_Blended(app->font, "URL:", blue);
        SDL_Texture* label_tex = SDL_CreateTextureFromSurface(app->ren, label_surf);
        SDL_Rect label_dst = {left_margin + 5, top_margin + 5, label_surf->w, label_surf->h};
        SDL_RenderCopy(app->ren, label_tex, NULL, &label_dst);
        SDL_FreeSurface(label_surf);
        SDL_DestroyTexture(label_tex);
        
        SDL_SetRenderDrawColor(app->ren, 245, 245, 245, 255);
        SDL_Rect url_box = {left_margin, top_margin + 30, edit_area.w - 10, 25};
        SDL_RenderFillRect(app->ren, &url_box);
        SDL_SetRenderDrawColor(app->ren, 150, 150, 150, 255);
        SDL_RenderDrawRect(app->ren, &url_box);
        
        if (app->edit_buf[0] != '\0') {
            SDL_Surface* url_surf = TTF_RenderUTF8_Blended(app->font, app->edit_buf, (SDL_Color){0, 0, 0, 255});
            SDL_Texture* url_tex = SDL_CreateTextureFromSurface(app->ren, url_surf);
            SDL_Rect url_dst = {left_margin + 5, top_margin + 33, url_surf->w, url_surf->h};
            if (url_dst.w > url_box.w - 10) url_dst.w = url_box.w - 10;
            SDL_RenderCopy(app->ren, url_tex, NULL, &url_dst);
            SDL_FreeSurface(url_surf);
            SDL_DestroyTexture(url_tex);
        }
        
        int cursor_x = left_margin + 5;
        if (app->edit_buf[0] != '\0') {
            int w, h;
            TTF_SizeUTF8(app->font, app->edit_buf, &w, &h);
            cursor_x += w;
        }
        SDL_SetRenderDrawColor(app->ren, 0, 0, 0, 255);
        SDL_RenderDrawLine(app->ren, cursor_x, top_margin + 32, cursor_x, top_margin + 53);
    } else {
        ensure_cursor_visible(app);
        
        int len = strlen(app->edit_buf);
        int cursor_buf_pos = app->edit_cursor;
        if (cursor_buf_pos > len) cursor_buf_pos = len;
        
        int sy = app->edit_scroll_y;
        int y = top_margin + 5;
        int i = 0;
        int rendered_up_to = 0;
        
        while (i < len) {
            int x = left_margin + 5;
            int line_start = i;
            int screen_y = y - sy;
            
            while (i < len && app->edit_buf[i] != '\n') {
                int char_len = utf8_char_len_at(app->edit_buf, i);
                char tmp[5] = {0};
                for (int k = 0; k < char_len && i + k < len; k++) {
                    tmp[k] = app->edit_buf[i + k];
                }
                
                int char_w, char_h;
                TTF_SizeUTF8(app->font, tmp, &char_w, &char_h);
                
                if (x + char_w > right_margin - 5) {
                    break;
                }
                
                if (screen_y >= top_margin - line_height && screen_y <= bottom_margin) {
                    SDL_Color black = {0, 0, 0, 255};
                    SDL_Surface* surf = TTF_RenderUTF8_Blended(app->font, tmp, black);
                    if (surf) {
                        SDL_Texture* tex = SDL_CreateTextureFromSurface(app->ren, surf);
                        SDL_Rect dst = {x, screen_y, surf->w, surf->h};
                        SDL_RenderCopy(app->ren, tex, NULL, &dst);
                        SDL_DestroyTexture(tex);
                        SDL_FreeSurface(surf);
                    }
                }
                
                x += char_w;
                i += char_len;
            }
            
            rendered_up_to = i;
            y += line_height;
            
            if (i < len && app->edit_buf[i] == '\n') {
                i++;
                rendered_up_to = i;
            }
            
            if (y - sy > bottom_margin) break;
            
            if (cursor_buf_pos <= rendered_up_to && cursor_buf_pos >= line_start) {
                int cx = left_margin + 5;
                for (int j = line_start; j < cursor_buf_pos && j < len && app->edit_buf[j] != '\n';) {
                    int char_len = utf8_char_len_at(app->edit_buf, j);
                    char tmp[5] = {0};
                    for (int k = 0; k < char_len && j + k < len; k++) {
                        tmp[k] = app->edit_buf[j + k];
                    }
                    int cw, ch;
                    TTF_SizeUTF8(app->font, tmp, &cw, &ch);
                    cx += cw;
                    j += char_len;
                }
                int cursor_sy = y - sy;
                SDL_SetRenderDrawColor(app->ren, 0, 100, 200, 255);
                SDL_RenderDrawLine(app->ren, cx, cursor_sy - line_height + 1, cx, cursor_sy - 3);
            }
        }
        
        if (cursor_buf_pos >= len) {
            int cx = left_margin + 5;
            for (int j = rendered_up_to; j < len && app->edit_buf[j] != '\n';) {
                int char_len = utf8_char_len_at(app->edit_buf, j);
                char tmp[5] = {0};
                for (int k = 0; k < char_len && j + k < len; k++) {
                    tmp[k] = app->edit_buf[j + k];
                }
                int cw, ch;
                TTF_SizeUTF8(app->font, tmp, &cw, &ch);
                cx += cw;
                j += char_len;
            }
            int cursor_sy = y - sy;
            SDL_SetRenderDrawColor(app->ren, 0, 100, 200, 255);
            SDL_RenderDrawLine(app->ren, cx, cursor_sy - line_height + 1, cx, cursor_sy - 3);
        }
    }
    
    SDL_RenderPresent(app->ren);
}

void edit_mode_key(App* app, SDL_Event* e) {
    if (e->type == SDL_KEYDOWN) {
        if (e->key.keysym.sym == SDLK_ESCAPE) {
            exit_edit_mode(app);
            rerender(app);
        }
        else if (e->key.keysym.sym == SDLK_RETURN) {
            if (app->edit_type == 0) {
                save_current_file(app);
            } else {
                if (is_url(app->edit_buf)) {
                    load_url(app, app->edit_buf);
                    history_push(app, app->edit_buf, 1);
                }
            }
            exit_edit_mode(app);
            rerender(app);
        }
        else if (e->key.keysym.sym == SDLK_BACKSPACE) {
            if (app->edit_cursor > 0) {
                int char_len = utf8_char_len(app->edit_buf[app->edit_cursor - 1]);
                app->edit_cursor -= char_len;
                int len = strlen(app->edit_buf);
                for (int i = app->edit_cursor; i < len - char_len + 1; i++) {
                    app->edit_buf[i] = app->edit_buf[i + char_len];
                }
            }
        }
        else if (e->key.keysym.sym == SDLK_DELETE) {
            int len = strlen(app->edit_buf);
            if (app->edit_cursor < len) {
                int char_len = utf8_char_len(app->edit_buf[app->edit_cursor]);
                for (int i = app->edit_cursor; i < len - char_len + 1; i++) {
                    app->edit_buf[i] = app->edit_buf[i + char_len];
                }
            }
        }
        else if (e->key.keysym.sym == SDLK_LEFT) {
            if (app->edit_cursor > 0) {
                int char_len = utf8_char_len(app->edit_buf[app->edit_cursor - 1]);
                app->edit_cursor -= char_len;
            }
        }
        else if (e->key.keysym.sym == SDLK_RIGHT) {
            int len = strlen(app->edit_buf);
            if (app->edit_cursor < len) {
                int char_len = utf8_char_len(app->edit_buf[app->edit_cursor]);
                app->edit_cursor += char_len;
            }
        }
        else if (e->key.keysym.sym == SDLK_HOME) {
            int len = strlen(app->edit_buf);
            while (app->edit_cursor > 0 && app->edit_buf[app->edit_cursor - 1] != '\n') {
                int char_len = utf8_char_len(app->edit_buf[app->edit_cursor - 1]);
                app->edit_cursor -= char_len;
            }
        }
        else if (e->key.keysym.sym == SDLK_END) {
            int len = strlen(app->edit_buf);
            while (app->edit_cursor < len && app->edit_buf[app->edit_cursor] != '\n') {
                int char_len = utf8_char_len(app->edit_buf[app->edit_cursor]);
                app->edit_cursor += char_len;
            }
        }
        else if (e->key.keysym.sym == SDLK_UP) {
            int current_line = 0;
            for (int i = 0; i < app->edit_cursor && app->edit_buf[i]; i++) {
                if (app->edit_buf[i] == '\n') current_line++;
            }
            if (current_line > 0) {
                int pos_in_line = 0;
                int i = 0;
                int line = 0;
                while (app->edit_buf[i] && line < current_line) {
                    if (app->edit_buf[i] == '\n') line++;
                    i++;
                }
                while (app->edit_buf[i] && app->edit_buf[i] != '\n') {
                    int char_len = utf8_char_len(app->edit_buf[i]);
                    pos_in_line++;
                    i += char_len;
                }
                
                int target_line = current_line - 1;
                int target_pos = 0;
                i = 0;
                line = 0;
                while (app->edit_buf[i] && line < target_line) {
                    if (app->edit_buf[i] == '\n') line++;
                    i++;
                }
                while (app->edit_buf[i] && app->edit_buf[i] != '\n' && target_pos < pos_in_line) {
                    int char_len = utf8_char_len(app->edit_buf[i]);
                    target_pos++;
                    i += char_len;
                }
                app->edit_cursor = i;
            }
        }
        else if (e->key.keysym.sym == SDLK_DOWN) {
            int len = strlen(app->edit_buf);
            int current_line = 0;
            for (int i = 0; i < app->edit_cursor && app->edit_buf[i]; i++) {
                if (app->edit_buf[i] == '\n') current_line++;
            }
            
            int pos_in_line = 0;
            int i = 0;
            int line = 0;
            while (app->edit_buf[i] && line < current_line) {
                if (app->edit_buf[i] == '\n') line++;
                i++;
            }
            while (i < app->edit_cursor && app->edit_buf[i] && app->edit_buf[i] != '\n') {
                int char_len = utf8_char_len(app->edit_buf[i]);
                pos_in_line++;
                i += char_len;
            }
            
            int next_line_start = i;
            while (next_line_start < len && app->edit_buf[next_line_start] != '\n') {
                int char_len = utf8_char_len(app->edit_buf[next_line_start]);
                next_line_start += char_len;
            }
            if (next_line_start < len) next_line_start++;
            
            if (next_line_start < len) {
                int target_pos = 0;
                i = next_line_start;
                while (app->edit_buf[i] && app->edit_buf[i] != '\n' && target_pos < pos_in_line) {
                    int char_len = utf8_char_len(app->edit_buf[i]);
                    target_pos++;
                    i += char_len;
                }
                app->edit_cursor = i;
            }
        }
        else if (e->key.keysym.sym == SDLK_v && (e->key.keysym.mod & KMOD_CTRL)) {
            char* clipboard = SDL_GetClipboardText();
            if (clipboard) {
                int len = strlen(clipboard);
                int cur_len = strlen(app->edit_buf);
                if (app->edit_cursor < cur_len) {
                    memmove(app->edit_buf + app->edit_cursor + len, 
                            app->edit_buf + app->edit_cursor, 
                            cur_len - app->edit_cursor + 1);
                    memcpy(app->edit_buf + app->edit_cursor, clipboard, len);
                } else {
                    strcpy(app->edit_buf + cur_len, clipboard);
                }
                app->edit_cursor += len;
                SDL_free(clipboard);
            }
        }
    }
    else if (e->type == SDL_TEXTINPUT) {
        int len = strlen(e->text.text);
        int cur_len = strlen(app->edit_buf);
        if (app->edit_cursor < cur_len) {
            memmove(app->edit_buf + app->edit_cursor + len, 
                    app->edit_buf + app->edit_cursor, 
                    cur_len - app->edit_cursor + 1);
            memcpy(app->edit_buf + app->edit_cursor, e->text.text, len);
        } else {
            strcpy(app->edit_buf + cur_len, e->text.text);
        }
        app->edit_cursor += len;
    }
}
