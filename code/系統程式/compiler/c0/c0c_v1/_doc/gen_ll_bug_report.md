# Bug Report: test_gen_ll.sh

## 問題描述

執行 `test_gen_ll.sh` 時所有 12 個測試案例都失敗，出現 **Bus error: 10**。

## 錯誤輸出

```
=== Stage 4: LLVM IR code generation test ===
    lli:   lli
    clang: Homebrew clang version 22.1.2

[0] Building c0c...
    OK: c0c compiled

[1] Stage 3 regression...
    OK: Stage 3 still passes

[2] Running test cases...
./test_gen_ll.sh: line 63: 34190 Bus error: 10           "$LLI" "$ll_file" 2> /dev/null
... (重複 12 次)

Results: 0 passed, 12 failed out of 12 tests
```

每個測試案例的 c0c 退出碼為 138 (即 Signal 10 - SIGBUS)。

## 根本原因

c0c 編譯器產生的 LLVM IR 包含硬編碼的目標平台：

```llvm
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"
```

但測試環境為 **Darwin arm64** (macOS on Apple Silicon)：

```
$ uname -s -m
Darwin arm64
```

lli 無法執行為不同平台生成的 IR，導致 Bus error。

## 問題位置

`src/codegen_ll.c:1138` 硬編碼了 target triple：

```c
fprintf(cg->out,"target triple = \"x86_64-pc-linux-gnu\"\n\n");
```

## 解決方向

- 讓 c0c 根據執行環境自動偵測或接受命令行參數來指定 target triple
- 或者使用較通用的 target 如 `aarch64-apple-darwin` 或 `arm64-apple-darwin`
