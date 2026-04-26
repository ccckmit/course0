# make0

`make0` 是一個輕量級、跨平台的 C/C++ 專案建置工具。

## 安裝

```bash
pip install make0
```

## 使用方法

在專案目錄下建立 

檔案： make0.py

```python
app = target("ssl_app")
app.set_kind("binary")
app.add_files("main.c", "math_utils.c")
app.add_packages("openssl")
```

檔案： main.c

```c
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

```

檔案： math_util.h

```h
#ifndef MATH_UTILS_H
#define MATH_UTILS_H

int square(int a);

#endif
```

檔案： math_util.c

```c
#include "math_utils.h"

int square(int a) {
    return a * a;
}
```

然後執行建置：

```bash
make0 build
```

這樣就會產生

```sh
(venv) cccuser@cccimacdeiMac hello_make0 % ls -all
total 104
drwxr-xr-x@ 8 cccuser  staff    256 Mar 15 17:32 .
drwxr-xr-x@ 3 cccuser  staff     96 Mar 15 17:32 ..
drwxr-xr-x@ 4 cccuser  staff    128 Mar 15 17:32 .pmake_cache
-rw-r--r--@ 1 cccuser  staff    752 Mar 15 20:14 main.c
-rw-r--r--@ 1 cccuser  staff     64 Mar 15 16:14 math_utils.c
-rw-r--r--@ 1 cccuser  staff     69 Mar 15 16:01 math_utils.h
-rw-r--r--@ 1 cccuser  staff    195 Mar 15 16:28 make0file
-rwxr-xr-x@ 1 cccuser  staff  33688 Mar 15 17:32 ssl_app
```

最後你就可以執行

```sh
(venv) cccuser@cccimacdeiMac hello_make0 % make0    
🛠️  正在建置目標: ssl_app
  [CC] main.c
  [LINK] ssl_app
✅ 建置成功: ssl_app

(venv) cccuser@cccimacdeiMac hello_make0 % ./ssl_app
--- pmake 跨平台測試 ---
數字 5 的平方是: 25
字串: "Hello make0!"
SHA256 結果: 7125ba4e245a131db68a904342602208aefed07ace1b6ea02a8efff3fb07b6a4
------------------------
```
