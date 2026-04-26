# 編譯器基礎範例測試結果

## 執行方式
```bash
cd _code/系統程式/編譯器基礎
rustc 01-compiler.rs -o 01-compiler
./01-compiler
```

## 執行結果

```
=== Rust 編譯器基礎範例 ===

=== 1. 詞法分析器 (Lexer) ===
原始程式: let x = 42 + 10;
Tokens:
  0: Keyword("let")
  1: Identifier("x")
  2: Equal
  3: Number(42)
  4: Plus
  5: Number(10)
  6: Semicolon
  7: EOF

=== 2. 語法分析器 (Parser) ===
原始程式: let x = 1 + 2 * 3;
AST:
  Assignment { name: "x", value: Binary { op: "+", left: Number(1), right: Binary { op: "*", left: Number(2), right: Number(3) } } }

=== 3. 代碼生成 ===
原始程式: let x = 10 + 20;
生成的 Rust 代碼: let x = (10 + 20);
生成的 Assembly:
; Generated Assembly
section .text
global _start
_start:
    ; syscall exit
    mov eax, 60
    xor edi, edi
    syscall


=== 4. 代碼最佳化 ===
優化前: (2 * 3) + 4 = Binary { op: "+", left: Binary { op: "*", left: Number(2), right: Number(3) }, right: Number(4) }
優化後: Number(10)

=== 5. 表達式解析 ===
'5' => 5
'x + y' => (x + y)
'1 + 2 * 3' => (1 + (2 * 3))
'(1 + 2) * 3' => ((1 + 2) * 3)
編譯器範例完成!
```

## 相關頁面
- [編譯器概念](../系統程式/編譯器.md)
