#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <libgen.h>

// --- 配置 ---
#define WINDOW_WIDTH   360
#define WINDOW_HEIGHT  480
#define FONT_SIZE      12
#define MARGIN_CM      0.5f       // 【新增】邊距設定 (單位：公分)
#define MAX_LINKS      64
#define MAX_PATH       512
#define MAX_LINE_BUF   1024

// --- 狀態結構 ---
typedef struct {
    char url[MAX_PATH];
    SDL_Rect rect;
} Link;

typedef struct {
    SDL_Window*   win;
    SDL_Renderer* ren;
    TTF_Font*     font;
    int           margin;
    
    char*         content;
    Link          links[MAX_LINKS];
    int           link_count;
    
    char          current_dir[MAX_PATH];
    int           scroll_y;
    const char*   current_file;
} App;

// --- 輔助：取得 UTF-8 字符的位元組長度 ---
int utf8_char_len(const char c) {
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
char* read_file(const char* path) {
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

// --- 【修改】計算 Margin 像素 (根據設定的 CM 數) ---
int get_margin_px(SDL_Window* win) {
    int idx = SDL_GetWindowDisplayIndex(win);
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(idx, &ddpi, &hdpi, &vdpi) == 0) {
        // 公式：(每吋像素 / 2.54) * 公分數
        return (int)((hdpi / 2.54f) * MARGIN_CM);
    }
    // 備用值：如果抓不到 DPI，用 96 DPI 計算
    return (int)((96.0f / 2.54f) * MARGIN_CM);
}

// --- 繪製文字 ---
void draw_text_segment(App* app, int* x, int* y, const char* text, int len, SDL_Color color, const char* url) {
    if (len <= 0) return;

    int line_height = TTF_FontHeight(app->font);
    int max_content_width = WINDOW_WIDTH - app->margin;
    
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

// --- 重新渲染 ---
void rerender(App* app) {
    if (!app->current_file) return;
    
    app->link_count = 0;
    if (app->content) free(app->content);
    
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
    
    SDL_RenderPresent(app->ren);
}

// --- 載入新檔案 ---
void load_file(App* app, const char* filepath) {
    app->scroll_y = 0;
    app->current_file = filepath;
    rerender(app);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s file.md\n", argv[0]);
        return 1;
    }

    App app = {0};
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    app.win = SDL_CreateWindow("md0r", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    app.ren = SDL_CreateRenderer(app.win, -1, SDL_RENDERER_PRESENTVSYNC);
    app.margin = get_margin_px(app.win);

    app.font = TTF_OpenFont("/System/Library/Fonts/PingFang.ttc", FONT_SIZE);
    if (!app.font) app.font = TTF_OpenFont("/System/Library/Fonts/STHeiti Light.ttc", FONT_SIZE);
    if (!app.font) app.font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", FONT_SIZE);
    if (!app.font) {
        printf("Font load failed!\n");
        return 1;
    }

    load_file(&app, argv[1]);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } 
            else if (e.type == SDL_MOUSEWHEEL) {
                app.scroll_y -= e.wheel.y * 30;
                if (app.scroll_y < 0) app.scroll_y = 0;
                rerender(&app);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                for (int i = 0; i < app.link_count; i++) {
                    SDL_Rect r = app.links[i].rect;
                    if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
                        char full_path[MAX_PATH];
                        join_path(full_path, app.current_dir, app.links[i].url);
                        printf("Loading: %s\n", full_path);
                        load_file(&app, full_path);
                        break;
                    }
                }
            }
        }
    }

    if (app.content) free(app.content);
    TTF_CloseFont(app.font);
    SDL_DestroyRenderer(app.ren);
    SDL_DestroyWindow(app.win);
    TTF_Quit();
    SDL_Quit();

    return 0;
}