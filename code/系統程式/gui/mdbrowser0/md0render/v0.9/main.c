#include <SDL.h>
#include <SDL_ttf.h>
#include "md_render.h"

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

    // 載入中文字體
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
            // 【修復】新增底部邊界的滾動限制
            else if (e.type == SDL_MOUSEWHEEL) {
                app.scroll_y -= e.wheel.y * 30;

                // 頂部邊界：不讓內容滾到頂部邊距以上
                if (app.scroll_y < 0) {
                    app.scroll_y = 0;
                }

                // 底部邊界：不讓內容滾到超出底部邊距
                // 最大滾動值 = 內容總高度 - (窗口高度 - 底部邊距)
                int max_scroll = app.content_total_height - (WINDOW_HEIGHT - app.margin);
                if (max_scroll < 0) max_scroll = 0; // 內容不滿一屏時，禁止向下滾動
                if (app.scroll_y > max_scroll) {
                    app.scroll_y = max_scroll;
                }

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

    // 清理
    if (app.content) free(app.content);
    TTF_CloseFont(app.font);
    SDL_DestroyRenderer(app.ren);
    SDL_DestroyWindow(app.win);
    TTF_Quit();
    SDL_Quit();

    return 0;
}