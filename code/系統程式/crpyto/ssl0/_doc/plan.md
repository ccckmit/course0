# ssl0 專案

## 設計原則

1. 採用最小程式碼原則，沒用到的不需要做
2. 需要寫測試程式，測試每個函式庫是否正確
3. 為每個函式庫寫一個 xxx_test.sh ，讓使用者方便測試

## 步驟

1. 實作完成 https 所需要的 ssl 與 crpyto.c 函式庫
2. 先寫出 crypto.c ，只有 https 會用到的才做 
    * 憑證產生用 openssl 就好，crypto.c 只要能不用任何 ssl, crypto 套件建立 https 就行。
3. 寫出 https 需要的 ssl 函式庫，然後實作一個 httpd.c 的靜態 server