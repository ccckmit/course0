# Zig 語言

Zig 是一門新興的系統程式語言，由 Andrew Kelley 於 2015 年創建，設計目標是成為 C 語言的現代替代者。Zig 強調簡潔、顯式、安全，同時保持零成本抽象和裸機控制能力。

## Zig 的設計理念

### 顯式優於隱式

Zig 拒絕 C++ 和 Rust 中許多隱式機制，要求程式設計師明確表達意圖：

```zig
// Zig 明確處理錯誤
const result = try someFunction();

// C++ 隱式 throw
auto result = someFunction();  // 可能拋出異常，無聲無息

// Rust 使用 ? 運算子
let result = someFunction()?;  // 明確傳播錯誤
```

### 無隱式控制流

Zig 沒有：
- 構造函數/解構函數（RAII）
- 運算子重載
- 繼承
- 巨集（但有強大的 comptime）

這使得 Zig 程式更易預測和推理。

### Comptime：編譯時計算

Zig 的 comptime 是革命性的特性，允許在編譯時執行任意 Zig 程式碼：

```zig
// 編譯時計算
const fibonacci = comptime {
    var fib: [10]u32 = undefined;
    fib[0] = 0;
    fib[1] = 1;
    for (fib[2..]) |*n| {
        n.* = fib[i-1] + fib[i-2];
    }
    break :blk fib;
};

// 編譯時類型計算
fn Matrix(comptime rows: usize, comptime cols: usize, comptime T: type) type {
    return [rows][cols]T;
}

const mat: Matrix(3, 4, f32) = undefined;
```

Comptime 可用於：
- 生成複雜的資料結構
- 編譯時 assert
- 模板元程式設計（替代 C++ templates）

## Zig 語法基礎

### 變數與型別

```zig
const x: i32 = 42;           // 常量，編譯時確定
var y: u64 = 100;            // 變量，可修改

// 推斷型別
const inferred = @as(i32, 42);

// 基本型別
const a: u8 = 255;           // 無符號 8 位元
const b: i32 = -1;           // 有符號 32 位元
const c: f64 = 3.14;         // 64 位元浮點數
const d: bool = true;
const e: void = {};
const f: noreturn = unreachable;
```

### 流程控制

```zig
// if 表達式
const max = if (a > b) a else b;

// while
var i: usize = 0;
while (i < 10) : (i += 1) {
    if (i == 5) continue;
    if (i == 8) break;
    sum += i;
}

// for 迴圈（迭代陣列）
for (items) |item, index| {
    std.debug.print("{}: {}\n", .{ index, item });
}

// switch 表達式
const result = switch (value) {
    1 => "one",
    2 => "two",
    else => "other",
};
```

### 函數

```zig
fn add(a: i32, b: i32) i32 {
    return a + b;
}

// 匿名函數
const addFn = struct {
    fn call(a: i32, b: i32) i32 {
        return a + b;
    }
}.call;

// 多返回
fn divide(a: i32, b: i32) struct { quotient: i32, remainder: i32 } {
    return .{ .quotient = a / b, .remainder = a % b };
}
```

## 錯誤處理

Zig 採用類似 Go 的錯誤處理策略，但更顯式：

### Error Union Type

```zig
const FileError = error {
    NotFound,
    PermissionDenied,
    OutOfMemory,
};

const MyError = error{Overflow};

const ErrorOrInt = FileError || MyError || i32;
```

### 錯誤傳播

```zig
const std = @import("std");

fn openFile(path: []const u8) !std.fs.File {
    const file = try std.fs.cwd().openFile(path, .{});
    return file;
}

// 使用 try（類似 ? 運算子）
fn readConfig() ![]u8 {
    const file = try openFile("config.txt");
    defer file.close();  // defer 確保離開時執行
    return try file.readToEndAlloc(std.heap.page_allocator, 1024);
}

// 自定義錯誤處理
fn safeRead() void {
    if (openFile("config.txt")) |file| {
        defer file.close();
        // 成功處理
    } else |err| {
        // 錯誤處理
        std.debug.print("Error: {}\n", .{err});
    }
}
```

## 記憶體管理

Zig 沒有垃圾回收，記憶體管理完全由程式設計師控制：

### defer 語句

```zig
fn process() void {
    const file = try openFile("data.txt");
    defer file.close();  // 無論如何都會關閉

    const allocator = std.heap.page_allocator;
    const data = try allocator.alloc(u8, 1024);
    defer allocator.free(data);  // 無論如何都會釋放

    // 主要邏輯
}
```

