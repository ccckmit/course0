# Prolog (Programming in Logic)

## 概述

Prolog 是最早也是最重要的邏輯程式語言之一，由法國馬賽大學的 Alain Colmerauer 和愛丁堡大學的 Robert Kowalski 於 1972 年共同發明。Prolog 名稱源自「Programming in Logic」，其核心思想是使用一階邏輯來表示知識和進行推理。與傳統的命令式語言不同，Prolog 程式描述的是事實和規則，而非具體的操作步驟，由解釋器自動完成邏輯推理。

Prolog 在人工智慧領域有著廣泛的應用，特別適合處理符號推理、專家系統、自然語言處理、知識表示、定理證明等任務。它的宣告式特性使得程式師只需描述問題的邏輯結構，而不需要考慮求解的具體過程，這種「告訴電腦做什麼，而非如何做」的編程範式對後來的聲明式語言產生了深遠影響。

## 歷史背景

Prolog 的發展與計算邏輯的進步密切相關。1970 年代初，Kowalski 提出了原子性定理證明的改進演算法，而 Colmerauer 正在開發自然語言處理系統。兩人的合作催生了第一個 Prolog 解釋器。1977 年，David Warren 在愛丁堡大學實現了第一個高效 的 Prolog 編譯器，稱為 Warren Abstract Machine（WAM），這是 Prolog 真正實用化的關鍵里程碑。

1986 年，ISO 標準化組織發布了 Prolog 標準（ISO/IEC 13211），確保了不同 Prolog 實現之間的互操作性。此後，Prolog 出現了多個重要實現，包括 SWI-Prolog（阿姆斯特丹大學開發，開源且功能豐富）、GNU Prolog（GNU 項目）、Sicstus Prolog（商業版本）等。Prolog 還催生了眾多相關語言和系統，如 Constraint Logic Programming（CLP）、Answer Set Programming（ASP）等。

## 核心概念

### 事實與規則

Prolog 程式的核心由事實（Facts）和規則（Rules）組成：

```prolog
% 事實：描述客觀事實
father(tom, alice).     % Tom 是 Alice 的父親
mother(sue, alice).     % Sue 是 Alice 的母親
male(tom).              % Tom 是男性
female(sue).            % Sue 是女性

% 規則：描述邏輯關係
parent(X, Y) :- father(X, Y).
parent(X, Y) :- mother(X, Y).

grandparent(X, Z) :- parent(X, Y), parent(Y, Z).

sibling(X, Y) :- parent(P, X), parent(P, Y), X \= Y.
```

### 查詢與推理

Prolog 的查詢過程就是邏輯推理的過程：

```prolog
% 查詢：誰是 Alice 的父母？
?- parent(X, alice).
X = tom ;
X = sue.

% 查詢：Tom 是 Alice 的祖父嗎？
?- grandparent(tom, alice).
true.

% 查詢：找出所有兄弟姐妹
?- sibling(X, Y).
X = alice, Y = bob ;
X = bob, Y = alice.
```

### 遞迴與列表

列表是 Prolog 中最重要的複合資料結構：

```prolog
% 列表表示
[1, 2, 3, 4, 5]
[X | Y]      % 頭部 X，尾部 Y
[]           % 空列表

% 會員關係
member(X, [X | _]).
member(X, [_ | T]) :- member(X, T).

% 附加
append([], L, L).
append([H | T], L, [H | R]) :- append(T, L, R).

% 反轉
reverse([], []).
reverse([H | T], R) :- reverse(T, TR), append(TR, [H], R).

% 排序（快速排序）
quicksort([], []).
quicksort([H | T], S) :-
    partition(T, H, Small, Big),
    quicksort(Small, SS),
    quicksort(Big, SB),
    append(SS, [H | SB], S).

partition([], _, [], []).
partition([X | T], H, [X | S], B) :- X =< H, !, partition(T, H, S, B).
partition([X | T], H, S, [X | B]) :- partition(T, H, S, B).
```

