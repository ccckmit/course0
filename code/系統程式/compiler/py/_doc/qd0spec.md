這是一個修改後的規格書。我已將所有四元組格式從 `(op, arg1, arg2, result)` 修改為以空白或逗號分隔的純文字格式，並移除所有括號。

---

## 第一部分：範例驅動說明

透過具體程式碼片段，展示此 IR 的結構。

### 範例 A：基本運算與動態型別

**原始碼（Python 風格）**

```python
x = 1 + "hello"

```

**四元組 IR**

```
LOAD_CONST    1         _      t0    ; t1 = 1
LOAD_CONST    "hello"   _      t1    ; t2 = "hello"
BINOP         t0        t1     t2    ; t3 = t1 + t2 (由 runtime 決定)
STORE         t2        _      x     ; x = t3

```

---

### 範例 B：函式呼叫

**原始碼**

```python
result = foo(a, b + 1)

```

**四元組 IR**

```
LOAD_NAME     a         _      t0    ; 查找 a
LOAD_NAME     b         _      t1    ; 查找 b
LOAD_CONST    1         _      t2
BINOP         t1        t2     t3    ; t3 = b + 1
LOAD_NAME     foo       _      t4    ; 加載函式物件
ARG_PUSH      t0        0      _     ; 推入第 0 個引數
ARG_PUSH      t3        1      _     ; 推入第 1 個引數
CALL          t4        2      t5    ; 呼叫 t4，2 個引數，存至 t5
STORE         t5        _      result

```

---

### 範例 C：條件判斷

**原始碼**

```python
if x > 0:
    y = x
else:
    y = -x

```

**四元組 IR**

```
LOAD_NAME     x         _      t0
LOAD_CONST    0         _      t1
CMP           t0        t1     t2    ; t2 = (t0 > t1)
BRANCH_IF_FALSE  t2     _      L_else
LOAD_NAME     x         _      t3
STORE         t3        _      y
JUMP          L_end     _      _
LABEL         L_else    _      _
LOAD_NAME     x         _      t4
UNARY         NEG       t4     t5    ; t5 = -t4
STORE         t5        _      y
LABEL         L_end     _      _

```

---

## 第二部分：完整指令集規格

### 格式定義

指令運算碼與參數之間以空白分隔：
`op    arg1    arg2    result`

* `_` 表示該欄位不使用。
* `t0, t1 ...` 為暫存變數。
* `L_name` 為跳躍標籤。

---

### 一、值的載入與儲存 (Load/Store)

| 指令 | arg1 | arg2 | result | 語義 |
| --- | --- | --- | --- | --- |
| LOAD_CONST | 常數值 | _ | dest | dest = 常數 |
| LOAD_NAME | 變數名 | _ | dest | 從當前作用域查找變數 |
| LOAD_ATTR | obj | attr | dest | dest = obj.attr |
| STORE | src | _ | var | var = src |
| STORE_ATTR | obj | attr | src | obj.attr = src |
| DELETE_NAME | var | _ | _ | 刪除變數 |

---

### 二、算術與比較運算 (Arithmetic/Compare)

| 指令 | arg1 | arg2 | result | 語義 |
| --- | --- | --- | --- | --- |
| ADD | lhs | rhs | dest | dest = lhs + rhs |
| SUB | lhs | rhs | dest | dest = lhs - rhs |
| MUL | lhs | rhs | dest | dest = lhs * rhs |
| DIV | lhs | rhs | dest | dest = lhs / rhs |
| NEG | operand | _ | dest | dest = -operand |
| CMP_EQ | lhs | rhs | dest | dest = (lhs == rhs) |
| CMP_LT | lhs | rhs | dest | dest = (lhs < rhs) |
| CMP_IN | item | coll | dest | dest = (item in coll) |

---

### 三、控制流與跳躍 (Control Flow)

| 指令 | arg1 | arg2 | result | 語義 |
| --- | --- | --- | --- | --- |
| LABEL | label_n | _ | _ | 定義標籤位置 |
| JUMP | label_n | _ | _ | 無條件跳躍 |
| BRANCH_IF_TRUE | cond | _ | label_n | 若為真則跳躍 |
| BRANCH_IF_FALSE | cond | _ | label_n | 若為假則跳躍 |
| ITER_NEXT | iter | _ | dest | 取下一個迭代值，若耗盡則標記 |
| BRANCH_IF_EXHAUST | iter | _ | label_n | 若迭代器耗盡則跳躍 |

---

### 四、函式與物件 (Function/Object)

| 指令 | arg1 | arg2 | result | 語義 |
| --- | --- | --- | --- | --- |
| ARG_PUSH | value | index | _ | 準備位置引數 |
| CALL | func | argc | dest | 執行呼叫，結果存至 dest |
| RETURN | value | _ | _ | 回傳值並結束函式 |
| BUILD_LIST | count | _ | dest | 建立含 count 個元素的 list |
| GET_ITER | obj | _ | dest | dest = iter(obj) |
| MAKE_CLOSURE | code | cells | dest | 建立閉包 |

---

### 五、例外處理 (Exception)

| 指令 | arg1 | arg2 | result | 語義 |
| --- | --- | --- | --- | --- |
| TRY_BEGIN | handler | _ | _ | 開始監控例外，設定處理標籤 |
| TRY_END | _ | _ | _ | 結束監控例外 |
| RAISE | exc_obj | _ | _ | 拋出例外 |
| MATCH_EXC | type | _ | dest | 檢查當前例外是否匹配 type |

---

### 六、LLVM 橋接指令 (Type Specialization)

| 指令 | arg1 | arg2 | result | 語義 |
| --- | --- | --- | --- | --- |
| ASSUME_TYPE | val | type | _ | 告知編譯器此值必為特定型別 |
| BOX | raw_val | type | dest | 將原生資料包裝成物件 |
| UNBOX | obj | type | dest | 從物件提取原生資料 |

---

需要我針對特定的動態語言（例如 Lua 的 Upvalue 機制）進一步調整這些指令的細節嗎？