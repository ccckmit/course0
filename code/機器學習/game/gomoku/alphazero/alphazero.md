# AlphaZero 理論原理詳解

## 1. 概述

AlphaZero 是 DeepMind 開發的通用棋類 AI 演算法，曾應用於圍棋、西洋棋、將棋。它結合了**深度神經網路**與**蒙地卡羅樹搜尋**，完全從零開始學習，不需要人類棋譜或先驗知識。

## 2. 核心概念

### 2.1 輸入表示

將棋盤狀態表示為三通道張量：

$$
\mathbf{x}_t = \begin{bmatrix}
\mathbf{1}_{t}^{\text{self}} \\
\mathbf{1}_{t}^{\text{opp}} \\
\mathbf{1}_{t}^{\text{color}}
\end{bmatrix} \in \mathbb{R}^{3 \times H \times W}
$$

其中：
- $\mathbf{1}_{t}^{\text{self}}$：當前玩家的棋子位置（值為 1）
- $\mathbf{1}_{t}^{\text{opp}}$：對手的棋子位置（值為 1）
- $\mathbf{1}_{t}^{\text{color}}$：全 1 矩陣，表示該誰下棋

### 2.2 輸出表示

網路輸出兩個值：
- **Policy** $\mathbf{p}_\theta(s)$：每個動作 $a$ 的機率分佈，維度為 $|A|$
- **Value** $v_\theta(s)$：當前局面的勝率估計，範圍 $[-1, 1]$

$$
(\mathbf{p}_\theta(s), v_\theta(s)) = f_\theta(\mathbf{x}_t)
$$

### 2.3 視角轉換（Canonical Form）

為使單一網路可處理雙方回合，將棋盤轉換為「當前玩家視角」：

$$
s^{\text{cano}} = s \times p
$$

其中 $p \in \{-1, +1\}$ 為當前玩家。這樣：
- 當前玩家的棋子永遠標記為 1
- 對手的棋子永遠標記為 -1

## 3. 蒙地卡羅樹搜尋（MCTS）

### 3.1 搜尋架構

每個節點代表一個遊戲狀態，結構如下：

```python
class Node:
    parent: Node
    children: dict(action -> Node)
    visit_count: int          # N(s,a)
    value_sum: float        # W(s,a)
    prior_prob: float      # P(s,a)
    state: np.ndarray
```

### 3.2 PUCT 選擇公式

在 Selection 階段，使用 PUCT（PUCT = Parent + UCB-Tuned）公式選擇最佳子節點：

$$
a^* = \arg\max_a \left[ Q(s,a) + c_{puct} \cdot P(s,a) \frac{\sqrt{N(\text{parent})}}{1 + N(s,a)} \right]
$$

其中：
- $Q(s,a)$ 為動作價值：

$$
Q(s,a) = -\frac{W(s,a)}{N(s,a)}
$$

取負是因為子節點是從對手視角計算的價值。
- $c_{puct}$ 為探索常數，通常設為 1.0～2.0
- $P(s,a)$ 為先驗機率，來自神經網路輸出
- $N(s,a)$ 為訪問次數

### 3.3 搜尋流程

```
MCTS Search(s_0):
    root = Node(state=s_0)

    # Expand root with network prior
    (p, v_0) = f_theta(root.state)
    valid = get_valid_moves(root.state)
    p = p * valid / sum(p)

    for action in valid moves:
        root.children[action] = Node(parent=root, prior_prob=p[action])

    for i = 1 to N_simulations:
        node = root

        # 1. Selection
        while node.is_expanded():
            action = PUCT_Select(node)
            node = node.children[action]
            state = get_next_state(node.state, action)

        # 2. Evaluate & Expand
        (p, v) = f_theta(state)
        valid = get_valid_moves(state)
        reward, terminated = get_reward(state)

        if not terminated:
            for action in valid moves:
                node.children[action] = Node(parent=node, prior_prob=p[action])
        else:
            v = reward  # 使用實際獎勵

        # 3. Backpropagate
        while node is not None:
            node.N += 1
            node.W += v
            node = node.parent
            v = -v  # 切換視角

    # 計算最終策略
    pi[a] = N(root.children[a]) / sum(N)
    return pi
```

