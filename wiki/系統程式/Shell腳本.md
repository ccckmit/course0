# Shell 腳本

Shell 是 Unix 系統的命令列解釋器，用於執行命令和管理系統。Shell 腳本是將多個命令寫入檔案中以自動執行，是系統管理和自動化任務的重要工具。

## 常見 Shell 種類

- **Bash** (Bourne Again Shell)：Linux 預設，功能豐富
- **Zsh**：增強的 Bash，補全功能強
- **Fish**：使用者友好的現代 Shell
- **Sh**：POSIX 標準的原始 Bourne Shell

## 基本概念

### Shebang

```bash
#!/bin/bash
# 這告訴系統用 bash 執行這個腳本

#!/usr/bin/env python3
# 使用 env 查找 Python，適用於不同系統
```

### 執行腳本

```bash
# 直接執行（需要執行權限）
chmod +x script.sh
./script.sh

# 用解釋器執行
bash script.sh
sh script.sh

# 帶除錯執行
bash -x script.sh  # 印出每條命令及參數
bash -v script.sh  # 印出讀取的原始行
```

## 變數

### 定義和使用

```bash
# 定義變數（等號兩側不能有空白）
name="Alice"
age=30
pi=3.14159

# 使用變數（$ 展開）
echo "Name: $name"
echo "Age: $age"

# 字串連接
greeting="Hello, $name!"

# 雙引號 vs 單引號
echo "$name"    # 展開變數：Alice
echo '$name'    # 不展開：$name

# 指令替換
current_date=$(date)
files=$(ls)
timestamp=`date +%Y%m%d`  # 傳統語法
```

### 特殊變數

```bash
$0      # 腳本名稱
$1-$9   # 前 9 個參數
${10}   # 第 10 個參數
$#      # 參數個數
$@      # 所有參數（分開）
$*      # 所有參數（合併）
$$      # 目前 Shell 的 PID
$?      # 上一個命令的退出狀態
$-      # 目前 Shell 的選項
$_      # 上一個命令的最後一個參數
```

### 陣列

```bash
# 定義陣列
fruits=("apple" "banana" "cherry")
numbers=(1 2 3 4 5)

# 存取元素
echo ${fruits[0]}  # apple
echo ${fruits[@]}  # 所有元素
echo ${#fruits[@]} # 元素個數

# 新增元素
fruits+=("orange")
fruits[3]="grape"

# 切片
echo ${fruits[@]:1:2}  # banana cherry

# 關聯陣列（需要 Bash 4+）
declare -A capitals
capitals[tw]="Taipei"
capitals[jp]="Tokyo"
capitals[kr]="Seoul"
```

## 條件判斷

### test 命令

```bash
# 數值比較
[ $a -eq $b ]   # 等於
[ $a -ne $b ]   # 不等於
[ $a -lt $b ]   # 小於
[ $a -le $b ]   # 小於等於
[ $a -gt $b ]   # 大於
[ $a -ge $b ]   # 大於等於

# 字串比較
[ "$s1" = "$s2" ]   # 等於
[ "$s1" != "$s2" ]  # 不等於
[ -z "$s" ]         # 為空
[ -n "$s" ]         # 不為空
[ "$s" == "pattern" ]  # 模式匹配

# 檔案測試
[ -e "$file" ]   # 存在
[ -f "$file" ]   # 普通檔案
[ -d "$dir" ]    # 目錄
[ -r "$file" ]   # 可讀
[ -w "$file" ]   # 可寫
[ -x "$file" ]   # 可執行
[ -L "$file" ]   # 符號連結
[ "$f1" -nt "$f2" ]  # f1 比 f2 新
```

### if 語句

```bash
# 基本語法
if [ condition ]; then
    commands
fi

# if-else
if [ "$age" -ge 18 ]; then
    echo "Adult"
else
    echo "Minor"
fi

# elif
if [ "$grade" -ge 90 ]; then
    echo "A"
elif [ "$grade" -ge 80 ]; then
    echo "B"
elif [ "$grade" -ge 70 ]; then
    echo "C"
else
    echo "F"
fi

# 巢狀條件
if [ -f "$file" ]; then
    if [ -r "$file" ]; then
        cat "$file"
    else
        echo "Cannot read file"
    fi
fi
```

### case 語句

```bash
case "$variable" in
    pattern1)
        commands
        ;;
    pattern2)
        commands
        ;;
    pattern3|pattern4)
        commands
        ;;
    *)
        default commands
        ;;
esac

# 範例
read -p "Continue? (y/n) " answer
case "$answer" in
    [yY]|[yY][eE][sS])
        echo "Yes!"
        ;;
    [nN]|[nN][oO])
        echo "No"
        exit 1
        ;;
    *)
        echo "Invalid input"
        ;;
esac
```

## 迴圈

### for 迴圈

```bash
# 基本 for
for i in 1 2 3 4 5; do
    echo "Number: $i"
done

# C 風格 for
for ((i=0; i<10; i++)); do
    echo "$i"
done

# 迭代字串
for word in "hello world" "foo bar"; do
    echo "$word"
done

# 迭代檔案
for file in *.txt; do
    echo "Processing $file"
done

# 迭代命令輸出
for line in $(cat file.txt); do
    echo "$line"
done

# 讀取一行
while IFS= read -r line; do
    echo "$line"
done < file.txt
```

