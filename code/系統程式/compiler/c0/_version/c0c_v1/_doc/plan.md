本專案目標是寫出一個可以產生 LLVM IR .ll 檔的 C 語言編譯器 (c0c)

## 設計法則

1. 分階段驗證，每階段都要和 clang 比較 (產生的 .c 檔，產生的 .ll 檔）完全一致，才算通過。
2. 請善用 sanitize , valgrind 等工具，解決記憶體和指標亂指的問題
3. 務必做到沒有 warning ，有 warning 都要先解掉
4. 先不支援 macro, 避免用 #define, #ifdef, ....

## 階段

1. 先寫出完整 C 語言 EBNF 語法，放在 _doc/EBNF.md 中 (包含 struct, for, while, if, function, call, ++, +=, ==, ...., 指標)
2. 根據該 EBNF 寫出 lexer.c ，並且寫出 lexer_test() 函數，該 lexer 要能 lex 自己。通過 test_lexer.sh 的測試。
3. 根據該 EBNF 寫出 parser.c, lexer.c, ast.c ，並且寫出 parser_test() 函數，該 parser 要能 parse 自己 (parser.c, lexer.c, ast.c)。創建出 ast，然後寫出將 ast 轉回 C 語言的函數 ast_to_c() ，用 ast_to_c(file) 的 codegen_c.c 程式，將 parser.c, lexer.c, ast.c 轉回 C 語言，得到 parser.ast.c, lexer.ast.c, ast.ast.c ，並比較是否和 parser.c, lexer.c, ast.c 一樣。（通過 test_genc.sh 的測試）
4. 寫出 codegen_ll.c 程式，包含 ast_to_ll(file) 函數，然後將 c0c <file.c> -o <file_c0c.ll> 的結果，與 clang -S -emit-llvm -O0 <file.c> -o <file_clang.ll> 的結果，務必確保 file_c0c.ll 和 file_clang.ll 兩者壹模壹樣，才算通過 test_gen_ll.sh 的測試。
