#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>  // 由 pmake 的 add_packages("openssl") 處理路徑
#include "math_utils.h"

int main() {
    // 1. 測試自定義函式庫
    int num = 5;
    printf("--- pmake 跨平台測試 ---\n");
    printf("數字 %d 的平方是: %d\n", num, square(num));

    // 2. 測試 OpenSSL SHA256
    const char *data = "Hello make0!";
    unsigned char hash[SHA256_DIGEST_LENGTH];

    // 呼叫 OpenSSL 函式
    SHA256((unsigned char*)data, strlen(data), hash);

    printf("字串: \"%s\"\n", data);
    printf("SHA256 結果: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n------------------------\n");

    return 0;
}
