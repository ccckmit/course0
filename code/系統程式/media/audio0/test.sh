set -x

# ffmpeg -i test.m4a test.mp3

gcc audio0.c shine/src/lib/*.c -o audio0 -I./shine/src -lm
#gcc audio0.c -o audio0 -I$(brew --prefix)/include -L$(brew --prefix)/lib -lmp3lame -lm
# gcc audio0.c -o audio -lmp3lame -lm

# 1. 查看 MP3 資訊
./audio0 test.mp3

# 2. 真實轉換為 WAV
./audio0 test.mp3 -o test.wav

# 3. 檢查轉換出來的 WAV 資訊
./audio0 test.wav

# 4. 轉換回 mp3
./audio0 test.wav -o test2.mp3
