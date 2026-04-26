# C++

C++ 是一門高效能的系統程式語言，由 Bjarne Stroustrup 於 1979 年在貝爾實驗室開始開發，最初命名為「C with Classes」。1983 年正式更名為 C++，並持續演進至今。C++ 結合了 C 語言的效率和底層控制能力，同時引入物件導向、泛型程式設計等高階特性，成為遊戲開發、嵌入式系統、高效能運算、金融交易等領域的首選語言。

## C++ 的發展歷程

### C++ 的誕生

Bjarne Stroustrup 在博士學位研究期間，發現 Simula 的物件導向特性對大型軟體開發極有價值，但執行效率太低。同時，他需要使用 C 語言開發分散式系統，卻苦於缺乏合適的工具。

1979 年，Stroustrup 開始開發「C with Classes」，在 C 語言基礎上添加類別、衍生類別、公有/私有存取控制、 建構函式和解構函式等特性。

1983 年，程式語言正式命名為 C++（++ 來自 C 的遞增運算子）。此後陸續加入虛擬函式、運算子重載、參照、範本、例外處理、命名空間等特性。

### 標準化歷程

- **C++98**：第一個 ISO 標準，建立語法基礎
- **C++03**：小幅修正，無重大變更
- **C++11**：現代 C++ 革命性更新（Lambda、移动语义、智能指標）
- **C++14**：C++11 的錯誤修正和改進
- **C++17**：檔案系統、std::optional、std::variant
- **C++20**：概念（Concepts）、協程、模組、範圍（Ranges）
- **C++23**：更多標準庫增強

## C++ 核心特性

### 物件導向程式設計

```cpp
class Shape {
protected:
    double x, y;
public:
    Shape(double x, double y) : x(x), y(y) {}
    virtual ~Shape() = default;
    virtual double area() const = 0;  // 純虛擬函式
};

class Rectangle : public Shape {
    double width, height;
public:
    Rectangle(double x, double y, double w, double h)
        : Shape(x, y), width(w), height(h) {}
    
    double area() const override {
        return width * height;
    }
};

class Circle : public Shape {
    double radius;
public:
    Circle(double x, double y, double r)
        : Shape(x, y), radius(r) {}
    
    double area() const override {
        return 3.14159 * radius * radius;
    }
};
```

### 範本與泛型程式設計

```cpp
// 函式範本
template<typename T>
T max(T a, T b) {
    return a > b ? a : b;
}

// 類別範本
template<typename T, size_t N>
class Array {
    T data[N];
public:
    T& operator[](size_t i) { return data[i]; }
    size_t size() const { return N; }
};

// 變異範本
template<typename... Args>
void print_all(Args&&... args) {
    (std::cout << ... << args) << '\n';
}
```

### 智慧指標

```cpp
#include <memory>

// 獨占擁有權
void exclusive_ownership() {
    auto ptr = std::make_unique<int>(42);
    // ptr 離開作用域時自動釋放
}

// 共享擁有權
void shared_ownership() {
    auto shared = std::make_shared<int>(100);
    {
        auto another = shared;  // 參照計數增加
        *another = 200;
    }  // another 離開，計數減少
    // shared 仍有效
}

// 觀察其他資源的擁有者
void weak_reference() {
    auto shared = std::make_shared<int>(42);
    std::weak_ptr<int> weak = shared;
    
    if (auto locked = weak.lock()) {
        // locked 是有效的 shared_ptr
    }
}
```

## 現代 C++ 特性

### Lambda 表達式

```cpp
// 基本語法
auto add = [](int a, int b) { return a + b; };

// 擷取外部變數
int multiplier = 10;
auto scale = [multiplier](int x) { return x * multiplier; };

// 可變 Lambda
int counter = 0;
auto increment = [counter]() mutable {
    counter++;  // 修改擷取的變數
    return counter;
};

// 泛型 Lambda (C++14)
auto generic_max = [](auto a, auto b) {
    return a > b ? a : b;
};

// 即時呼叫
[](int x) { return x * x; }(5);  // 結果 25
```

### 移動語意

