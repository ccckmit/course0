/Users/Shared/ccc/c0computer/web/browser0/_version/v1.1-url/ 資料夾下

browser0.c 是一個 markdown 瀏覽器， md0render/md0render.c 是一個 markdown render

目前該瀏覽器已經可以運作，是用 C + SDL 寫的

接下來要修改的功能是

支援網路上的網址之瀏覽

像是

[測試網路文章：c0computer](https://raw.githubusercontent.com/ccc-c/c0computer/refs/heads/main/README.md)

另外原本的程式，沒辦法支援相對路徑，檔案都得放在 md/ 下，請改成可以在命令列執行時， argv[1] 指定根目錄，然後用相對路徑的方式，存取檔案

如果在參數中沒有指定，那就用目前資料夾當根目錄

請不要使用 curl 之類的函式庫，網路功能從 socket 開始寫，加上 net/http.c , net/http.h 的程式，將網路功能寫在裡面

