# Telnet Client / Server

用 C 語言寫的簡易 Telnet 客戶端與伺服器，支援 `cd` 指令切換目錄。

## 編譯

```bash
gcc -Wall -o telnet_server telnet_server.c
gcc -Wall -o telnet_client telnet_client.c
```

## 使用方式

### 啟動伺服器

```bash
# 預設 port 2323
./telnet_server

# 指定 port
./telnet_server 8023
```

### 連線客戶端

```bash
# 用自己的 client 連線
./telnet_client localhost 2323

# 或用系統 telnet
telnet localhost 2323
```

## 功能

- **cd 指令**：透過 `chdir()` 切換目錄，在整個 session 中持續有效
- **一般指令**：透過 `popen()` 執行，輸出即時回傳給 client
- **多連線**：server 使用 `fork()` 支援多個同時連線
- **彩色 prompt**：顯示目前工作目錄
- 輸入 `exit` 或 `quit` 結束連線
