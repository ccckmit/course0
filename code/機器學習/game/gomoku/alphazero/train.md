# AlphaZero 黑白棋訓練程式說明

## 概述

此檔案實現完整的 AlphaZero 訓練流程，包括遊戲環境、神經網路架構、蒙地卡羅樹搜尋（MCTS）和自我對弈訓練迴圈。

## 理論基礎

### 1. AlphaZero 核心概念

AlphaZero 結合了：
- **深度卷積神經網路**：學習局面評估與策略
- **蒙地卡羅樹搜尋 (MCTS)**：透過模擬遊戲進行決策
- **自我對弈 (Self-Play)**：從零開始學習，不需要人類棋譜

### 2. 遊戲表示

黑白棋（Gomoku）的狀態表示：
- `0`：空位
- `1`：當前玩家
- `-1`：對手玩家

每次移動後翻轉視角：`state = -state`，使神經網路始終以玩家 1 的視角看牌

### 3. 神經網路輸入

三通道輸入：
1. **Player Channel**：當前玩家棋子位置 (== 1)
2. **Opponent Channel**：對手棋子位置 (== -1)
3. **Color Channel**：全 1，表示該誰下棋

## 數學原理

### 1. 神經網路輸出

網路輸出兩個值：
- **Policy** `π`: 每個動作的機率分佈，維度 = `board_size²`
- **Value** `v`: 當前局面的勝率估計，範圍 [-1, 1]

### 2. 損失函數

```
Loss = (v - z)² - π · log(πθ)
     = Value Loss + Policy Loss
```

其中：
- `z`: 實際遊戲結果（1 獲勝，-1 失敗，0 平手）
- `πθ`: 網路輸出的策略機率

### 3. MCTS - PUCT 公式

選擇子的 UCB 值：

```
UCB = Q(s,a) + U(s,a)

Q(s,a) = -value_sum / visit_count  （價值取負因為是對手視角）
U(s,a) = c_puct × P(s,a) × √N(parent) / (1 + N(s,a))
```

### 4. MCTS 策略更新

根據訪問次數計算最終策略：

```
π[a] = N[a] / Σ N[b]
```

## 運作流程

### 訓練迴圈

```
for epoch in epochs:
    1. self_play() ← 產生訓練資料
    2. train_network() ← 訓練神經網路
    3. 保存模型
```

### 自我對弈 (self_play)

```
while True:
    1. MCTS.search(state) → 獲取策略 π
    2. 根據 π 採樣動作
    3. 執行動作，檢查是否結束
    4. 若結束：根據結果更新所有歷史狀態的 value
```

### MCTS 搜尋

```
for _ in num_simulations:
    1. Select: 使用 PUCT 選擇最佳子節點
    2. Expand: 擴展新節點，網路評估
    3. Backpropagate: 向上傳遞價值（每次乘 -1 換視角）
```

## 網路架構

### ResBlock

```
Input → Conv3x3 → BN → ReLU → Conv3x3 → BN → ReLU
                                      ↓
Input ──────────────────────────────→ Add → Output
```

### AlphaZeroNet

```
Input (3×H×W)
    ↓
StartBlock (Conv3x3, BN, ReLU)
    ↓
×4 ResBlock
    ↓
    ├── Policy Head: Conv → BN → ReLU → Flatten → Linear → Softmax
    │
    └── Value Head: Conv → BN → ReLU → Flatten → Linear → Tanh
```

## 超參數

| 參數 | 值 | 說明 |
|------|-----|------|
| board_size | 8 | 棋盤大小 |
| n_in_row | 5 | 連子獲勝數 |
| num_resBlocks | 4 | ResNet 區塊數 |
| num_hidden | 64 | 隱藏層通道數 |
| num_simulations | 100 | MCTS 模擬次數 |
| c_puct | 1.5 | 探索參數 |
| lr | 0.001 | 學習率 |
| weight_decay | 1e-4 | 權重衰減 |
| batch_size | 64 | 批次大小 |

## 關鍵類別

### GomokuGame

- `get_initial_state()`: 初始空棋盤
- `get_next_state(state, action)`: 執行動作
- `get_valid_moves(state)`: 取得合法動作遮罩
- `check_win(state, action)`: 檢查是否獲勝
- `get_reward_and_terminated(state, action)`: 獲取獎勵與結束狀態

### AlphaZeroNet

- `forward(x)`: 前向傳播
- `predict(state)`: 單一局面預測

### MCTS

- `search(state)`: 執行完整搜尋
- `Node`: 樹節點，包含 visit_count, value_sum, prior_prob

### AlphaZeroTrainer

- `self_play()`: 自我對弈產生資料
- `train_network()`: 訓練網路