```cpp
class Buffer {
    char* data;
    size_t size;
public:
    Buffer(size_t n) : data(new char[n]), size(n) {}
    
    // 移動建構函式
    Buffer(Buffer&& other) noexcept
        : data(other.data), size(other.size) {
        other.data = nullptr;  // 避免解構時刪除
        other.size = 0;
    }
    
    // 移動指派運算子
    Buffer& operator=(Buffer&& other) noexcept {
        if (this != &other) {
            delete[] data;
            data = other.data;
            size = other.size;
            other.data = nullptr;
            other.size = 0;
        }
        return *this;
    }
    
    ~Buffer() { delete[] data; }
};

// 使用移動避免複製
std::vector<Buffer> buffers;
buffers.push_back(Buffer(1024));  // 移動而非複製
```

### auto 與 decltype

```cpp
// 自動型別推斷
auto i = 42;           // int
auto d = 3.14;         // double
auto s = "hello";      // const char*
auto v = std::vector<int>{1, 2, 3};

// decltype：推斷型別而不計算表達式
int x = 5;
decltype(x) y = 10;    // y 是 int

decltype(auto) func() {
    int z = 42;
    return z;           // 返回 int
}

// decltype 在宣告中的應用
std::map<std::string, int> dict;
decltype(dict)::iterator it;  // it 是 map<string, int>::iterator
```

### 範圍 for 迴圈

```cpp
std::vector<int> nums = {1, 2, 3, 4, 5};

// 基本用法
for (int n : nums) {
    std::cout << n << ' ';
}

// 修改元素（需要參照）
for (int& n : nums) {
    n *= 2;
}

// 只讀（const 參照）
for (const int& n : nums) {
    // ...
}

// 使用初始化（C++20）
for (auto it = std::begin(nums); const auto& n : nums) {
    // it 在初始化器中，n 在範圍表達式中
}
```

## C++ 標準庫

### STL 容器

```cpp
#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

// 序列容器
std::vector<int> vec = {1, 2, 3};  // 動態陣列
std::list<int> lst = {1, 2, 3};    // 雙向鏈結串列
std::deque<int> deq = {1, 2, 3};   // 雙端佇列

// 關聯容器（有序）
std::set<int> s = {3, 1, 4, 1, 5};     // 集合
std::map<std::string, int> m;           // 映射
m["one"] = 1;
m["two"] = 2;

// 雜湊容器（無序）
std::unordered_set<int> us;
std::unordered_map<std::string, int> um;
```

### 演算法

```cpp
#include <algorithm>
#include <numeric>

std::vector<int> v = {5, 2, 8, 1, 9};

// 排序
std::sort(v.begin(), v.end());

// 二分搜尋
bool found = std::binary_search(v.begin(), v.end(), 8);

// 查找
auto it = std::find(v.begin(), v.end(), 3);

// 計數
int count = std::count_if(v.begin(), v.end(),
    [](int x) { return x > 5; });

// 轉換
std::vector<int> doubled(v.size());
std::transform(v.begin(), v.end(), doubled.begin(),
    [](int x) { return x * 2; });

// 累加
int sum = std::accumulate(v.begin(), v.end(), 0);
```

### 迭代器

```cpp
std::vector<int> v = {1, 2, 3, 4, 5};

// 正向迭代器
for (auto it = v.begin(); it != v.end(); ++it) {
    std::cout << *it << ' ';
}

// 反向迭代器
for (auto it = v.rbegin(); it != v.rend(); ++it) {
    std::cout << *it << ' ';
}

// 插入迭代器
std::vector<int> dest;
std::copy(v.begin(), v.end(), std::back_inserter(dest));

// 流迭代器
std::copy(v.begin(), v.end(),
    std::ostream_iterator<int>(std::cout, " "));
```

## 記憶體管理

### 運算子 new/delete

```cpp
// 配置單一物件
int* p = new int(42);
delete p;

// 配置陣列
int* arr = new int[100];
delete[] arr;

// 配置並初始化
auto* obj = new (std::nothrow) int[100];
if (!obj) {
    // 配置失敗處理
}

// placement new：在已配置的記憶體中建構物件
char buffer[sizeof(int)];
int* pi = new(buffer) int(42);
pi->~int();  // 明確呼叫解構
```

