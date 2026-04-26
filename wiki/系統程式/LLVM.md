# LLVM

LLVM（Low Level Virtual Machine）是一個強大且模組化的編譯器框架，由 Chris Lattner 於 2000 年在伊利諾大學開發。LLVM 的設計理念是提供一個可重用的編譯器基礎設施，能夠支援多種前端語言和多種目標平臺。與傳統的編譯器不同，LLVM 採用了分層的設計，中間表示式（LLVM IR）是其核心，這使得優化和目標碼生成可以在統一的框架下進行。LLVM 徹底改變了編譯器技術的發展方式，成為學術研究和工業應用的重要工具。

## LLVM 的核心概念

### 編譯器架構

```
┌─────────────────────────────────────────────────────────────┐
│                    LLVM 編譯器架構                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   原始程式碼                                                  │
│        │                                                     │
│        ▼                                                     │
│   ┌─────────────┐                                           │
│   │   Frontend  │  (Clang, Swift, Rust 前端等)              │
│   │   (Lexer)   │  - 詞法分析                                │
│   │   (Parser)  │  - 語法分析                                │
│   │   (AST)     │  - 語意分析                                │
│   └──────┬──────┘                                           │
│          │                                                  │
│          ▼                                                  │
│   ┌─────────────┐                                           │
│   │  LLVM IR    │  中間表示式                                │
│   │   (SSA)     │  - 靜態單一賦值                            │
│   └──────┬──────┘                                           │
│          │                                                  │
│          ▼                                                  │
│   ┌────────────────────────────────────────────────────────┐│
│   │              LLVM Optimizer (優化通道)                  ││
│   │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐    ││
│   │  │ Constant │ │Dead Code│ │  Inline  │ │Loop Opt │    ││
│   │  │ Folding  │ │Eliminate │ │          │ │         │    ││
│   │  └──────────┘ └──────────┘ └──────────┘ └──────────┘    ││
│   └────────────────────────────────────────────────────────┘│
│          │                                                  │
│          ▼                                                  │
│   ┌─────────────┐                                           │
│   │   Backend   │  (x86, ARM, RISC-V, GPU 後端等)           │
│   │   CodeGen   │  - 指令選擇                               │
│   │   RegAlloc  │  - 暫存器分配                             │
│   │   Scheduling│  - 指令排程                               │
│   └──────┬──────┘                                           │
│          │                                                  │
│          ▼                                                  │
│   目的碼 (Object File)                                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 為什麼使用 LLVM？

```
┌─────────────────────────────────────────────────────────────┐
│                  LLVM 的優勢                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   1. 模組化設計                                              │
│      - 前端、優化器、後端完全分離                             │
│      - 可獨立替換或升級任一模組                              │
│                                                             │
│   2. 多語言支援                                               │
│      - C, C++, Objective-C (Clang)                          │
│      - Swift, Rust, Julia, Fortran                           │
│      - 任何可以編譯到 LLVM IR 的語言                         │
│                                                             │
│   3. 多目標平臺                                              │
│      - x86, ARM, RISC-V, MIPS, PowerPC                      │
│      - GPU (NVIDIA PTX, AMD GCN)                            │
│      - WebAssembly, JavaScript                               │
│                                                             │
│   4. SSA 表示                                                │
│      - LLVM IR 使用靜態單一賦值形式                          │
│      - 方便進行全域分析和優化                                │
│                                                             │
│   5. 可擴展性                                                 │
│      - 插件式 Pass 系統                                      │
│      - 自訂目標特定優化                                      │
│                                                             │
│   6. 成熟穩定                                                │
│      - 廣泛使用 (iOS, Android, Chrome 等)                  │
│      - 活躍的社區和開發                                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

## LLVM IR（中間表示式）

### 基本語法

