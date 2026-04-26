#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include "md_render.h"

// --- 輔助：取得 UTF-8 字符的位元組長度 ---
static int utf8_char_len(const char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return 1;
}

// --- 輔助：組合路徑 ---
void join_path(char* result, const char* dir, const char* filename) {
    if (filename[0] == '/') {
        strncpy(result, filename, MAX_PATH);
    } else {
        snprintf(result, MAX_PATH, "%s/%s", dir, filename);
    }
}

// --- 讀取文件 ---
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

// --- 計算 Margin 像素 ---
int get_margin_px(SDL_Window* win) {
    int idx = SDL_GetWindowDisplayIndex(win);
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(idx, &ddpi, &hdpi, &vdpi) == 0) {
        return (int)((hdpi / 2.54f) * MARGIN_CM);
    }
    return (int)((96.0f / 2.54f) * MARGIN_CM);
}

// --- 繪製文字 (修復左右邊距計算) ---
static void draw_text_segment(App* app, int* x, int* y, const char* text, int len, SDL_Color color, const char* url) {
    if (len <= 0) return;

    int line_height = TTF_FontHeight(app->font);
    // 【修復】左右各留一個margin，內容最大寬度 = 窗口寬 - 2*邊距
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

// --- 重新渲染 (新增內容總高度記錄) ---
void rerender(App* app) {
    if (!app->current_file) return;
    
    app->link_count = 0;
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
    
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_RenderClear(app->ren);
    
    // 起始位置：左上邊距
    int x = app->margin;
    int y = app->margin;
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
    
    // 【新增】記錄整份內容渲染完的總高度，用於底部邊距控制
    app->content_total_height = y;
    
    SDL_RenderPresent(app->ren);
}

// --- 載入新檔案 ---
void load_file(App* app, const char* filepath) {
    app->scroll_y = 0;
    app->current_file = filepath;
    rerender(app);
}