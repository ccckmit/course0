set -x

# 使用 sdl2-config 自動抓取標頭檔和函式庫路徑並編譯
clang md_render.c main.c -o main `sdl2-config --cflags --libs`  -lSDL2_ttf

./main md/index.md