```llvm
; 模組結構
; Module = 翻譯單元，由函式和全域變數組成

; -----------------------------------------------------------
; 註釋以 ; 開頭
; -----------------------------------------------------------

; 全域變數 (@ 開頭)
@global_var = global i32 42
@str = constant [13 x i8] c"Hello, World!\00"

; 函式定義 (define 關鍵字)
; 返回類型 @函式名稱 (參數列表) { 函式主體 }

; 簡單的加法函式
define i32 @add(i32 %a, i32 %b) {
entry:
    ; %result = a + b
    %result = add i32 %a, %b
    ret i32 %result
}

; 條件分支
define i32 @max(i32 %a, i32 %b) {
entry:
    %cmp = icmp sgt i32 %a, %b
    br i1 %cmp, label %if_true, label %if_false

if_true:
    ret i32 %a

if_false:
    ret i32 %b
}

; 迴圈
define i32 @sum(i32 %n) {
entry:
    %i = alloca i32
    store i32 0, i32* %i
    
    %sum = alloca i32
    store i32 0, i32* %sum
    
    br label %loop

loop:
    %i_val = load i32, i32* %i
    %cond = icmp slt i32 %i_val, %n
    br i1 %cond, label %body, label %end

body:
    %sum_val = load i32, i32* %sum
    %new_sum = add i32 %sum_val, %i_val
    store i32 %new_sum, i32* %sum
    
    %i_next = add i32 %i_val, 1
    store i32 %i_next, i32* %i
    br label %loop

end:
    %result = load i32, i32* %sum
    ret i32 %result
}
```

### 類型系統

```llvm
; 基本類型
i1              ; 1 位元整數 (布林)
i8              ; 8 位元整數
i16             ; 16 位元整數
i32             ; 32 位元整數
i64             ; 64 位元整數
i128            ; 128 位元整數

; 浮點類型
half            ; 16 位元浮點
float           ; 32 位元浮點
double          ; 64 位元浮點
fp128           ; 128 位元浮點

; 向量類型
<4 x i32>       ; 4 元素向量
<8 x float>     ; 8 元素浮點向量

; 指標類型
i32*            ; 指向 i32 的指標
i8*             ; 位元組指標 (C char*)
void ()*        ; 函式指標

; 陣列類型
[10 x i32]      ; 10 元素 i32 陣列

; 結構體類型
%MyStruct = type { i32, float, i8* }

; 定義結構體
%Point = type { double, double }
define %Point @create_point(double %x, double %y) {
    %p = alloca %Point
    %x_ptr = getelementptr %Point, %Point* %p, i32 0, i32 0
    store double %x, double* %x_ptr
    %y_ptr = getelementptr %Point, %Point* %p, i32 0, i32 1
    store double %y, double* %y_ptr
    ret %Point* %p
}
```

### 常用指令

