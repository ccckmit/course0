請寫一個 fulltext0.c 全文檢索引擎，檢索以 row 為單位的文章。

要使用 reverse index 來做索引。

目前已經有 1000 句語句，其中要有很多關鍵詞是重疊的，放在 _data/corpus.txt 下。

500 句中文，500句英文

然後再開始寫一個 fulltext0.c 可以對 corpus.txt 以 row 為單位進行檢索。

要寫 test.sh 讓我可以測試 fulltext0.c 的檢索結果是否正確。


中文語句不能用空白格開。

全文檢索的索引，要用二字＋一字的方式建索引。

查詢時，先用二字檢索，只有單一字時就用單字檢索，檢索後再過濾掉那些不合的查詢關鍵字的。

把程式分成幾部分

1. 建索引 index.c
2. 搜尋 query.c
3. 共用的 fulltext0.c


索引必須以 4K 為單位輸出到 _index/data.idx ，當資料多的時候，會一邊建一邊輸出。

搜尋時直接讀取該索引檔，然後過濾輸出


共用的部分，放在 fulltext0.c ，然後另外寫 query.c 和 index.c

把 fulltext0.h 也寫出來，共用的部分，不要重複，直接引用


