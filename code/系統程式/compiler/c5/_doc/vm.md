# C5 虛擬機器 (VM) 指令集與核心原理

這份文件記錄了 `c5tool run` 所掛載的 C5 Bytecode 虛擬機器的架構與運作原理。C5 的 VM 是一個**針對 C 語言底層高度特化的堆疊機器 (Stack Machine)**，其設計兼具了精簡與高效。

## 1. 記憶體架構 (Memory Model)

整個 VM 除了執行檔的資料之外，會申請一塊巨大的連續記憶體 (`poolsz`，預設為 1024 * 1024 Bytes) 來充當運行空間。在這塊空間裡，記憶體被大致劃分（邏輯上）為三大區段：
1. **.text**：儲存長整數 (Long Word) 大小的二進位指令流。指標稱為 `t_base`。
2. **.data**：存放純量、常數字串陣列等。指標稱為 `d_base`。
3. **Stack (堆疊)**：從 `poolsz` 的**最高位址向低位址**生長 (Grows downwards)，被用來保存傳遞參數、區域變數、與函數呼叫之 Stack Frame。

### 指標虛擬化 (Pointer Translation)
雖然 C5 保有一定程度對於 Host CPU 實體位址的相容性，但在 `load_and_run` 時，多數寫死在 `.text` 裡的指標都會被處理為**相對於 Segment Base 的 Offset** (`addr < poolsz`)。只有遇到 `malloc()` 返回的系統原生大位址時，VM 才會以實體指標直接操作。

## 2. 虛擬暫存器 (Registers)

C5 僅靠 **4 個長整數 (Long) 暫存器** 就驅動了所有運算：
*   `pc` (Program Counter)：程式計數器，指向 `.text` 陣列中下一個要執行的指令。
*   `sp` (Stack Pointer)：堆疊指標，永遠指向 Stack 目前的最頂端元素。
*   `bp` (Base Pointer)：基底指標 (或 Frame pointer)，用於定址目前函數的區域變數與被傳入的參數。
*   `a`  (Accumulator)：累加器暫存器，**所有運算的算術結果、函數回傳值，都會儲存在 `a` 當中**。

## 3. 指令集架構 (Instruction Set Architecture)

C5 所有的指令都是一個 64-bit 的 `Long`。有些指令會帶有一個伴隨的操作數 (也佔據一個 `Long`)，緊接在 Opcode 之後。

### 3.1 堆疊與變數存取 (Stack & Variables)
*   **`IMM <val>`**：將常數 `<val>` 載入進 `a` 目前值 (`a = *pc++`)。
*   **`LLA <offset>`**：計算區域變數的位址。它會將 Frame Pointer 加上 offset，並把位址存入 `a` (`a = bp + *pc++`)。
*   **`PSH`**：將 `a` 目前的值推入堆疊 (`*--sp = a`)。所有雙元運算前都會頻繁使用這條指令來保留左結算值。

### 3.2 提取與寫入 (Load / Store)
當 `a` 存有一筆指標記憶體位址時，這些指令對該實際位址進行操作：
*   **`LI` / `LC`** (Load Int/Char)：以 `a` 為位址，讀取其值覆蓋掉 `a` （`a = *a` 或取 1 byte）。
*   **`SI` / `SC`** (Store Int/Char)：將 `a` 的值寫入堆疊頂部（會被 pop `*sp++` 出來）所記載的那個位址之中。

### 3.3 算術與邏輯單元 (ALU)
所有的 ALU 指令都是**雙元操作 (Binary Operations)**。
它們的運作邏輯極度一致：**從堆疊中彈出 (Pop) 先前所保存的左值 (Left Operand)，將其與暫存器 `a` (Right Operand) 進行運算，並把最後的結果寫回 `a`**。
*   **算術**：`ADD`, `SUB`, `MUL`, `DIV`, `MOD`
*   **位元**：`OR`, `XOR`, `AND`, `SHL`, `SHR`
*   **比較**：`EQ`, `NE`, `LT`, `GT`, `LE`, `GE`
  *(例如：`ADD` 的動作為 `a = *sp++ + a`)*

### 3.4 流程控制與跳轉 (Control Flow)
*   **`JMP <addr>`**：無條件跳躍到指定的常數 `.text` 索引。
*   **`BZ <addr>` / `BNZ <addr>`**：如果 `a` 是 0 (Branch if Zero) 或 `a` 不是 0 (Branch if Not Zero)，才跨越跳轉；否則繼續往下走。

### 3.5 函數呼叫與棧幀管理 (Function Subroutines)
這是 C5 能夠跑起完整 C 語言遞迴體系的靈魂指令：
*   **`JSR <addr>`** (Jump to Subroutine)：呼叫函數。會先把下一條指令 (`pc + 1`) PSH 存進堆疊中，再跳轉。
*   **`ENT <vars_size>`** (Enter Function)：函數的起手式。會把舊的 `bp` 存起來，並更新目前的 `bp` 到 `sp`。接著將 `sp` 再往下減去預留個位，正式配置區域變數的實體空間。
*   **`ADJ <args_size>`** (Adjust Stack)：從函數呼叫回歸後，由**呼叫者(Caller)**用來修剪掉堆疊中傳遞參數佔去的空間 (`sp = sp + *pc++`)。
*   **`LEV`** (Leave)：準備回歸。將 `sp` 退回 `bp` 的位置，接著恢復原有的 `bp`，最後 Pop 回當初的 `pc`，將主導權交還給 Caller。

