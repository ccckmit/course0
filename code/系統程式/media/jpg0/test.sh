set -x
gcc -O2 -o jpg0 jpg0.c -lm
./jpg0 img/test.ppm
./jpg0 img/test.ppm -o img/test.jpg          # 預設 quality=75
./jpg0 img/test.jpg
./jpg0 img/test.jpg -o img/test2.ppm
