set -x

cd "$(dirname "$0")"

# openssl req -x509 -newkey rsa:2048 -keyout key.pem -out cert.pem -days 365 -nodes
# gcc -o https_server https_server.c -lssl -lcrypto
gcc -o https_server https_server.c -I/opt/homebrew/opt/openssl/include -L/opt/homebrew/opt/openssl/lib -lssl -lcrypto
./https_server 8443 cert.pem key.pem