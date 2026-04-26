#include <stdio.h>

/* 宣告函式 */
int match(const char *regexp, const char *text);
int matchhere(const char *regexp, const char *text);
int matchstar(int c, const char *regexp, const char *text);

/* match: 在 text 中尋找 regexp 的比對 */
int match(const char *regexp, const char *text) {
    /* 如果有 ^，表示必須從字串開頭進行比對 */
    if (regexp[0] == '^')
        return matchhere(regexp + 1, text);
    
    /* 否則，在字串的每個位置都嘗試比對一次 (就算字串是空的也要試一次) */
    do {
        if (matchhere(regexp, text))
            return 1;
    } while (*text++ != '\0');
    
    return 0;
}

/* matchhere: 在 text 的「目前位置」比對 regexp */
int matchhere(const char *regexp, const char *text) {
    /* 如果 regexp 已經走完，代表比對成功 */
    if (regexp[0] == '\0')
        return 1;
        
    /* 處理 '*' (前一個字元出現 0 次或多次) */
    if (regexp[1] == '*')
        return matchstar(regexp[0], regexp + 2, text);
        
    /* 處理 '$' (字串結尾) */
    if (regexp[0] == '$' && regexp[1] == '\0')
        return *text == '\0';
        
    /* 處理 '.' (任意字元) 或 一般字元比對 */
    if (*text != '\0' && (regexp[0] == '.' || regexp[0] == *text))
        return matchhere(regexp + 1, text + 1);
        
    return 0;
}

/* matchstar: 比對 c 出現零次或多次，接著比對剩餘的 regexp */
int matchstar(int c, const char *regexp, const char *text) {
    do {
        /* a* 可以匹配 0 次，所以先嘗試比對剩餘的 regexp */
        if (matchhere(regexp, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    
    return 0;
}

/* 主程式 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "用法: %s <regex> <text>\n", argv[0]);
        return 2; /* 參數錯誤返回 2 */
    }

    const char *regex = argv[1];
    const char *text = argv[2];

    if (match(regex, text)) {
        printf("MATCH    : /%s/ matches \"%s\"\n", regex, text);
        return 0; /* 比對成功返回 0 (Unix 標準成功碼) */
    } else {
        printf("NO MATCH : /%s/ does not match \"%s\"\n", regex, text);
        return 1; /* 比對失敗返回 1 */
    }
}