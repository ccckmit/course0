# C4v1.0 設計藍圖與架構解析 (Design Document)

這份文件旨在詳細記錄將原版的 **C in four functions (c5)** 微型直譯器重構成為**具備連結器 (Linker) 與多檔案物件生成能力**的編譯工具鏈 (`c5` + `c5tool`) 時的核心架構設計考量。

## 1. 架構演進哲學：從「單檔極簡」到「模組化基建」
原版的 c5 編譯器為了極致的最小化，採用了單檔直譯架構（約 4 個主要函數、不具備 Preprocessor、缺乏真正的 Link 概念）。雖然非常優雅，但也意味著它只適合做為展現語言特性的「玩具」，所有源碼被強制局限在單一模組內。

在 v1.0 架構中，我們的首要目標是**打破單檔限制**並走向軟體工程中標準的「分離關注點 (Separation of Concerns)」模式：
- 將 **編譯過程 (Compile)** 與 **執行過程 (Execute)** 從同一個執行檔中剝離。
- 引入 **物件檔 (Object File)** 作為中繼交換格式。
- 打造專屬的 **靜態連結器 (Static Linker)** 以實現真正的跨檔案共享函式。

## 2. 核心架構分層

### 2.1. 語言前端與指令生成 (`c5.c`)
- **功能**：扮演 `Lexer`、`Parser` 以及 `Bytecode Generator`。
- **設計特性**：採用純粹的 Recursive Descent (遞迴下降) 演算法。它是一次性 (One-pass) 的編譯，解析到語法節點時立即吐出對應的 Stack-based 指令串流。遇到跨檔未定義的函式呼叫時（藉由 `extern` 標示），會將其註冊在符號表內並等待連結器解析，而不會像舊版一樣直接報錯。

### 2.2. ELF 寫出後端 (`c5_elf.c`)
- **功能**：標準化與包裝執行檔（相當於 `Object Emitter`）。
- **設計特性**：從 `c5.c` 抽離後，`c5_elf.c` 完全不在乎 C 語言的語法，只專注於建構 64-bit ELF 格式的 `Header`, `Section Headers` (`.text`, `.data`, `.symtab`, `.strtab`, 等) 以及處理自製的 `Rela` 重定位表。

### 2.3. 多檔靜態連結器 (`c5tool link`)
- **功能**：合併多個 ELF 目的檔，並填補 (Patch) 指標跳躍與資料記憶體位置。
- **運作階段**：
  1. **Pass 1 (Collect)**：掃描所有傳入的 ELF 檔案，將所有遇到之定義（函數與資料）匯聚到一張 Global Symbol Table 中。
  2. **Pass 2 (Relocate)**：將分散的 `.text` 和 `.data` 對齊組合成長長的串流（透過記錄各模組 Base Address）。再根據每一份 `.rela` 表，將佔位符修補為重新算出的絕對位置。
  3. **Output**：最終產出一個完全 resolved、只留有一個 main 進入點的純淨執行檔。

### 2.4. 虛擬機器 VM (`c5tool run`)
- **功能**：載入執行檔與還原記憶體環境，精準翻譯中介碼狀態。
- **設計特性**：
  - 以極精簡的 Stack machine 建構 (`pc`, `sp`, `bp`, `a` 四個基礎暫存器) 。
  - 核心擴展了大量原版沒有的浮點數運算 Opcodes (如 `FADD`, `FGT`, `ITF`)。
  - 對底層依賴降至最低，移除繁瑣的 libc FILE IO 包裝，只透過 POSIX Standard syscalls (`open`, `read`, `write`, `close`) 和 host os 直接對接。

## 3. 重定位與符號設計 (Relocation & Symbols)
為了以最精簡的 C 寫出 Linker，目前專案並沒有完全跟進繁瑣的 `x86_64` 或 `AArch64` Rela 標準，而是實作了符合 C5 歷史架構的極簡重定位三劍客 `struct Rela`：
* **`RELA_DATA = 1`**：這個中介碼位址需要被填入指向 `.data` 區段的 offset。在連結時，若該符號是跨檔定義，會用 Global Symbol 表裡的目標偏移加上目前檔案的 Base address 寫死，使得變數共享可行。
* **`RELA_FUNC = 2`**：用於函式指標傳遞等場景。編譯器需在相應的 long word 給予被參照的 `.text` 索引。
* **`RELA_JMP = 3`**：特別留給 `JSR`, `JMP`, `BZ`, `BNZ` 等跳轉指令，會在 `c5tool link` 期間校正跨物件檔呼叫（解決 `extern void foo();` 到對應實作區塊精準跳轉的難題）。

## 4. 多重模組自我編譯 (Self-Hosting Lifecycle)
這套架構最大的驕傲即為 **全組件虛擬化驗證**，能成功完成以下迴圈：
1. **Host GCC Compiler** -> 產生 Native `c5` 與 Native `c5tool`。
2. **Native c5 Compiler** -> 解析讀取 `c5.c` 和 `c5_elf.c` 源碼，產生出 bytecode 版本的 `c5.elf` / `c5_elf.elf`。
3. **Native c5tool Linker** -> 把 `c5.elf` 和 `c5_elf.elf` 動態拼裝，生成了虛擬機環境下的微型編譯器大腦 `c5_final.elf`。
4. **自舉驗證 (Bootstrapping test)** -> 以 Native `c5tool` 運行虛擬的 `c5_final.elf`。此時「虛擬化編譯器」具備完整語言分析和輸出能力，能夠正確編譯你的使用者專案。

## 5. 架構待解之挑戰 (Future Design Proposals)
雖然解決了多檔連結問題，但考量未來彈性，依然有幾個架構節點可以再進化：
1. **微型前置處理器 (Micro-Preprocessor)**：由於現在可以把程式分檔撰寫，缺乏 `#include` 展開使得每一個 `.c` 都需要寫大把的 `extern` 來維持相容，手感仍然生硬。
2. **標準化 ELF 對接**：如果未來的發展目標是向 GNU `objdump`，甚至是外部 C 語言 Standard Library 對齊，那麼我們專屬的 `RELA_DATA` 必須轉換成官方的 `R_X86_64_PC32` 等標準型別，也必須實作 Section String Names 等對應要求。
3. **VM 除錯與沙盒機制**：考慮在虛擬機器指令派發階段，加入暫存器狀態檢查器或記憶體界線攔截 (Bounds Checking Layout)，甚至打造具備 `Trace dump` 的微型作業系統底層。