### 3.4 價值反向傳播

由於是零和遊戲，從子節點反向傳播時需要取負：

$$
v_{\text{parent}} = -v_{\text{child}}
$$

這反映了「對手的勝利就是我的失敗」的概念。

## 4. 自我對弈（Self-Play）

### 4.1 對弈流程

```
SelfPlay():
    memory = []
    s = initial_state()
    player = 1

    while True:
        s_cano = s * player
        pi = MCTS.Search(s_cano)

        memory.append((s_cano, pi, player))

        action = sample(pi)  # 根據機率採樣
        s = next_state(s, action)

        (r, done) = get_reward(s)
        if done:
            # 根據實際結果設定 target value
            for (s_h, pi_h, p_h) in memory:
                v = r if p_h == player else -r
                training_set.append((s_h, pi_h, v))
            return training_set

        player = -player
```

### 4.2 溫度參數

訓練時使用溫度參數控制探索：

$$
\pi[a] \propto N[a]^{1/T}
$$

- $T = 0$：只選擇最高訪問次數的動作（貪心）
- $T = 1$：原始訪問次數比例
- $T > 1$：增加隨機性（探索）

## 5. 訓練目標

### 5.1 損失函數

結合 Policy Loss 和 Value Loss：

$$
\mathcal{L}(\theta) = (v - z)^2 - \pi_t \cdot \log(\mathbf{p})
$$

其中：
- $z \in \{-1, 0, +1\}$：實際遊戲結果（贏、平、輸）
- $\pi_t$：MCTS 產生的目標策略
- $\mathbf{p}$：網路輸出的預測策略

### 5.2 損失分解

$$
\mathcal{L} = \underbrace{(v - z)^2}_{\text{Value Loss}} + \underbrace{-\sum_a \pi_t(a) \log(p_a)}_{\text{Policy Loss}}
$$

其中 Policy Loss 是交叉熵：

$$
H(\pi_t, p) = -\pi_t \log p
$$

### 5.3 梯度更新

使用 Adam 優化器：

$$
\theta \leftarrow \theta - \eta \nabla \mathcal{L}(\theta)
$$

其中學習率通常 $\eta = 0.001$，並加入權重衰減 $10^{-4}$。

## 6. 完整訓練流程

```
AlphaZero Training(game, f_theta):
    for iteration in ITERATIONS:
        # 1. Self-Play
        for game in GAMES:
            data += SelfPlay(game, f_theta, MCTS)

        # 2. Training
        for step in TRAIN_STEPS:
            batch = sample(data, BATCH_SIZE)
            (p, v) = f_theta(batch.states)
            loss = (v - batch.z)^2 - batch.pi * log(p)
            optimizer.step()

        # 3. Evaluate
        if iteration % EVAL_FREQ == 0:
            test_performance = Evaluate(f_theta)
            if test_performance > best:
                best = test_performance
                save_model(f_theta)
```

## 7. 數學總結

| 符號 | 意義 |
|------|------|
| $s$ | 遊戲狀態 |
| $a$ | 動作 |
| $\mathbf{p}, v$ | 網路輸出的策略、價值 |
| $\pi$ | MCTS 策略 |
| $z$ | 遊戲結果 |
| $N(s,a)$ | 訪問次數 |
| $W(s,a)$ | 價值總和 |
| $Q(s,a)$ | 平均價值 |
| $c_{puct}$ | 探索常數 |

## 8. 與傳統方法的比較

| 特性 | AlphaZero | 傳統搜尋 | Q-Learning |
|------|---------|----------|----------|
| 先驗知識 | 無 | 無 | 無 |
| 搜尋方式 | MCTS | Minimax | 無 |
| 學習方式 | Self-Play | 無 | 環境互動 |
| 評估 | 網路輸出 | 啟發式 | 網路輸出 |
| 可擴展性 | 高 | 低 | 中 |

## 9. 關鍵創新

1. **結合深度學習與搜尋**：網路引導搜尋，搜尋產生訓練數據
2. **殘差網路**：使用 ResNet 結構加深網路
3. **同時輸出 policy 和 value**：多任務學習
4. **完全從零開始**：不需要人類知識