### 記憶體分配器

Zig 允許自定義記憶體分配器：

```zig
// 標準分配器
var arena = std.heap.ArenaAllocator.init(std.heap.page_allocator);
defer arena.deinit();

// 固定緩衝區分配器
var buffer: [1024]u8 = undefined;
var fba = std.heap.FixedBufferAllocator.init(&buffer);

// 單調分配器（嵌入式）
var mem: [4096]u8 = undefined;
var mono = std.heap.MemoryPool(?[]u8).init(&mem);
```

### Error Return Traces

Zig 內建錯誤堆疊追蹤，無需例外處理框架：

```
error: FileNotFound
/home/user/project/src/main.zig:5:5: 0x2295a7 in openFile (main)
    try std.fs.cwd().openFile(path, .{});
        ^
/home/user/project/src/main.zig:12:5: 0x2296e0 in main (main)
    const data = try readConfig();
        ^
```

## 泛型

Zig 使用 struct 和類型作為泛型的基礎：

```zig
fn Stack(comptime T: type) type {
    return struct {
        items: []T,
        len: usize,
        
        const Self = @This();
        
        pub fn push(self: *Self, item: T) void {
            // ...
        }
        
        pub fn pop(self: *Self) T {
            // ...
        }
    };
}

// 使用
var intStack = Stack(i32){ .items = &.{}, .len = 0 };
var strStack = Stack([]const u8){ .items = &.{}, .len = 0 };
```

## C 語言交互

Zig 可以無縫調用 C 程式碼，也能被 C 調用：

### 調用 C 函數

```zig
const c = @cImport(@cInclude("stdio.h"));

pub fn main() void {
    _ = c.printf("Hello from C!\n");
}
```

### 導出 Zig 函數供 C 使用

```zig
export fn add(a: c_int, b: c_int) c_int {
    return a + b;
}
```

### build.zig 構建系統

Zig 自帶構建系統，無需 Makefile 或 CMake：

```zig
const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const mode = b.standardReleaseOptions();
    
    const exe = b.addExecutable("hello", "src/main.zig");
    exe.setBuildMode(mode);
    exe.install();
    
    const run = b.addBuildArtifact(exe);
    b.default_step.dependOn(&run.step);
}
```

## 實際應用場景

### 編譯器工具鏈

Zig 的交叉編譯能力是其亮點：

```bash
# 編譯為 Windows x86-64
zig build-exe -target x86_64-windows-gnu src/main.zig

# 編譯為 ARM64 macOS
zig build-exe -target aarch64-macos-none src/main.zig
```

### 嵌入式開發

Zig 的無 Runtime 和精確控制使其適合嵌入式：

```zig
const embedded = @import("embedded");
const cortex_m = @import("cortex_m");

export fn reset() void {
    cortex_m.init();
    main();
}
```

### 替代 C/C++ 專案

Zig 可直接替換現有 C 程式碼：
- 更安全的記憶體操作
- 更好的錯誤處理
- 更現代的語法

## Zig 與 Rust 的比較

| 特性 | Zig | Rust |
|------|-----|------|
| 記憶體安全 | 手動 + 工具 | 靜態借用檢查 |
| 錯誤處理 | Error union + try | Result + ? |
| 泛型 | 編譯時執行 | traits + generics |
| Runtime | 無 | 無（mostly） |
| 學習曲線 | 平緩 | 陡峭 |
| 學習曲線 | 低 | 高 |
| 生態系統 | 新興 | 成熟 |
| 野心 | C 替代 | 系統/應用 |

## Zig 的現狀與未來

### 當前版本（Zig 0.12+）

- 標準庫重構持續進行
- 編譯器前端基於 LLVM
- 逐漸成為 C 語言的有力替代

### 預計特性

- **Async/await**：原生非同步支援
- **GC allocator**：可選的垃圾回收
- **更好的標準庫**：持續改善

### 採用情況

Zig 已被用於：
- Bun（JavaScript 執行環境）
- TigerBeetle（分散式資料庫）
- Oxigraph（Rust 圖資料庫核心）
- 各種嵌入式專案

## 相關主題

- [C語言](C語言.md) - Zig 的主要替代目標
- [Rust](Rust.md) - 另一種記憶體安全語言
- [編譯器](編譯器.md) - 語言翻譯技術
- [Go](Go.md) - 系統程式語言
