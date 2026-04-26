# C5 編譯器生態系 (v1.0 模組化強化版)

基於知名的 [**"C in four functions" (c4)**](https://github.com/rswier/c4) 極簡自舉直譯器所深度改造。這個計畫將原本的實驗性腳本直譯器，徹底昇華為了一套**具備靜態連結器、物件檔生成、虛擬機器與支援多檔案自舉**的完整 C 語言微型工具鏈。

## 🎯 核心亮點 (Core Features)

1. **完整工具鏈架構 (`c5` + `c5tool`)**
   - **`c5` 編譯器**：只專注於解析 C 語言並將其編譯為底層 Bytecode，並將輸出封裝為符合規定的 ELF 目的檔 (`.elf`)。
   - **`c5tool`**：一套集成了「虛擬機器 (VM)」與「靜態連結器 (Linker)」的多功能控制台工具（借鑒了 Git/Docker 的動詞子指令設計）。
2. **多檔靜態連結 (Static Linking)**
   - 突破了原版 c5 只能將所有程式碼塞入單一檔案的限制。現在 `c5` 支援編譯含有 `extern` 的檔案，並且能夠透過 `c5tool link` 將多數 `.elf` 目的檔合併並解析外部函式。
3. **全面模組化自舉 (Multi-file Self-Hosting)**
   - 編譯器架構已經過重構與拆分，前端與編碼生成留在 `c5.c`，而龐大的物件檔生成與重定位機制作為後端被抽離至 `c5_elf.c`。並且能利用自己的 Linker 將這兩塊完美連結為一體。
4. **浮點數與 POSIX I/O 擴展**
   - 虛擬機器補齊了浮點數運算（`+`, `-`, `*`, `/`, 相等比較等）與基礎 I/O（`open`, `read`, `write`, `close`），完全擺脫對高階 `libc` 函數庫 (`FILE`) 的依賴。

---

## 🚀 快速上手 (Quick Start)

### 1. 初始化編譯 (Bootstrap)
你只需要標準的 C 編譯器（如 `gcc`）與 `make` 即可完成第一次建置：
```bash
# 一鍵編譯 C5 編譯器與工具鏈
make all
```

### 2. 基本編譯與執行
使用 `c5` 編譯一般的 C 語言原始碼為 ELF 目的檔，然後利用 `c5tool run` 掛載虛擬機器執行：
```bash
./c5 -o fib.elf test/fib.c
./c5tool run fib.elf
```

### 3. 多檔案靜態連結
使用 `c5` 分別把原始碼編譯為目的檔後，使用 `c5tool link` 將它們組裝成一個可執行的 ELF：
```bash
./c5 -o lib_math.elf test/lib_math.c
./c5 -o main_ext.elf test/main_ext.c
./c5tool link -o run.elf lib_math.elf main_ext.elf
./c5tool run run.elf
```

---

## 🧠 終極測試：自我編譯 (Self-Hosting)

這套系統完美支援自舉，你可以透過虛擬機運行自己，再去編譯下一個程式。為了方更自動化測試，我們已經將所有流程寫入了 `Makefile` 中。建議你直接跑一次最完整的整合測試：

```bash
make test_all
```

底下的腳本指令展示了這場「自我編譯大車拼 (`make selfhost`)」背後的發生順序，你會看到它完成以下驚人的循環：

```bash
# 1. 讓 c5 編譯自己的源碼模組
./c5 -o c5.elf c5.c
./c5 -o c5_elf.elf c5_elf.c

# 2. 將模組連結起來，形成虛擬化版本的 c5_final 核心編譯器！
./c5tool link -o c5_final.elf c5.elf c5_elf.elf

# 3. 讓虛擬化版本的 c5_final 去編譯 fib.c
./c5tool run c5_final.elf -o fib2.elf test/fib.c

# 4. 把編譯器 c5tool 也虛擬化，並用虛擬化的 c5tool 去執行虛擬化的 fib！
./c5 -o c5tool.elf c5tool.c
./c5tool run c5tool.elf run fib2.elf
```

## 🏗 資料夾檔案說明
* `c5.c`：C 語言到 Bytecode 的解析與生成。
* `c5_elf.c`：自定義 ELF 64-bit Relocatable Object 寫出模組。
* `c5tool.c`：集成了記憶體動態分配載入、多檔 Symbol/Relocation 連結、以及 30+ 條 Stack-Based 指令的高效虛擬機工具。
* `Makefile`：專案的核心建置系統，涵蓋了 `make all`, `make test_linker`, `make selfhost` 與一鍵大滿貫 `make test_all` 目標。
* `test/`：存放各種基礎 C 程式庫（如 `fib.c`, `float.c` 等）。

## 📈 未來發展方向
目前的架構已經徹底切斷了原版的單檔限制，未來可考慮之發展方向：
1. 為 `c5.c` 前端實作微型的 Preprocessor (前置展開 `#include` 與解析巨集)。
2. 將 Custom ELF 重定位 (`struct Rela`) 規範收斂為標準的 AArch64/x86_64 Rela 標準，與 GNU `objdump` 相容。
3. 為 VM 新增記憶體邊界檢查 (Bounds Checking) 或除錯傾印 (Stack Trace Dump) 強健其沙盒能力。
