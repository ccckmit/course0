set -x
# gcc -O2 -o mpeg2ppm mpeg2ppm0.c -lm
gcc mpeg2ppm.c -o mpeg2ppm \
    $(pkg-config --cflags --libs libavcodec libavformat libavutil libswscale)
./mpeg2ppm test.mpg 200 test.ppm
ffmpeg -i test.ppm test.jpg