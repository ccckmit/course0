#include <stdio.h>
#include <string.h>
#include "../include/rand.h"
#include "../include/common.h"

int main() {
    printf("=== rand tests ===\n\n");
    
    /* rand_bytes produces non-zero data */
    {
        uint8_t buf[16] = {0};
        rand_bytes(buf, 16);
        int non_zero = 0;
        for (int i = 0; i < 16; i++) {
            if (buf[i] != 0) non_zero = 1;
        }
        CHECK(non_zero, "rand_bytes produces non-zero");
    }
    
    /* rand_bytes len=0 */
    {
        uint8_t buf[1];
        buf[0] = 0x42;
        rand_bytes(buf, 0);
        CHECK(buf[0] == 0x42, "rand_bytes len=0 is no-op");
    }
    
    printf("\n=== Results: %d passed, %d failed ===\n", g_pass, g_fail);
    return g_fail > 0 ? 1 : 0;
}