```llvm
; -----------------------------------------------------------
; 算術指令
; -----------------------------------------------------------
%r1 = add i32 %a, %b         ; 加法
%r2 = sub i32 %a, %b         ; 減法
%r3 = mul i32 %a, %b         ; 乘法
%r4 = sdiv i32 %a, %b        ; 有符號除法
%r5 = urem i32 %a, %b        ; 無符號餘數
%r6 = shl i32 %a, %b         ; 左移
%r7 = lshr i32 %a, %b        ; 邏輯右移
%r8 = ashr i32 %a, %b        ; 算術右移
%r9 = and i32 %a, %b         ; 位元 AND
%r10 = or i32 %a, %b         ; 位元 OR
%r11 = xor i32 %a, %b        ; 位元 XOR

; -----------------------------------------------------------
; 比較指令
; -----------------------------------------------------------
%cmp1 = icmp eq i32 %a, %b   ; 等於
%cmp2 = icmp ne i32 %a, %b   ; 不等於
%cmp3 = icmp slt i32 %a, %b  ; 有符號小於
%cmp4 = icmp sle i32 %a, %b  ; 有符號小於等於
%cmp5 = icmp sgt i32 %a, %b  ; 有符號大於
%cmp6 = icmp ugt i32 %a, %b  ; 無符號大於
%cmp7 = fcmp oeq float %a, %b ; 浮點等於
%cmp8 = fcmp uno float %a, %b ; 浮點非序 (NaN)

; -----------------------------------------------------------
; 記憶體指令
; -----------------------------------------------------------
%ptr = alloca i32             ; 棧上分配
store i32 %val, i32* %ptr      ; 儲存
%val2 = load i32, i32* %ptr   ; 載入

; 結構體指標操作
%elem = getelementptr %Struct, %Struct* %s, i32 0, i32 1

; -----------------------------------------------------------
; 控制流指令
; -----------------------------------------------------------
br label %dest                 ; 無條件分支
br i1 %cond, label %true, label %false  ; 條件分支

%r = phi i32 [ %v1, %label1 ], [ %v2, %label2 ]  ; PHI 節點

ret i32 %val                   ; 返回
ret void                       ; 無返回值返回

; -----------------------------------------------------------
; 函式呼叫
; -----------------------------------------------------------
%result = call i32 @add(i32 %a, i32 %b)
call void @printf(i8* %str)
%ptr = call i32* @malloc(i64 %size) nounwind

; -----------------------------------------------------------
; 轉換指令
; -----------------------------------------------------------
%a_i = sext i8 %a to i32       ; 符號擴展
%b_i = zext i8 %b to i32      ; 零擴展
%f = sitofp i32 %i to float   ; 整數到浮點
%i = fptosi float %f to i32   ; 浮點到整數
%ptr = inttoptr i32 %i to i32* ; 整數到指標
```

## Clang：C/C++ 編譯器前端

```bash
# 使用 Clang 編譯
clang -O2 -c source.c -o source.o

# 生成 LLVM IR
clang -S -emit-llvm source.c -o source.ll

# 生成優化後的 IR
clang -S -O3 -emit-llvm source.c -o source_opt.ll

# 執行 Bitcode
clang -c source.bc -o source.o

# 連結多個 Bitcode 文件
llvm-link a.bc b.bc -o combined.bc
```

```c
// C 程式碼
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

# 生成的 LLVM IR
define i32 @factorial(i32 %n) {
entry:
  %cmp = icmp sle i32 %n, 1
  br i1 %cmp, label %if_true, label %if_false

if_true:
  ret i32 1

if_false:
  %sub = sub i32 %n, 1
  %call = call i32 @factorial(i32 %sub)
  %mul = mul i32 %n, %call
  ret i32 %mul
}
```

## 優化通道（Optimization Passes）

### 常用優化

```python
# LLVM 優化分類
optimization_passes = {
    "常數相關優化": {
        "Constant Folding": "編譯時計算常數表達式",
        "Constant Propagation": "用常數替換變數",
        "示例": "x = 3 + 4 → x = 7"
    },
    
    "死碼消除": {
        "Dead Code Elimination": "移除不會執行的程式碼",
        "Dead Store Elimination": "移除無用的儲存操作",
        "示例": "if (false) { x = 5; }"
    },
    
    "內聯優化": {
        "Function Inlining": "將函式呼叫內聯到呼叫點",
        "Example": "sum(average(x), average(x)) → average(x)*2"
    },
    
    "迴圈優化": {
        "Loop Unrolling": "展開迴圈減少分支",
        "Loop Invariant Code Motion": "將不變程式碼移出迴圈",
        "Loop Fusion": "合併相鄰迴圈",
        "Loop Vectorization": "向量化"
    },
    
    "全局優化": {
        "Global Value Numbering": "消除冗餘計算",
        "Common Subexpression Elimination": "消除重複計算"
    }
}
```

### 優化層級

```bash
# -O0: 無優化 (調試友好)
clang -O0 -g source.c -o debug_bin

# -O1: 基本優化 (快速編譯)
clang -O1 source.c -o bin

# -O2: 標準優化 (推薦)
clang -O2 source.c -o bin

# -O3: 激進優化
clang -O3 source.c -o bin

# -Os: 大小優化
clang -Os source.c -o bin

# -Oz: 最小化大小
clang -Oz source.c -o bin

# -Ofast: 快速執行 (可能放寬浮點精度)
clang -Ofast source.c -o bin
```

