#include "../include/rand.h"
#include <string.h>
#include <stdlib.h>

void rand_bytes(uint8_t *buf, size_t len) {
    if (len == 0) return;
    
    arc4random_buf(buf, len);
}