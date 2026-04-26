# 黑白棋（五子棋）遊戲 - 基礎 AI 程式說明

## 概述

此檔案實現黑白棋（Gomoku）遊戲，包含基礎的評分函數 AI。電腦會根據啟發式評分選擇最佳位置。

## 理論基礎

### 1. 黑白棋遊戲規則

- 15×15 棋盤
- 雙方輪流下子
- 先連續五子獲勝（橫、直、對角線）
- 電子符號：x 和 o

### 2. 位置編碼

使用十六進制輸入：
- 00-EE（0-14 的十六進制）
- 例如：88 代表第 8 列第 8 行

### 3. 啟發式評分

電腦不使用搜尋，而是根據局部模式評分每個可下的位置：
- 攻擊分：評估自己連子的威脅程度
- 防守分：評估阻止對手連子的必要性

### 4. 貪心策略

每次選擇當前評分最高的位置，不進行樹狀搜尋。

## 數學原理

### 評分函數

對於每個可下位置 (r, c)：

```
score(r,c) = attack_score(r,c) + defense_score(r,c) - distance_penalty
```

其中：
- `attack_score`：己方連子分數
- `defense_score`：對手連子分數 × 0.9（防守權重較低）
- `distance_penalty`：遠離中心的位置輕微扣分

### 連子評分 (check_line)

| 連續棋子數 | 兩端皆空 | 一端阻塞 | 兩端阻塞 |
|------------|----------|----------|----------|
| 4          | 1000     | 1000     | 250      |
| 3          | 100      | 10       | 2.5      |
| 2          | 5        | 3        | 0.75     |
| 1          | 1        | 1        | 0.25     |

### 具體評分邏輯

```python
if consecutive == 4:
    score = 1000      # 必勝
elif consecutive == 3:
    if empty_ends == 2:
        score = 100    # 活三
    else:
        score = 10     # 死三
elif consecutive == 2:
    if empty_ends == 2:
        score = 5      # 活二
    else:
        score = 3     # 死二
elif consecutive == 1:
    score = 1
```

### 阻塞懲罰

```python
if blocked == 2:
    score /= 4  # 兩端都被堵住，大幅降低分數
```

## 運作流程

### 電腦回合 (computer_turn)

```
1. 遍歷所有空位
2. 對每個位置調用 score_position()
3. 找尋最高分的位置
4. 若有多個同分位置，隨機選擇
5. 特殊情況：棋盤為空時下中心
```

### 評分位置 (score_position)

```
1. 檢查四個方向
2. 計算己方攻擊分 (check_line)
3. 計算對手防守分 (check_line) × 0.9
4. 計算中心距離 penalty
5. 返回總分
```

### 檢查線 (check_line)

```
1. 暫時放置棋子
2. 沿兩個方向檢查
3. 統計連續相同棋子數、空端數、阻塞數
4. 恢復原位置
5. 根據表格返回分數
```

## 程式碼架構

```
 Gomoku
 ├── Board
 │   ├── __init__(size=15)
 │   ├── display()
 │   ├── is_valid_move()
 │   ├── make_move()
 │   ├── check_win()
 │   └── is_full()
 │
 ├── human_turn()
 ├── computer_turn()
 │   ├── score_position()
 │   └── check_line()
 └── play_game()
```

## 使用方式

```bash
# 人對人
python gomoku.py P P

# 人對電腦（預設）
python gomoku.py P C

# 電腦對電腦
python gomoku.py C C
```

## 棋盤顯示

```
  0 1 2 ...
0 - - - ...
1 - - - ...
2 - - - ...
...
  0 1 2 ...
```

中心位置為 77。

## 局限性

- 不進行搜尋，只評估單步
- 容易被欺騙（例如假裝看不到威脅）
- 沒有考慮更遠的連子