## 語法與資料類型

### 基本資料類型

```prolog
% 原子（Atom）：小寫開頭的符號
atom
alice
'Hello World'

% 數字：整數和浮點數
42
3.14159
-17

% 變數：大寫開頭或下劃線
X
Y
_anonymous    % 匿名變數

% 複合項
person(tom, 30)
point(X, Y)
```

### 項的結構

```prolog
% 簡單事實
likes(john, mary).

% 帶參數的事實
edge(a, b, 5).       % 圖的邊
book(title, author, year).

% 嵌套結構
sentence(noun_phrase(det(the), noun(cat)),
         verb_phrase(verb(runs))).
```

### 算術運算

```prolog
% 算術表達式需要 is/2 計算
?- X is 2 + 3 * 4.
X = 14.

% 比較運算
?- 5 > 3, 5 =< 10.
true.

% 數學庫
?- X is sin(0.5).
X = 0.4794255386.

?- X is sqrt(16).
X = 4.0.
```

## 控制結構

### 失敗與回溯

```proloop
% 失敗表示式：fail
cut_if_first(X) :- member(X, [a,b,c]), !, X = a.
cut_if_first(_) :- fail.

% all 收集所有解
?- findall(X, parent(X, alice), Parents).
Parents = [tom, sue].

% bagof 和 setof
?- setof(X, parent(X, _), AllParents).
AllParents = [sue, tom].
```

### 迴圈的模擬

```prolog
% 重複執行（使用失敗回溯）
repeat.
repeat :- repeat.

% 計數器迴圈
between(From, To, Val) :- 
    from_to(From, To, Val).

from_to(From, To, From) :- From =< To.
from_to(From, To, Val) :- 
    From < To, 
    Next is From + 1, 
    from_to(Next, To, Val).
```

### 切斷與控制

```prolog
% 綠色切斷（不影響程式語義）
max(X, Y, X) :- X >= Y, !.
max(_, Y, Y).

% 紅色切斷（改變程式語義）
first_member(X, [X | _]) :- !.
first_member(_, [_ | T]) :- first_member(X, T).

% 失敗而非回溯
once(Goal) :- Goal, !.
```

## 資料結構

### 差分列表

```prolog
% 差分列表實現隊列
queue(E, [E|T], T).

enqueue(E, Q1, Q2) :- queue(E, Q1, Q2).
dequeue(E, Q1, Q2) :- queue(E, Q2, Q1).

% 追加（常數時間）
diff_append(A1-Z, Z-B, A1-B).
```

### 二元樹

```prolog
% 二元搜尋樹
tree(empty).
tree(node(Key, Value, Left, Right)).

% 插入
insert(Key, Value, empty, node(Key, Value, empty, empty)).
insert(Key, Value, node(K, V, L, R), node(K, V, NL, R)) :- 
    Key < K, !, insert(Key, Value, L, NL).
insert(Key, Value, node(K, V, L, R), node(K, V, L, NR)) :- 
    insert(Key, Value, R, NR).

% 查找
search(Key, node(K, V, _, _), V) :- K =:= Key, !.
search(Key, node(K, _, L, _), V) :- Key < K, !, search(Key, L, V).
search(Key, node(K, _, _, R), V) :- search(Key, R, V).
```

### 圖與路徑

```prolog
% 深度優先搜索
dfs(Start, Goal, Path) :-
    dfs_rec(Start, Goal, [Start], Path).

dfs_rec(Goal, Goal, Path, Path).
dfs_rec(Current, Goal, Visited, Path) :-
    edge(Current, Next),
    \+ member(Next, Visited),
    dfs_rec(Next, Goal, [Next | Visited], Path).

% 廣度優先搜索
bfs(Start, Goal, Path) :-
    bfs_queue([Start], [], Goal, PathRev),
    reverse(PathRev, Path).

bfs_queue([], _, _, []) :- !, fail.
bfs_queue([Goal | _], _, Goal, [Goal]).
bfs_queue([Current | Rest], Visited, Goal, Path) :-
    findall(Next, 
            (edge(Current, Next), \+ member(Next, Visited)), 
            Neighbors),
    append(Rest, Neighbors, NewQueue),
    union(Neighbors, Visited, NewVisited),
    bfs_queue(NewQueue, NewVisited, Goal, Path).
```

