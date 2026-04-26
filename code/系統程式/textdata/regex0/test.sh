#!/bin/bash

# 定義顏色
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# 1. 編譯 C 程式
echo "正在編譯 regex0.c ..."
gcc -o regex0 regex0.c
if [ $? -ne 0 ]; then
    echo -e "${RED}編譯失敗！${NC}"
    exit 1
fi
echo "編譯成功！開始測試..."
echo "------------------------------------------------"

# 2. 定義測試函數
# 參數: $1=Regex, $2=Text, $3=預期結果(0代表成功, 1代表失敗)
run_test() {
    local regex="$1"
    local text="$2"
    local expected="$3"

    # 執行程式並將標準輸出丟棄，只抓取 Return Code ($?)
    ./regex0 "$regex" "$text" > /dev/null
    local result=$?

    if [ "$result" -eq "$expected" ]; then
        echo -e "${GREEN}[PASS]${NC} Regex: /$(printf '%-8s' "$regex")/ Text: \"$text\""
    else
        echo -e "${RED}[FAIL]${NC} Regex: /$(printf '%-8s' "$regex")/ Text: \"$text\" (預期: $expected, 實際: $result)"
    fi
}

# 3. 測試案例 (Test Cases)

# -- 一般字元比對 --
run_test "abc" "abc" 0
run_test "abc" "xabcy" 0
run_test "abc" "ab" 1

# -- '.' 任意字元比對 --
run_test "a.c" "abc" 0
run_test "a.c" "axc" 0
run_test "a.c" "ac" 1

# -- '^' 開頭比對 --
run_test "^abc" "abcdef" 0
run_test "^abc" "xabcdef" 1

# -- '$' 結尾比對 --
run_test "xyz$" "abxyz" 0
run_test "xyz$" "abxyzq" 1

# -- '*' 零次或多次比對 --
run_test "ab*c" "ac" 0       # b 出現 0 次
run_test "ab*c" "abc" 0      # b 出現 1 次
run_test "ab*c" "abbbc" 0    # b 出現 3 次
run_test "ab*c" "abxc" 1     # 中間有 x

# -- 複合比對 --
run_test "^a.*b$" "ab" 0
run_test "^a.*b$" "axxxb" 0
run_test "^a.*b$" "axxxbc" 1
run_test ".*" "anything" 0

echo "------------------------------------------------"
echo "測試完畢。"

# 測試結束後清理執行檔
rm -f regex0