### 3.6 新增：浮點數擴充 (Floating Point Extension)
由於原本的 C5 只支援單一 `CHAR / LONG` 整數，新版本加載了透過強迫轉型為 Double (`*(double*)&`) 的 Float 指令：
*   **`LF` / `SF`**：浮點數版本的 Load / Store。
*   **`ITF` / `FTI`**：Integer to Float 與 Float to Integer (用來對待 `a` 的轉換)。
*   **`FADD` / `FMUL` 等**：所有浮點數運算，原理一樣透過堆疊左運算元 `*sp++` 與 `a` 用 `double` 強制轉型運算後，裝模作樣塞回暫存器 `a` 中。

### 3.7 新增：系統呼叫 (System Calls)
用來直接執行編譯器所需的環境建置，這部分也採用查表或硬編碼：
*   `OPEN, READ, WRIT, CLOS`：純 POSIX IO (取代原本 C5 骯髒的 `fopen` wrapper)。
*   `PRTF`：Printf 實作，利用特製的字串解析來遞迴列印堆疊中的長整數以繞過 `va_list`。
*   `MALC, FREE, MSET, MCMP`：Memory 函數庫對接。
*   **`EXIT`**：終止本 VM 執行緒並回傳狀態碼。

## 4. 原始碼與虛擬機器指令對照範例 (Translation Example)

為了幫助理解，這是利用 `./c5 -s test/fib.c` 所輸出的 `fib(n)` 函數，它展示了 C 語言如何被編譯為 C5 的堆疊機器指令：

```c
int f(int n) {
  if (n<=0) return 0;
    ENT  0      // 函數進入：保存目前的 bp，並將 sp 賦值給 bp。沒有預留額外區域變數(大小0)
    LLA  2      // Load Local Address: 參數 n 的位置在 bp + 2（因為 bp 存了舊的 bp，bp+1 存了 return address）
    LI          // Load Integer: 提取 n 的值 (現在 a = n)
    PSH         // Push: 將 a 的值(n)壓入堆疊
    IMM  0      // Load Immediate: 載入常數 0 (a = 0)
    LE          // Less or Equal: 比較 (*sp) <= a，也就是 n <= 0。結果放入 a
    BZ   [標籤] // Branch Zero: 如果 a (比較結果) 為 0 (假)，就跳到後面的程式碼
    IMM  0      // (如果為真) 載入常數 0 作為回傳值 (a = 0)
    LEV         // Leave: 返還 sp 到 bp，並 pop 出原始的 bp 與 pc，跳回呼叫者，回傳值在 a 中

  if (n==1) return 1;
    LLA  2      // 再次取得 n 的位址
    LI          // 提取 n
    PSH 
    IMM  1      // 載入常數 1
    EQ          // Equal: 檢查 n == 1
    BZ   [標籤] // 如果不相等，則跳越
    IMM  1      // 載入常數 1 作為回傳值
    LEV 

  return f(n-1) + f(n-2);
    // 第一個呼叫 f(n-1)
    LLA  2
    LI  
    PSH 
    IMM  1
    SUB         // n - 1
    PSH         // 把準備傳遞給函數的參數 (n-1) 推入堆疊
    JSR  [函數f] // 跳轉呼叫到函數 f (把下一條指令位置壓棧，然後修改 pc)
    ADJ  1      // Adjust: 從函數 f 返還後，丟棄堆疊上傳遞過去的 1 個參數
    PSH         // 將 f(n-1) 的回傳值 a 壓入堆疊，準備最後的加法

    // 第二個呼叫 f(n-2)
    LLA  2
    LI  
    PSH 
    IMM  2
    SUB         // n - 2
    PSH 
    JSR  [函數f]
    ADJ  1      // 丟棄參數
    
    // 將最後兩筆結果相加
    ADD         // a = *sp++ + a，也就是將 f(n-1) 加上 f(n-2) 的結果放入歸零的 a
    LEV         // 以 a 存著的相加結果離開，回傳給上一層
}
    LEV         // 函數預設結束出口 (編譯器防護)
```

從這個精緻的 `fib` 範例中，可以強烈感受到 C5 編譯器的特性：
* **所有的參數存取** 都轉譯為了對 Stack Frame 的 offset 尋址 (`LLA 2` -> `LI`)。
* **所有的條件判斷** 都化為了將左值推進堆疊 (`PSH`)，讀取右值，算完後做 Branching 跳轉 (`BZ`)。
* **所有的函數呼叫** 都對應到塞參數 (`PSH`) -> 跳越 (`JSR`) -> 清除參數 (`ADJ`)。

## 結語
在 C5 裡面跑 C 語言的奧義就在於：C 語言的一切都可以被簡單地還原為**取位址 (LLA)** -> **壓棧 (PSH)** -> **取值 (LI)** -> **ALU運算 (ADD)** 的完美組合。這些指令搭配 4 個基礎 Register 的極簡設計，展現了最洗鍊的圖靈完備之美。
