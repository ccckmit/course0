# Q-Learning 井字棋訓練程式說明

## 概述

此檔案使用 Deep Q-Learning (DQN) 演算法訓練井字棋 AI。不同於 AlphaZero，DQN 使用傳統的強化學習方法，需要與環境互動產生經驗。

## 理論基礎

### 1. Q-Learning 核心概念

- **Q-函數**：Q(s, a) 表示在狀態 s 執行動作 a 的長期期望獎勵
- **Bellman 方程**：Q(s, a) = r + γ × max_a' Q(s', a')
- **Deep Q-Network**：用神經網路近似 Q-函數

### 2. 經驗回放 (Experience Replay)

將歷史經驗存入記憶庫，隨機採樣進行訓練：
- 打破樣本間的相關性
- 提高樣本效率
- 穩定訓練

### 3. 目標網路 (Target Network)

定期複製 Policy Network 作為目標網路：
- 目標 Q 值更穩定
- 避免訓練震盪
- 每 N 步更新一次

### 4. Epsilon-Greedy 探索

```
以 ε 機率：隨機動作（探索）
以 1-ε 機率：選擇最大 Q 值的動作（利用）

ε 隨時間衰減：ε = ε × decay
```

## 數學原理

### 1. Q-Learning 更新公式

```
Q(s, a) ← Q(s, a) + α [r + γ × max_a' Q(s', a') - Q(s, a)]
```

在神經網路中寫成 MSE Loss：

```
Loss = (Q(s, a) - target_Q)²

target_Q = r + γ × max_a' Q_target(s', a')  (若未結束)
        = r                                     (若結束)
```

### 2. 相對狀態表示

將棋盤轉換為「當前玩家視角」：

```
relative_state = state × player
```

這讓網路學習單一策略，應對雙方。

### 3. 獎勵設計

| 情況 | 獎勵 |
|------|------|
| 贏棋 | +1.0 |
| 輸棋 | -1.0（對手） |
| 平手 | 0.0 |
| 非法動作 | -10.0 |

## 運作流程

### 主迴圈

```python
for episode in EPISODES:
    state = env.reset()
    player = 1

    while True:
        # 視角轉換
        relative_state = state × player

        # 選擇動作 (ε-greedy)
        action = ε-greedy(relative_state)

        # 執行動作
        next_state, reward, done = env.step(action, player)

        # 儲存經驗
        store(relative_state, action, reward, next_state × (-player), done)

        # 若結束且非平手，儲存對手的經驗
        if done and reward == 1.0:
            store(prev_relative_state, prev_action, -1.0, ...)

        # 訓練網路
        if len(memory) > BATCH_SIZE:
            train_batch()

        if done: break

        # 切換玩家
        player *= -1

    # 更新 ε 和 Target Network
    ε *= epsilon_decay
    if episode % TARGET_UPDATE == 0:
        target_net = policy_net
```

### 訓練批次

```python
# 從記憶庫採樣
batch = sample(memory, BATCH_SIZE)

# 計算當前 Q 值
curr_Q = policy_net(states)[actions]

# 計算目標 Q 值
next_Q_max = target_net(next_states).max(1)
target_Q = rewards + GAMMA × next_Q_max × (1 - dones)

# 更新
loss = MSE(curr_Q, target_Q)
optimizer.zero_grad()
loss.backward()
optimizer.step()
```

## 網路架構

### DQN

```
Input (9,)
    ↓
Linear(9, 128) → ReLU
    ↓
Linear(128, 128) → ReLU
    ↓
Linear(128, 9)
    ↓
Output: Q-values for each action
```

## 超參數

| 參數 | 值 | 說明 |
|------|-----|------|
| EPISODES | 20000 | 訓練回合數 |
| GAMMA | 0.99 | 折扣因子 |
| BATCH_SIZE | 128 | 批次大小 |
| TARGET_UPDATE | 500 | Target Network 更新頻率 |
| epsilon | 1.0 | 初始探索率 |
| epsilon_min | 0.05 | 最低探索率 |
| epsilon_decay | 0.9998 | 探索率衰減 |
| lr | 0.001 | 學習率 |
| memory_size | 20000 | 經驗回放大小 |

## 關鍵函式

### TicTacToeEnv

- `reset()`: 重置棋盤
- `step(action, player)`: 執行動作
- `check_winner()`: 檢查八種獲勝組合

### DQN

- `forward(x)`: 前向傳播，輸出 9 維 Q 值

### 訓練邏輯

- `store()`: 儲存經驗到 deque
- `train_batch()`: 從記憶庫採樣訓練
- `ε-greedy()`: 探索-利用平衡