### while 迴圈

```bash
# 基本 while
count=0
while [ $count -lt 5 ]; do
    echo "Count: $count"
    count=$((count + 1))
done

# 讀取使用者輸入
echo "Enter 'quit' to exit"
while true; do
    read -p "Command: " cmd
    if [ "$cmd" = "quit" ]; then
        break
    fi
    echo "You entered: $cmd"
done

# 讀取檔案
while IFS= read -r line; do
    process "$line"
done < "$filename"
```

### 迴圈控制

```bash
# break - 跳出迴圈
for i in {1..10}; do
    if [ $i -eq 5 ]; then
        break
    fi
    echo "$i"
done

# continue - 跳過本次迭代
for i in {1..10}; do
    if [ $((i % 2)) -eq 0 ]; then
        continue
    fi
    echo "$i"
done
```

## 函數

```bash
# 定義函數
function greet {
    echo "Hello, $1!"
}

# 或
greet() {
    echo "Hello, $1!"
}

# 呼叫
greet "Alice"

# 返回值
function get_sum {
    local result=$(($1 + $2))
    echo "$result"
}

sum=$(get_sum 3 5)
echo "Sum: $sum"

# 返回狀態
function check_file {
    if [ -f "$1" ]; then
        return 0  # 成功
    else
        return 1  # 失敗
    fi
}

if check_file "test.txt"; then
    echo "File exists"
fi
```

## 常用命令

### 字串操作

```bash
# 長度
${#string}

# 子字串
${string:position}        # 從 position 開始
${string:position:length} # 取出 length 個字元

# 刪除
${string#pattern}   # 最短匹配開頭
${string##pattern} # 最長匹配開頭
${string%pattern}    # 最短匹配結尾
${string%%pattern} # 最長匹配結尾

# 替換
${string/pattern/replacement} # 替換第一個
${string//pattern/replacement} # 替換所有

# 大小寫
${string^^}  # 轉大寫
${string,,}  # 轉小寫
```

### 算術運算

```bash
# 算術展開
result=$((a + b))
result=$((a - b))
result=$((a * b))
result=$((a / b))
result=$((a % b))

# 複合賦值
((a++))
((a--))
((a += 5))
((a *= 2))

# let 命令
let "a = a + 1"
let "b = b * 2"
```

### 命令組合

```bash
# 序列執行
command1; command2; command3

# AND（前面成功才執行後面）
command1 && command2 && command3

# OR（前面失敗才執行後面）
command1 || command2 || command3

# 管道
command1 | command2 | command3

# 命令群組
(commands)    # 在子 Shell 中執行
{ commands; } # 在目前 Shell 中執行
```

## 輸入輸出

### read

```bash
# 基本讀取
read name
echo "Hello, $name"

# 帶提示
read -p "Enter your name: " name

# 讀取密碼（不回顯）
read -sp "Enter password: " password

# 讀取多個值
read -p "Enter name and age: " name age

# 讀取陣列
read -a fruits
echo "${fruits[0]}"
```

### echo 和 printf

```bash
# echo
echo "Hello World"
echo -n "No newline"  # 不換行
echo -e "Tab:\t newline:\n"

# printf（C 風格）
printf "Name: %s, Age: %d\n" "$name" $age
printf "%05d\n" 42     # 補零：00042
printf "%.2f\n" 3.14159  # 小數：3.14
```

## 腳本範例

### 批次重新命名

```bash
#!/bin/bash
# 將 .txt 檔案重新命名為 .bak

for file in *.txt; do
    if [ -f "$file" ]; then
        mv "$file" "${file%.txt}.bak"
        echo "Renamed: $file -> ${file%.txt}.bak"
    fi
done
```

### 系統監控

```bash
#!/bin/bash
# 監控 CPU 和記憶體使用

echo "=== System Monitor ==="
echo "CPU Load: $(uptime)"
echo "Memory Usage:"
free -h
echo ""
echo "Top 5 CPU Processes:"
ps aux --sort=-%cpu | head -6
```

### 備份腳本

```bash
#!/bin/bash
# 簡單的備份腳本

BACKUP_DIR="/backup"
SOURCE_DIR="/home/user/documents"
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

# 建立備份目錄
mkdir -p "$BACKUP_DIR"

# 壓縮並備份
tar -czf "$BACKUP_DIR/backup_$TIMESTAMP.tar.gz" "$SOURCE_DIR"

# 刪除 7 天前的備份
find "$BACKUP_DIR" -name "backup_*.tar.gz" -mtime +7 -delete

echo "Backup completed: backup_$TIMESTAMP.tar.gz"
```

## 最佳實踐

```bash
#!/bin/bash
# 最佳實踐範例

set -euo pipefail  # 嚴格模式
IFS=$'\n\t'        # 安全分割

# 使用常數
readonly LOG_FILE="/var/log/myscript.log"

# 錯誤處理函數
error_exit() {
    echo "Error: $1" >&2
    exit 1
}

# 檢查依賴
command -v rsync >/dev/null 2>&1 || error_exit "rsync not found"

# 清理函數
cleanup() {
    rm -rf "$tmp_dir"
}
trap cleanup EXIT
```

## 相關主題

- [Unix](Unix.md) - Shell 的作業系統環境
- [Linux系統呼叫](Linux系統呼叫.md) - 系統程式介面