### 自訂 Pass 開發

```cpp
// LLVM Pass 範例結構
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {
    struct MyPass : public FunctionPass {
        static char ID;
        MyPass() : FunctionPass(ID) {}
        
        bool runOnFunction(Function &F) override {
            errs() << "Running on function: " << F.getName() << "\n";
            
            // 遍歷基本區塊
            for (auto &BB : F) {
                // 遍歷指令
                for (auto &I : BB) {
                    // 分析或轉換指令
                }
            }
            
            return false; // 是否修改了函式
        }
    };
}

char MyPass::ID = 0;
static RegisterPass<MyPass> X("my-pass", "My custom pass");

# 編譯和使用
clang -Xclang -load -Xclang /path/to/libmypass.so \
       -mllvm -my-pass source.c -o bin
```

## 後端目標

### 支援的目標平臺

```python
# LLVM 支援的目標
llvm_targets = {
    "x86": {
        "類型": "桌面/伺服器",
        "供應商": "Intel, AMD"
    },
    
    "ARM": {
        "類型": "行動/嵌入式",
        "裝置": "手機, 平板, 微控制器"
    },
    
    "RISC-V": {
        "類型": "開源處理器",
        "應用": "IoT, 學習, 研究"
    },
    
    "MIPS": {
        "類型": "網路設備"
    },
    
    "PowerPC": {
        "類型": "伺服器/超級電腦"
    },
    
    "WebAssembly": {
        "類型": "瀏覽器/邊緣"
    },
    
    "NVPTX": {
        "類型": "NVIDIA GPU"
    },
    
    "AMDGPU": {
        "類型": "AMD GPU"
    }
}
```

### 目標特定優化

```llvm
; 目標特定的屬性
target datalayout = "e-m:e-i64:64-f32:32-f64:64-v128:64:128-n32:64"
target triple = "x86_64-pc-linux-gnu"

; SIMD 向量化提示
define void @vectorized_add(<4 x float>* %a, <4 x float>* %b, <4 x float>* %c) #0 {
entry:
    %a_vec = load <4 x float>, <4 x float>* %a
    %b_vec = load <4 x float>, <4 x float>* %b
    %c_vec = fadd <4 x float> %a_vec, %b_vec
    store <4 x float> %c_vec, <4 x float>* %c
    ret void
}

attributes #0 = { "target-features"="+avx2,+fma" }
```

## 工具生態

```bash
# 常用 LLVM 工具
tools = {
    "opt": "執行優化 passes",
    "llc": "編譯 LLVM IR 到組合語言",
    "lli": "直譯 LLVM IR",
    "llvm-dis": "反組譯 Bitcode",
    "llvm-as": "組譯 LLVM IR",
    "llvm-link": "連結多個 Bitcode",
    "llvm-extract": "提取模組",
    "llvm-bcanalyzer": "分析 Bitcode",
    "clang": "C/C++ 編譯器",
    "clang-format": "程式碼格式化",
    "clang-tidy": "靜態分析",
    "lldb": "除錯器"
}
```

```bash
# 完整編譯流程範例
# 1. C → LLVM IR
clang -S -emit-llvm -O2 hello.c -o hello.ll

# 2. 優化
opt -O3 hello.ll -o hello_opt.ll

# 3. 目標碼生成
llc -O3 -march=x86-64 hello_opt.ll -o hello.s

# 4. 組譯
as -o hello.o hello.s

# 5. 連結
ld -o hello hello.o -lpthread

# 或者一步完成
clang -O2 -o hello hello.c -lpthread

# 反組譯查看
llvm-dis hello.bc -o -
```

## 相關主題

- [編譯器](編譯器.md) - 編譯器基礎
- [C語言](C語言.md) - C 程式設計
- [RISC-V](RISC-V.md) - RISC-V 後端支援
- [組合語言](組合語言.md) - 低階程式設計