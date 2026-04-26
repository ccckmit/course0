# 編譯
gcc -O2 -o lz0 lz0.c

# 壓縮（輸出 test.txt.lz）
./lz0 test.txt

cp test.txt.lz test2.txt.lz

# 解壓縮（輸出 test.txt）
./lz0 test2.txt.lz