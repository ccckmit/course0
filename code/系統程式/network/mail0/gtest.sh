set -x
gcc -o gmailget gmailget.c -lssl -lcrypto \
    -I$(brew --prefix openssl)/include \
    -L$(brew --prefix openssl)/lib
./gmailget ccckmit@gmail.com "mveg vtyz nabw avui"