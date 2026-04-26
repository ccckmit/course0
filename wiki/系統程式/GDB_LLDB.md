# GDB 與 LLDB

GDB（GNU Debugger）和 LLDB 是兩個主要的命令列調試工具，用於除錯 C、C++ 等語言的程式。它們幫助開發者找出程式中的錯誤，觀察變數值，逐步執行代碼。

## GDB

GDB 是 GNU 計畫的一部分，由 Richard Stallman 於 1986 年開始開發，是 Linux 系統上最廣泛使用的除錯工具。

### 基本概念

要使用 GDB 調試，必須在編譯時加入除錯資訊：

```bash
# 編譯時加入 -g 選項
gcc -g -O0 -o program program.c

# -O0 關閉最佳化，避免除錯時變數值被最佳化掉
```

### 啟動 GDB

```bash
# 基本啟動
gdb ./program

# 帶參數
gdb --args ./program arg1 arg2

# 附加到行程
gdb -p 12345

# 核心轉儲檔案
gdb ./program core.dump
```

### 常用命令

```gdb
# 執行控制
run                  # 開始執行（簡寫 r）
continue             # 繼續執行（簡寫 c）
next                 # 執行下一行，不進入函式（簡寫 n）
step                 # 執行下一行，進入函式（簡寫 s）
finish              # 執行到目前函式返回
until               # 執行到指定位置
break               # 設定中斷點（簡寫 b）
delete breakpoints  # 刪除所有中斷點（簡寫 d）

# 檢視
print var           # 印出變數值（簡寫 p）
print/x var         # 以十六進制印出
print *ptr          # 印出指標指向的值
print array[0]@10   # 印出從 array[0] 開始的 10 個元素
display var         # 每次中斷時自動印出變數
undisplay           # 取消自動顯示
info locals         # 印出所有區域變數
info args           # 印出函式參數
info breakpoints    # 印出所有中斷點
info threads        # 印出所有執行緒
info registers      # 印出暫存器值

# 記憶體
x/nfu address       # 檢視記憶體
x/10x buffer        # 以十六進制印出 buffer 前 10 個 word
x/s string_ptr      # 以字串印出
x/20c buffer        # 以字元印出

# 堆疊
backtrace           # 印出呼叫堆疊（簡寫 bt）
frame n             # 切換到第 n 個堆疊框架（簡寫 f）
up                  # 向上移動一個堆疊框架
down                # 向下移動一個堆疊框架
info frame          # 印出目前堆疊框架資訊

# 執行緒
thread n            # 切換到第 n 個執行緒
thread apply all bt # 印出所有執行緒的堆疊
break location thread n  # 只在特定執行緒中斷

# 其他
watch var           # 設定 watch point（變數改變時中斷）
awatch var          # 讀取或寫入時中斷
rwatch var          # 讀取時中斷
list                # 印出原始碼（簡寫 l）
list function       # 印出函式原始碼
 disassemble        # 反組譯
```

### 條件中斷點

```gdb
# 條件中斷
break foo.c:42 if x > 10

# 中斷後執行命令
break foo if counter == 100
commands
silent
printf "counter reached %d\n", counter
continue
end

# 忽略中斷點
ignore 1 10  # 忽略 1 號中斷點 10 次
```

### 指標和結構

```gdb
# 指針操作
p *ptr           # 印出指標指向的結構
p ptr->member    # 印出結構成員
p ptr[5]         # 印出指標算術結果

# 結構和類別
p sizeof(struct point)  # 印出結構大小
ptype Point              # 印出類型定義

# C++ 類別
p this                # 印出 this 指標
p *this               # 印出 this 指向的物件
p vptr                # 印出虛擬表指標

# 完整結構印出
set print pretty on
p myStruct
```

### Python 腳本

```gdb
# 可以在 GDB 中執行 Python
python
print("Hello from Python!")
end

# 定義 Python 命令
python
class MyCommand(gdb.Command):
    def __init__(self):
        super().__init__("mycmd", gdb.COMMAND_USER)
    
    def invoke(self, arg, from_tty):
        print("Custom command!")

MyCommand()
end
```

### GDB 巨集

```gdb
# 定義巨集
define printArray
    set $i = 0
    while $i < $arg0
        printf "arr[%d] = %d\n", $i, arr[$i]
        set $i++
    end
end

# 使用
printArray 10
```

### TUI 模式

```gdb
# 進入 TUI 模式
gdb -tui ./program

# TUI 快捷鍵
Ctrl+X Ctrl+A   # 進入/退出 TUI
Ctrl+X Ctrl+2   # 分屏顯示
Ctrl+L         # 刷新螢幕
```