### 自定義配置器

```cpp
template<typename T>
class PoolAllocator {
    struct Node {
        Node* next;
    };
    Node* free_list = nullptr;
    
public:
    T* allocate(size_t n) {
        if (n != 1) throw std::bad_alloc();
        
        if (!free_list) {
            // 擴展池
            size_t block_size = 4096;
            char* block = static_cast<char*>(::operator new(block_size));
            // 將區塊分割為自由串列
            for (size_t i = 0; i < block_size / sizeof(Node); ++i) {
                reinterpret_cast<Node*>(block + i * sizeof(Node))->next =
                    reinterpret_cast<Node*>(block + (i+1) * sizeof(Node));
            }
            reinterpret_cast<Node*>(block + (block_size - sizeof(Node)))->next = nullptr;
            free_list = reinterpret_cast<Node*>(block);
        }
        
        Node* result = free_list;
        free_list = free_list->next;
        return reinterpret_cast<T*>(result);
    }
    
    void deallocate(T* p, size_t n) {
        if (n != 1) return;
        Node* node = reinterpret_cast<Node*>(p);
        node->next = free_list;
        free_list = node;
    }
};
```

## 執行緒與並行

```cpp
#include <thread>
#include <mutex>
#include <future>

// 建立執行緒
void worker(int id) {
    std::cout << "Worker " << id << '\n';
}

std::thread t1(worker, 1);
std::thread t2(worker, 2);
t1.join();
t2.join();

// 互斥鎖
std::mutex mtx;
int counter = 0;

void safe_increment() {
    std::lock_guard<std::mutex> lock(mtx);
    ++counter;
}

// 原子操作
std::atomic<int> atomic_counter(0);
atomic_counter.fetch_add(1);

// 异步執行
std::future<int> async_task = std::async(std::launch::async, []() {
    return 42;
});
int result = async_task.get();
```

## C++ 最佳實踐

### RAII 慣用法

```cpp
class FileHandle {
    FILE* file;
public:
    FileHandle(const char* name) {
        file = fopen(name, "r");
        if (!file) throw std::runtime_error("Cannot open file");
    }
    
    ~FileHandle() {
        if (file) fclose(file);
    }
    
    // 禁用複製
    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
};

// 使用時自動管理資源
void process() {
    FileHandle fh("data.txt");
    // ... 使用檔案
}  // fh 自動關閉檔案
```

### Rule of Zero/Three/Five

```cpp
// Rule of Zero：使用預設行為或智慧指標
class ResourceOwner {
    std::vector<int> data;  // RAII 資源
public:
    // 使用預設的特殊成員函式即可
};

// Rule of Five：自定義所有五個特殊成員函式
class ManualResource {
    char* buffer;
public:
    ManualResource(size_t n) : buffer(new char[n]) {}
    ~ManualResource() { delete[] buffer; }
    
    ManualResource(const ManualResource&) = delete;
    ManualResource& operator=(const ManualResource&) = delete;
    
    ManualResource(ManualResource&& other) noexcept
        : buffer(other.buffer) {
        other.buffer = nullptr;
    }
    
    ManualResource& operator=(ManualResource&& other) noexcept {
        if (this != &other) {
            delete[] buffer;
            buffer = other.buffer;
            other.buffer = nullptr;
        }
        return *this;
    }
};
```

## 應用場景

- **遊戲開發**：Unreal Engine、Unity 核心
- **系統程式設計**：作業系統、驅動程式
- **嵌入式系統**：資源受限環境
- **高效能運算**：科學計算、金融建模
- **交易系統**：低延遲金融交易
- **圖形學**：遊戲引擎、3D 軟體

## 相關主題

- [C語言](C語言.md) - C++ 的前身
- [Rust](Rust.md) - 記憶體安全的替代語言
- [編譯器](編譯器.md) - 編譯器理論
- [物件導向](物件導向.md) - 物件導向程式設計
