# LISP (LISt Processor)

## 概述

LISP 是最古老的high-level程式語言之一，由 John McCarthy 於 1958 年在 MIT 發明。LISP 名稱源自「LISt Processor」，反映了其核心資料結構——列表（List）。作為首個函數式程式語言，LISP 奠定了人工智慧研究的基礎，影響了無數後續語言，包括 Scheme、Common Lisp、Clojure 等。

## 歷史背景

LISP 的誕生與人工智慧研究密切相關。1956 年的達特茅斯會議確定了「人工智慧」這一術語，1958 年 McCarthy 在 MIT 設計了 LISP，試圖為符號處理和人工智慧提供合適的程式語言工具。LISP 的許多創新概念領先時代數十年，包括垃圾回收、樹狀資料結構、動態類型、程式即資料等。

LISP 的發展經歷了多個階段。1958 年的 LISP 1 是第一個完整實現，1962 年的 LISP 1.5 進行了重大改進。此後，MACLISP、Interlisp、Zetalisp 等方言在不同機構發展。1984 年的 Common Lisp 標準試圖統一眾多 LISP 方言，成為 LISP 的主要分支。1975 年的 Scheme 由 Guy Steele 和 Gerald Jay Sussman 在 MIT 設計，強調簡潔性和功能性。2007 年的 Clojure 是 LISP 的現代實現，運行於 JVM 和 CLR 之上，強調並發和函數式編程。

## 核心概念

### S-表達式

LISP 的程式和資料都使用 S-表達式（S-Expression）表示，這是一種前綴表示法：

```lisp
; 數字
42

; 符號
foo

; 列表
(1 2 3)

; 巢狀列表
(1 (2 3) (4 (5 6)))

; 函數調用
(+ 1 2)

; 函數定義
(defun factorial (n)
  (if (= n 0)
      1
      (* n (factorial (- n 1)))))
```

### 程式即資料

LISP 的一個核心原則是「程式即資料」。程式碼本身可以作為資料進行操作，這使得元程式設計（Metaprogramming）變得自然：

```lisp
; 程式碼即列表
'(+ 1 2)  ; => (+ 1 2)

; 評估程式碼
(eval '(+ 1 2))  ; => 3

; 動態生成程式
(defun make-adder (n)
  (lambda (x) (+ x n)))

; 使用
(funcall (make-adder 5) 3)  ; => 8
```

### 函數式編程

LISP 是原生支援函數式編程的語言：

```lisp
; 高階函數
(mapcar #'sqrt '(1 4 9 16))  ; => (1 2 3 4)

(filter #'(lambda (x) (> x 5)) '(1 3 6 8))  ; => (6 8)

(reduce #'+ '(1 2 3 4 5))  ; => 15

; 匿名函數
(lambda (x y) (+ x y))

; 閉包
(let ((counter 0))
  (defun increment ()
    (setf counter (1+ counter))))
```

## Common Lisp

Common Lisp 是 LISP 的主要標準，提供了完整的開發環境：

### 基本語法

```lisp
; 變數
(defvar *name* "Alice")  ; 全域變數
(let ((x 10)
      (y 20))
  (+ x y))               ; 區域綁定

; 資料類型
42         ; 整數
3.14       ; 浮點數
"hello"    ; 字串
#\A        ; 字元
'(1 2 3)   ; 列表
#(1 2 3)   ; 向量
#h{a 1 b 2}; 雜湊表

; 結構
(defstruct person
  name age city)

(make-person :name "Bob" :age 30 :city "Taipei")
```

### 控制流

```lisp
; 條件
(if (> 3 2)
    "greater"
    "lesser")

(cond
  ((= x 0) "zero")
  ((> x 0) "positive")
  (t "negative"))

(case day
  ((mon tue wed thu fri) "weekday")
  ((sat sun) "weekend"))

; 迴圈
(loop for i from 1 to 10
      collect (* i i))

(dotimes (i 5)
  (print i))

(dolist (item '(a b c))
  (print item))
```

