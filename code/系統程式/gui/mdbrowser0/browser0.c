#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "md0render.h"

static void get_current_dir(char* buf, size_t size) {
    if (getcwd(buf, size) == NULL) {
        buf[0] = '.';
        buf[1] = '\0';
    }
}

static void resolve_root_and_file(const char* arg, char* root_dir, char* file_path) {
    if (is_url(arg)) {
        strcpy(root_dir, "");
        strcpy(file_path, arg);
    } else if (arg[0] == '/') {
        char tmp[MAX_PATH];
        strncpy(tmp, arg, MAX_PATH - 1);
        tmp[MAX_PATH - 1] = '\0';
        char* dir = dirname(tmp);
        strncpy(root_dir, dir, MAX_PATH - 1);
        root_dir[MAX_PATH - 1] = '\0';
        strcpy(file_path, arg);
    } else {
        char* last_slash = strrchr(arg, '/');
        if (last_slash) {
            char tmp[MAX_PATH];
            strncpy(tmp, arg, MAX_PATH - 1);
            tmp[MAX_PATH - 1] = '\0';
            char* dir = dirname(tmp);
            strncpy(root_dir, dir, MAX_PATH - 1);
            root_dir[MAX_PATH - 1] = '\0';
            strcpy(file_path, last_slash + 1);
        } else {
            get_current_dir(root_dir, MAX_PATH);
            strcpy(file_path, arg);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <file> [root_dir]\n", argv[0]);
        printf("  If root_dir is not specified, uses current directory\n");
        return 1;
    }

    App app;
    memset(&app, 0, sizeof(app));
    
    char init_file[MAX_PATH];
    if (argc >= 3) {
        strncpy(app.root_dir, argv[2], MAX_PATH - 1);
        app.root_dir[MAX_PATH - 1] = '\0';
        strncpy(init_file, argv[1], MAX_PATH - 1);
        init_file[MAX_PATH - 1] = '\0';
    } else {
        resolve_root_and_file(argv[1], app.root_dir, init_file);
    }
    
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    app.win = SDL_CreateWindow("md0r", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    app.ren = SDL_CreateRenderer(app.win, -1, SDL_RENDERER_PRESENTVSYNC);
    app.margin = get_margin_px(app.win);

    app.font = TTF_OpenFont("/System/Library/Fonts/STHeiti Light.ttc", FONT_SIZE);
    if (!app.font) app.font = TTF_OpenFont("/System/Library/Fonts/Helvetica.ttc", FONT_SIZE);
    if (!app.font) app.font = TTF_OpenFont("/System/Library/Fonts/PingFang.ttc", FONT_SIZE);
    if (!app.font) {
        printf("Font load failed!\n");
        return 1;
    }

    load_file(&app, init_file);

    SDL_Event e;
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (app.edit_mode) {
                if (e.type == SDL_QUIT) {
                    running = 0;
                }
                else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    exit_edit_mode(&app);
                    rerender(&app);
                }
                else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
                    if (app.edit_type == 0) {
                        save_current_file(&app);
                    } else {
                        if (is_url(app.edit_buf)) {
                            load_url(&app, app.edit_buf);
                            history_push(&app, app.edit_buf, 1);
                        }
                    }
                    exit_edit_mode(&app);
                    rerender(&app);
                }
                else if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int mx = e.button.x;
                    int my = e.button.y;
                    if (is_done_button_click(mx, my)) {
                        if (app.edit_type == 0) {
                            save_current_file(&app);
                        } else {
                            if (is_url(app.edit_buf)) {
                                load_url(&app, app.edit_buf);
                                history_push(&app, app.edit_buf, 1);
                            }
                        }
                        exit_edit_mode(&app);
                        rerender(&app);
                    } else {
                        set_cursor_from_mouse(&app, mx, my);
                        draw_edit_mode(&app);
                    }
                }
                else {
                    edit_mode_key(&app, &e);
                    draw_edit_mode(&app);
                }
                continue;
            }
            
            if (e.type == SDL_QUIT) {
                running = 0;
            }
            else if (app.url_bar_focused && e.type == SDL_KEYDOWN) {
                url_bar_handle_key(&app, &e);
                rerender(&app);
            }
            else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_LEFT) {
                    history_back(&app);
                }
                else if (e.key.keysym.sym == SDLK_RIGHT) {
                    history_forward(&app);
                }
            }
            else if (e.type == SDL_MOUSEWHEEL) {
                app.scroll_y -= e.wheel.y * 30;
                if (app.scroll_y < 0) app.scroll_y = 0;
                int max_scroll = app.content_total_height - (WINDOW_HEIGHT - app.margin);
                if (max_scroll < 0) max_scroll = 0;
                if (app.scroll_y > max_scroll) app.scroll_y = max_scroll;
                rerender(&app);
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mx = e.button.x;
                int my = e.button.y;
                int nav = is_nav_button_click(&app, mx, my);
                if (nav == 1) {
                    history_back(&app);
                } else if (nav == 2) {
                    history_forward(&app);
                } else if (is_go_button_click(mx, my)) {
                    navigate_url_bar(&app);
                    app.url_bar_focused = 0;
                } else if (is_url_bar_click(mx, my)) {
                    app.url_bar_focused = 1;
                    app.url_bar_cursor = strlen(app.url_bar);
                    rerender(&app);
                } else if (is_edit_button_click(&app, mx, my)) {
                    if (!app.is_url) {
                        app.url_bar_focused = 0;
                        enter_edit_mode(&app, 0);
                        draw_edit_mode(&app);
                    }
                } else {
                    if (app.url_bar_focused) {
                        app.url_bar_focused = 0;
                        rerender(&app);
                    }
                    for (int i = 0; i < app.link_count; i++) {
                        SDL_Rect r = app.links[i].rect;
                        if (mx >= r.x && mx <= r.x + r.w && my >= r.y && my <= r.y + r.h) {
                            load_file(&app, app.links[i].url);
                            break;
                        }
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
