set -x
gcc mpg0.c -o mpg0 -lm
./mpg0 test.mpg # 會印出 mpg 的基本資訊
./mpg0 test.mpg 200 test.ppm # 會將 mpg 的第 200 個影格抽出來
ffmpeg -i test.ppm test.jpg
