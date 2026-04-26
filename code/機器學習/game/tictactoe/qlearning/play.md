# Q-Learning 井字棋 - 對戰程式說明

## 概述

此檔案提供 Q-Learning 訓練的井字棋 AI 的對戰介面。使用者可以與 AI 對戰，或觀看兩個 AI 互相對戰。

## 理論基礎

### 1. DQN 的決策方式

訓練好的 DQN 直接輸出每個動作的 Q 值，選擇最大 Q 值的動作：

```
action = argmax_a Q(s, a)
```

這是純粹的「利用」（exploitation），不包含探索。

### 2. 視角轉換

與訓練時相同，將棋盤轉換為當前玩家視角：

```python
relative_board = board * player
```

其中 player = 1 或 -1。

### 3. 非法動作處理

將非法動作的 Q 值設為極負數，確保不會被選擇：

```python
q_values[invalid_moves] = -1e9
```

## 數學原理

### Q 值選擇

```
Q(s, a) = network(s)[a]

action = argmax(Q(s, a)) such that a is valid
```

## 運作流程

### 主選單

1. 人機對戰 (Human vs AI)
2. 機機對戰 (AI vs AI)
3. 離開

### 人機對戰模式

```
while True:
    顯示棋盤
    if 人類回合:
        輸入位置 (0-8)
    else:
        action = get_ai_action(model, board, turn)
    執行落子
    檢查 winner
    切換 turn
```

### AI vs AI 模式

```
while True:
    顯示棋盤
    action = get_ai_action(model, board, turn)
    執行落子
    檢查 winner
    切換 turn
```

## 棋盤表示

```
 0 | 1 | 2
---+---+---
 3 | 4 | 5
---+---+---
 6 | 7 | 8
```

- X (⭕)：先手，玩家 turn = 1
- O (❌)：後手，玩家 turn = -1

## 使用方式

```bash
python play.py
```

進入主選單後選擇模式。

## 與 train.py 的關係

- 載入 `train.py` 訓練的模型 `tictactoe_dqn_strong.pth`
- 使用相同的 TicTacToeEnv 和 DQN 類別
- AI 只進行「利用」，不進行探索

## 關鍵函式

### get_ai_action()

```python
def get_ai_action(model, board, player):
    relative_board = board * player
    state_tensor = FloatTensor(relative_board).unsqueeze(0)

    q_values = model(state_tensor)

    # 遮罩非法動作
    invalid = (board != 0)
    q_values[0][invalid] = -1e9

    return argmax(q_values)
```

### print_board()

將 9 維陣列轉換為視覺化棋盤，使用 Unicode 符號：
- ⭕ (U+2B55) 代表 X
- ❌ (U+274C) 代表 O

### check_winner()

檢查八種獲勝組合：
- 三橫、三直、兩對角線