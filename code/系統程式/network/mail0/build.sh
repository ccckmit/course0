# macOS
# brew install openssl
gcc -o gmailget gmailget.c -lssl -lcrypto \
    -I$(brew --prefix openssl)/include \
    -L$(brew --prefix openssl)/lib

# Linux
#sudo apt install libssl-dev
#gcc -o gmailget gmailget.c -lssl -lcrypto