### 類別與物件

```lisp
; 定義類別
(defclass animal ()
  ((name :initarg :name :accessor animal-name)
   (age :initarg :age :accessor animal-age)))

; 定義方法
(defmethod speak ((a animal))
  (format t "~A makes a sound~%" (animal-name a)))

; 繼承
(defclass dog (animal)
  ((breed :initarg :breed :accessor dog-breed)))

(defmethod speak ((d dog))
  (format t "~A barks~%" (animal-name d)))
```

## Scheme

Scheme 是 LISP 的一個簡潔方言，強調功能性：

```scheme
; 基本語法
(define x 10)
(define (factorial n)
  (if (= n 0)
      1
      (* n (factorial (- n 1)))))

; 尾遞迴優化
(define (factorial n)
  (define (iter result count)
    (if (= count 0)
        result
        (iter (* result count) (- count 1))))
  (iter 1 n))

; 延遲求值
(define (make-delay thunk)
  (lambda () (thunk)))

(define ones (cons 1 (delay ones)))
```

## Clojure

Clojure 是運行於 JVM 的現代 LISP：

```clojure
; 基本語法
(def x 10)
(defn factorial [n]
  (if (zero? n)
    1
    (* n (factorial (dec n)))))

; 資料結構
[1 2 3]       ; 向量
{:a 1 :b 2}   ; 映射
#{1 2 3}      ; 集合
'(1 2 3)      ; 列表

; 函數式操作
(map inc [1 2 3])      ; => (2 3 4)
(filter even? [1 2 3]) ; => (2)
(reduce + [1 2 3 4])   ; => 10

; 並發
(def counter (atom 0))
(swap! counter inc)

; 代價 threads
(future (do-something))
```

## LISP 的遺產

### 對人工智慧的影響

LISP 長期是人工智慧研究的首選語言。專家系統、邏輯編程、自然語言處理等領域都大量使用 LISP。1970 年代的 Lisp Machine 是專門為 LISP 設計的商用電腦。

### 對語言設計的影響

LISP 影響了幾乎所有現代編程語言。Python、Ruby、JavaScript 的列表推導、lambda 表達式都源自 LISP。Haskell 的函數式特性也深受 LISP 影響。Macros、REPL、垃圾回收等概念都由 LISP 首創。

### 經典實現

```lisp
;經典 Lisp 程序：Evaulator
(defun eval-exp (exp env)
  (cond
    ((numberp exp) exp)
    ((symbolp exp) (lookup exp env))
    ((eq (car exp) 'quote) (cadr exp))
    ((eq (car exp) 'if)
     (if (eval-exp (cadr exp) env)
         (eval-exp (caddr exp) env)
         (eval-exp (cadddr exp) env)))
    ((eq (car exp) 'lambda)
     (list 'closure (cdr exp) env))
    (t
     (apply-func (eval-exp (car exp) env)
                 (mapcar (lambda (e) (eval-exp e env))
                         (cdr exp))))))
```

## 現代應用

### 開發工具

Emacs 使用 Elisp 作為擴展語言，是最强大的文字編輯器。ACL2 是形式化驗證工具，用於數學定理證明。Maxima 是電腦代數系統，廣泛用於數學計算。

### 人工智慧

LISP 仍是符號人工智慧的常用工具。深度學習框架的原型驗證常使用 LISP。知識表示和推理系統常用 LISP 實現。

### 教育

LISP 的簡單語法和強大功能使其適合作為程式設計入門語言。SICP（計算機程式的結構和解釋）使用 Scheme 教學。

## 相關概念

- [函數式編程](函數式編程.md) - 函數式編程範式
- [Lambda演算](Lambda演算.md) - LISP 的理論基礎

## Tags

#LISP #Scheme #CommonLisp #Clojure #函數式編程 #人工智慧