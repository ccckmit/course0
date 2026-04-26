set -x

cd "$(dirname "$0")"

# gcc -o webserver webserver.c
gcc -o webserver webserver.c
./webserver 8080