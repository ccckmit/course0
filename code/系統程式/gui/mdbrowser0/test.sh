set -x

SSL_CFLAGS=$(pkg-config --cflags openssl 2>/dev/null || echo "-I/opt/homebrew/Cellar/openssl@3/3.6.1/include")
SSL_LIBS=$(pkg-config --libs openssl 2>/dev/null || echo "-L/opt/homebrew/Cellar/openssl@3/3.6.1/lib -lssl -lcrypto")

clang -I md0render/ -I net/ $SSL_CFLAGS md0render/md0render.c net/http.c browser0.c -o browser0 \
    `sdl2-config --cflags --libs` -lSDL2_ttf $SSL_LIBS

echo ""
echo "=== Build Complete ==="
echo ""
echo "Usage:"
echo "  ./browser0 <file> [root_dir]  - Open markdown file"
echo "  ./browser0 <url>               - Open URL directly"

./browser0 index.md md