## 實際應用

### 專家系統

```prolog
% 醫療診斷專家系統
symptom(john, fever).
symptom(john, cough).
symptom(john, headache).

disease(flu) :- 
    symptom(Person, fever),
    symptom(Person, cough),
    symptom(Person, headache).

disease(cold) :- 
    symptom(Person, cough),
    symptom(Person, runny_nose).

treatment(Person, flu) :- 
    disease(flu), !,
    advise(Person, rest),
    advise(Person, fluids),
    advise(Person, medication).

treatment(Person, cold) :- 
    advise(Person, rest),
    advise(Person, vitamin_c).
```

### 自然語言處理

```prolog
% 簡單的語法規則
s --> np, vp.
np --> det, n.
vp --> v, np.
vp --> v.

det --> [the].
det --> [a].
n --> [cat].
n --> [dog].
v --> [chases].
v --> [eats].

% 查詢
?- s([the, cat, chases, the, dog], []).
true.

% 語意表示
semantics(s(NP, VP), sem(NP_sem, VP_sem)) :-
    semantics(NP, NP_sem),
    semantics(VP, VP_sem).

semantics(np(det(the), n(cat)), cat).
semantics(vp(v(chases), np(det(the), n(dog))), chase(cat, dog)).
```

### 約束邏輯編程

```prolog
% 引入 CLP(FD) 庫
:- use_module(library(clpfd)).

% 數獨求解
sudoku(Rows) :-
    length(Rows, 9),
    maplist(length(9), Rows),
    append(Rows, Vs),
    Vs ins 1..9,
    maplist(all_distinct, Rows),
    transpose(Rows, Cols),
    maplist(all_distinct, Cols),
    Blocks = [A,B,C,D,E,F,G,H,I],
    maplist(sudoku_block, Rows, Blocks),
    maplist(label, Rows).

sudoku_block([], []).
sudoku_block([R1,R2,R3|Rest], [B|Bs]) :-
    all_distinct([R1,R2,R3]),
    sudoku_block(Rest, Bs).
```

## 與其他語言的比較

### 與 LISP 比較

LISP 和 Prolog都是 AI 常用的語言，但設計理念不同。LISP 是函數式語言，強調函數和遞迴；Prolog 是邏輯語言，強調關係和模式匹配。LISP 程式是主動的，描述如何執行；Prolog 程式是被動的，描述問題的邏輯結構，由解釋器決定執行策略。

### 與命令式語言比較

Prolog 的宣告式特性使其更適合表達「是什麼」而非「如何做」。對於某些問題（如約束求解、邏輯推理），Prolog 可以用很少的代碼實現強大的功能。但對於傳統的演算法問題，命令式語言通常更直觀和高效。

## 現代應用

### 知識表示與推理

Prolog 適合表示知識庫和進行推理。它是語意網和本體語言（如 RDF、OWL）的理論基礎。Protégé 本體編輯器支持 Prolog 風格的推理規則。

### 計算語言學

Prolog 的模式匹配和規則系統非常適合處理自然語言。DCG（Definite Clause Grammar）是一種強大的語法表示方法，廣泛用於 NLP 領域。

### 問題求解

規劃系統、調度問題、配置問題等都可以用 Prolog 優雅地表示。 Answer Set Programming（ASP）是由 Prolog 發展而來的聲明式問題求解範式。

## 相關概念

- [人工智慧](人工智慧.md) - AI 基礎
- [LISP](LISP.md) - 另一個 AI 常用語言

## Tags

#Prolog #邏輯編程 #AI #知識表示 #專家系統