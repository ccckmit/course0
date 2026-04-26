#ifndef MD_RENDER_H
#define MD_RENDER_H

#include <SDL.h>
#include <SDL_ttf.h>

// --- 配置 ---
#define WINDOW_WIDTH   360
#define WINDOW_HEIGHT  480
#define FONT_SIZE      12
#define MARGIN_CM      0.5f
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
    int           content_total_height; // 【新增】記錄整份內容的總高度，用於底部邊距控制
} App;

// --- 函數宣告 ---
int   get_margin_px(SDL_Window* win);
void  load_file(App* app, const char* filepath);
void  rerender(App* app);
void  join_path(char* result, const char* dir, const char* filename);

#endif