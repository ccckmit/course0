set -x
gcc mp4.c -o mp4 -lm
./mp4 test.mp4 # 會印出 mp4 的基本資訊
./mp4 test.mp4 200 test.ppm # 會將 mp4 的第 200 個影格抽出來
ffmpeg -i test.ppm test.jpg
