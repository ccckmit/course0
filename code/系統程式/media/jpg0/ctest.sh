set -x
gcc -O2 -o jpg0 jpg0.c -lm
./jpg0 img/test.bmp
./jpg0 img/test.bmp -o img/test.jpg          # 預設 quality=75
./jpg0 img/test.jpg
./jpg0 img/test.jpg -o img/test2.bmp
# ./bmp2jpg input.bmp output.jpg 90       # 高品質
# ./bmp2jpg input.bmp output.jpg 50       # 較小檔案