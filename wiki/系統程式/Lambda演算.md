# Lambda 演算 (Lambda Calculus)

## 概述

Lambda 演算是 Alonzo Church 在 1936 年發明的計算模型，與圖靈機等價，是函數式編程的理論基礎。

## 核心語法

```
項 (Term) ::= 變數 x
            | λ 變數 . 項     (抽象/函數定義)
            | (項 項)          (應用/函數呼叫)
```

**範例:**
```
λx.x           # 恆等函數
λx.λy.x       # 雙參數函數（柯里化）
(λx.x) y      # 應用：y 傳給恆等函數
```

## 三個版本

| 版本 | 特色 |
|------|------|
| 精簡版 (77行) | 核心功能 |
| 完整版 (219行) | Church Encoding + Y Combinator |
| 直譯器 (188行) | 完整 Parser + AST |

## AST 節點

```python
class Node: pass

class Variable(Node):
    def __init__(self, name): self.name = name

class Abstraction(Node):
    def __init__(self, param, body):
        self.param = param
        self.body = body

class Application(Node):
    def __init__(self, lhs, rhs):
        self.lhs = lhs
        self.rhs = rhs
```

## β-歸約 (Beta Reduction)

```python
def eval(node):
    if isinstance(node, Application):
        lhs = eval(node.lhs)
        if isinstance(lhs, Abstraction):
            return eval(substitute(lhs.body, lhs.param, node.rhs))
    if isinstance(node, Abstraction):
        return Abstraction(node.param, eval(node.body))
    return node

def substitute(node, var, replacement):
    if isinstance(node, Variable):
        return replacement if node.name == var else node
    if isinstance(node, Abstraction):
        if node.param == var: return node
        return Abstraction(node.param, substitute(node.body, var, replacement))
    if isinstance(node, Application):
        return Application(
            substitute(node.lhs, var, replacement),
            substitute(node.rhs, var, replacement))
```

## Church 編碼

### Booleans
```python
TRUE  = λx.λy.x
FALSE = λx.λy.y
IF    = λc.λx.λy.c x y
AND   = λp.λq.p q p
OR    = λp.λq.p p q
NOT   = λc.c FALSE TRUE
```

### Church Numerals
```python
ZERO  = λf.λx.x
ONE   = λf.λx.f x
TWO   = λf.λx.f (f x)
THREE = λf.λx.f (f (f x))

SUCC  = λn.λf.λx.f (n f x)
ADD   = λm.λn.λf.λx.m f (n f x)
MULT  = λm.λn.λf.m (n f)
PRED  = λn.λf.λx.n (λg.λh.h (g f)) (λu.x) (λu.u)
```

## Y Combinator

實現遞迴的關鍵：

```python
Y = λf.(λx.f (x x)) (λx.f (x x))

# 應用：階乘
FACTORIAL = Y (λf.λn.IF (IS_ZERO n)
    (λ_.ONE)
    (λ_.MULT n (f (PRED n)))
    NIL)
```

## 與圖靈機的比較

| 特性 | Lambda 演算 | 圖靈機 |
|------|-------------|--------|
| 發明者 | Alonzo Church | Alan Turing |
| 年份 | 1936 | 1936 |
| 核心概念 | 函數應用與代換 | 狀態與轉移 |
| 資料表示 | Church 編碼 | 符號串 |

## Church-Turing 論點

> 任何可計算的函數都可以用 Lambda 演算或圖靈機表示。

## 發展脈絡

```
Lambda 演算 (1936)
      ↓
Lisp (1958) - 第一個函數式語言
      ↓
Scheme, Clojure, Racket
      ↓
ML, Haskell, OCaml
      ↓
現代語言的函數式特性 (Python, JS, Rust)
```

## 相關資源

- 相關概念：[圖靈機](圖靈機.md)
- 相關概念：[形式文法](形式文法.md)

## Tags

#Lambda演算 #函數式 #Church編碼 #Y_Combinator #計算理論
