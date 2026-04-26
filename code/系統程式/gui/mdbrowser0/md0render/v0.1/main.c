#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <libgen.h> // 用於 dirname 函數

// --- 配置 ---
#define WINDOW_WIDTH  360
#define WINDOW_HEIGHT 640
#define FONT_SIZE     18
#define MAX_LINKS     64
#define MAX_PATH      512

// --- 狀態結構 ---
typedef struct {
    char url[MAX_PATH]; // 連結的檔名
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
    
    char          current_dir[MAX_PATH]; // 【新增】記錄當前檔案所在的資料夾
} App;

// --- 【新增】輔助函數：組合路徑 ---
void join_path(char* result, const char* dir, const char* filename) {
    if (filename[0] == '/') {
        // 如果是絕對路徑，直接複製
        strncpy(result, filename, MAX_PATH);
    } else {
        // 組合：資料夾 + 斜線 + 檔名
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

// --- 計算 1cm Margin ---
int get_margin_px(SDL_Window* win) {
    int idx = SDL_GetWindowDisplayIndex(win);
    float ddpi, hdpi, vdpi;
    if (SDL_GetDisplayDPI(idx, &ddpi, &hdpi, &vdpi) == 0) {
        return (int)(hdpi / 2.54f);
    }
    return 40;
}

// --- 渲染文字片段 ---
void draw_text(App* app, int* x, int* y, const char* text, int len, SDL_Color color, const char* url) {
    if (len <= 0) return;
    
    char buf[1024];
    if (len >= 1024) len = 1023;
    strncpy(buf, text, len);
    buf[len] = '\0';
    
    int w, h;
    TTF_SizeUTF8(app->font, buf, &w, &h);
    
    if (*x + w > WINDOW_WIDTH - app->margin) {
        *x = app->margin;
        *y += h + 4;
    }
    
    SDL_Surface* surf = TTF_RenderUTF8_Blended(app->font, buf, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(app->ren, surf);
    
    SDL_Rect dst = {*x, *y, surf->w, surf->h};
    SDL_RenderCopy(app->ren, tex, NULL, &dst);
    
    if (url && app->link_count < MAX_LINKS) {
        strncpy(app->links[app->link_count].url, url, MAX_PATH-1);
        app->links[app->link_count].rect = dst;
        app->link_count++;
    }
    
    *x += w;
    
    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

// --- 渲染 Markdown ---
void render_markdown(App* app, const char* filepath) {
    app->link_count = 0;
    if (app->content) free(app->content);
    
    // 【修改】記錄當前檔案的資料夾路徑
    char tmp_path[MAX_PATH];
    strncpy(tmp_path, filepath, MAX_PATH);
    char* dir = dirname(tmp_path); // 取得 "md"
    strncpy(app->current_dir, dir, MAX_PATH);
    printf("Current directory: %s\n", app->current_dir);
    
    app->content = read_file(filepath);
    if (!app->content) {
        printf("Cannot open file: %s\n", filepath);
        return;
    }
    
    SDL_SetRenderDrawColor(app->ren, 255, 255, 255, 255);
    SDL_RenderClear(app->ren);
    
    int x = app->margin;
    int y = app->margin;
    int i = 0;
    int len = strlen(app->content);
    
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color blue  = {0, 0, 255, 255};
    
    while (i < len) {
        if (app->content[i] == '\n') {
            int h = TTF_FontHeight(app->font);
            x = app->margin;
            y += h + 8;
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
                
                draw_text(app, &x, &y, &app->content[start_text], text_len, blue, url);
                
                i++;
                continue;
            }
        }
        
        int start = i;
        while (i < len && app->content[i] != '[' && app->content[i] != '\n') i++;
        draw_text(app, &x, &y, &app->content[start], i - start, black, NULL);
    }
    
    SDL_RenderPresent(app->ren);
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

    // 戴入中文字體
    app.font = TTF_OpenFont("/System/Library/Fonts/PingFang.ttc", FONT_SIZE);
    if (!app.font) app.font = TTF_OpenFont("/System/Library/Fonts/STHeiti Light.ttc", FONT_SIZE);
    if (!app.font) app.font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", FONT_SIZE);
    if (!app.font) {
        printf("Font load failed!\n");
        return 1;
    }

    render_markdown(&app, argv[1]);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = 0;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                for (int i = 0; i < app.link_count; i++) {
                    SDL_Rect r = app.links[i].rect;
                    if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
                        // 【修改】組合路徑後再載入
                        char full_path[MAX_PATH];
                        join_path(full_path, app.current_dir, app.links[i].url);
                        printf("Loading: %s\n", full_path);
                        render_markdown(&app, full_path);
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