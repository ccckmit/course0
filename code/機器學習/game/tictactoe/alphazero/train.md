# AlphaZero 井字棋訓練程式說明

## 概述

此檔案實現完整的 AlphaZero 訓練流程，用於井字棋（Tic-Tac-Toe）。這是一個簡化的實現，適合學習 AlphaZero 的核心概念。

## 理論基礎

### 1. 井字棋遊戲特性

- **確定性**：沒有隨機因素
- **完全資訊**：雙方可見完整棋盤
- **小狀態空間**：3×3 = 9 格，共 5478 種合法局面
- **必不輸**：先手必不敗（最佳策略是平手）

### 2. 神經網路架構（微型 ResNet）

使用簡化的 ResNet 而非完整的 AlphaZero 架構：

- **輸入**：單通道 3×3 棋盤
- **特徵提取**：2 層 Conv3x3
- **輸出**：
  - Policy Head：9 維機率分佈
  - Value Head：1 維勝率估計

### 3. MCTS - PUCT 公式

```
UCB = Q + c_puct × P × √N(parent) / (1 + N(child))

Q = 1 - ((value_sum / visit_count) + 1) / 2
  = 1 - (average_value + 1) / 2
  = 勝率（以當前玩家視角）
```

### 4. 零和遊戲

井字棋是零和遊戲：
- 一方贏（+1）= 另一方輸（-1）
- 價值在反向傳播時需要取負

## 數學原理

### 1. 損失函數

```
Loss = CrossEntropy(π_target, π_pred) + MSE(v_target, v_pred)
     = -Σ π_target · log(π_pred) + (v - z)²
```

### 2. MCTS 策略

根據訪問次數計算最終策略：

```
π[a] = N[a] / Σ N[b]
```

這類似於策略梯度中的 Softmax 輸出。

### 3. 終局處理

當遊戲結束時（is_terminal = True）：

```
value = -value
```

這是因為「發現自己已經輸了」的那個節點，價值應該是 -1（從它的角度看）。

## 運作流程

### 訓練迴圈

```
for iteration in iterations:
    1. self_play() ← 產生 training data
    2. train(model) ← 更新網路
    3. 保存模型
```

### 自我對弈 (self_play)

```
state = initial
player = 1

while True:
    canonical = state * player
    π = MCTS.search(canonical)
    memory.append((canonical, π, player))

    action = sample(π)  # 根據機率採樣
    state = next_state(state, action)

    value, terminated = get_value(state)

    if terminated:
        for hist_state, hist_π, hist_player in memory:
            # 根據實際結果設定 target value
            v = value if hist_player == player else -value
            training_memory.append((hist_state, hist_π, v))
        return training_memory

    player *= -1
```

### MCTS 搜尋

```
root = Node(state)

for _ in num_mcts_sims:
    node = root

    # Select: 選擇 UCB 最大的子節點
    while node.is_fully_expanded():
        node = node.select()

    value, terminated = game.get_value(node.state)

    # Terminal: 檢查遊戲是否結束
    if terminated:
        value = -value  # 換視角
    else:
        # Evaluate: 網路預測
        policy, v = model(state)
        node.expand(policy)

    # Backpropagate
    node.backpropagate(value)

# 計算最終策略
return visit_counts / sum(visit_counts)
```

## 網路架構

### ResNet

```
Input (1, 3, 3)
    ↓
Conv(1, 64, 3, padding=1) → ReLU
    ↓
Conv(64, 64, 3, padding=1) → ReLU
    ↓
Flatten
    ├── Policy: Linear(576, 9) → Softmax
    └── Value: Linear(576, 1) → Tanh
```

## 超參數

| 參數 | 值 | 說明 |
|------|-----|------|
| num_iterations | 10 | 訓練迭代次數 |
| num_self_play | 50 | 每迭代自我對弈局數 |
| num_mcts_sims | 100 | MCTS 每步模擬次數 |
| c_puct | 1.0 | 探索常數 |
| lr | 0.001 | 學習率 |

## 關鍵類別

### TicTacToe

- `get_initial_state()`: 3×3 零矩陣
- `get_next_state(state, action, player)`: 放置棋子
- `get_valid_moves(state)`: 合法動作遮罩
- `check_win(state, action)`: 檢查四條連線
- `get_canonical_board(state, player)`: 視角轉換

### ResNet

- `forward(x)`: (policy, value) 元組

### MCTS

- `search(state)`: 返回策略分佈

### Node

- `select()`: PUCT 選擇
- `expand(policy)`: 擴展子節點
- `backpropagate(value)`: 反向傳播