## LLDB

LLDB 是 LLVM 專案的一部分，由 Apple 開發，是 macOS 上的預設除錯器，也可用於 Linux。

### 基本命令對照

| GDB | LLDB |
|-----|------|
| run | run / r |
| continue | continue / c |
| next | next / n |
| step | step / s |
| finish | finish |
| break | breakpoint set |
| print | print / p |
| backtrace | bt / thread backtrace |
| frame | frame select / fr |
| watchpoint | watchpoint set |
| delete | breakpoint delete |

### LLDB 特有命令

```lldb
# 影像庫
image list              # 列出所有載入的模組
image lookup -a address  # 根據位址查符號
image dump symtab        # 轉儲符號表

# 反組譯
disassemble             # 反組譯目前函式
disassemble -n main     # 反組譯 main 函式
disassemble -A false    # 不顯示位址

# 格式化輸出
frame variable           # 印出所有區域變數
frame variable --no-args  # 不含參數
type lookup Point        # 查詢類型定義

# 正規表達式搜尋
breakpoint set --name-regex ".*"  # 所有符合的函式設定中斷點

# 命令別名
command alias gi breakpoint set -s %1 -n %2
```

### 斷點操作

```lldb
# 設定中斷點
breakpoint set --file foo.c --line 42
breakpoint set --name main
breakpoint set --selector draw:

# 條件中斷
breakpoint set --name foo --condition "count > 10"

# 中斷後的命令腳本
breakpoint command add 1
    frame variable
    continue
DONE

# 列出中斷點
breakpoint list

# 刪除
breakpoint delete 1
breakpoint delete --all
```

### Watchpoint

```lldb
# 監視變數
watchpoint set variable global_var
watchpoint set expression &global_var

# 監視讀取或寫入
watchpoint set expression --wrall global_var
watchpoint set expression --read global_var

# watchpoint 命令
watchpoint command add 1
    frame variable
    continue
DONE
```

### 執行緒操作

```lldb
# 列出執行緒
thread list

# 選擇執行緒
thread select 2

# 所有執行緒堆疊
thread backtrace all

# 執行緒特定中斷
breakpoint set --name foo --thread-index 3
```

### Python 腳本

```lldb
# 進入 Python 模式
script

# 定義命令
def my_command(debugger, command, result, internal_dict):
    print("Custom command!")
    result.SetTopLevelText("Done")

# 註冊命令
debugger.HandleCommand("command script add -f myscript.my_command mycmd")

# 常用 API
target = debugger.GetSelectedTarget()
process = target.GetProcess()
thread = process.GetSelectedThread()
frame = thread.GetFrameAtIndex(0)
var = frame.FindVariable("myvar")
```

## 調試技巧

### 記憶體錯誤

```gdb
# 記憶體洩漏檢測（使用 Valgrind）
valgrind --leak-check=full ./program

# 緩衝區溢位
# 編譯時加入 sanitizer
gcc -fsanitize=address -g -o program program.c
gdb ./program

# 使用 watchpoint 追蹤損壞
watch -location struct_ptr->member
```

### 並行程式

```gdb
# 找出死鎖
info threads
thread apply all bt
# 檢查各執行緒等待的鎖

# 資料競爭
gcc -fsanitize=thread -g -o program program.c
valgrind --tool=helgrind ./program
```

### 核心轉儲分析

```bash
# 產生核心轉儲
ulimit -c unlimited
./program  # 崩潰時產生 core
gdb ./program core
```

```gdb
# 分析核心轉儲
bt              # 找出崩潰位置
info registers  # 檢查暫存器
x/i $pc        # 檢查崩潰時的指令
```

## 圖形化前端

### GDB Dashboard

Python 編寫的 GDB TUI 介面：

```bash
# 安裝
curl -fsSL https://github.com/cyrus-and/gdb-dashboard/raw/master/.gdbinit -o ~/.gdbinit

# 功能
# - 原始碼視窗
# - 組譯視窗
# - 暫存器視窗
# - 堆疊視窗
```

### VS Code 除錯

```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "(gdb) Launch",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/a.out",
            "args": [],
            "cwd": "${workspaceFolder}",
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing",
                    "text": "-enable-pretty-printing"
                }
            ]
        }
    ]
}
```

## 相關主題

- [C語言](C語言.md) - C 程式調試
- [Make_CMake](Make_CMake.md) - 建構系統
- [Linux系統呼叫](Linux系統呼叫.md) - 系統層除錯
