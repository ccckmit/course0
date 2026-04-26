# AlphaZero 井字棋 - 對戰程式說明

## 概述

此檔案為 AlphaZero 訓練好的井字棋（Tic-Tac-Toe）AI 模型提供人機對戰介面。

## 理論基礎

### 1. Canonical Form（標準形式）

將任意棋盤轉換為「當前玩家視角」：

```python
canonical_board = state * player
```

- 玩家 1 (X)：不變
- 玩家 -1 (O)：翻轉棋盤

這讓神經網路可以專注學習單一玩家的策略。

### 2. MCTS 結果應用

- **訓練時**：使用溫度參數，根據機率分佈採樣（探索）
- **對戰時**：溫度 = 0，直接選擇最高機率的動作（利用）

## 數學原理

### 動作選擇

```python
action = argmax(π)
```

其中 `π` 來自 MCTS 根據訪問次數計算的策略：

```python
π[a] = N[a] / Σ N[b]
```

## 運作流程

### 主迴圈

1. 顯示棋盤
2. 根據玩家類型（P/C）決定誰下棋
3. 人類：輸入 0-8 的位置
4. AI：MCTS 搜尋 → 選擇最佳動作
5. 執行動作，檢查結束狀態
6. 切換玩家

### 棋盤表示

```
 0 | 1 | 2
---+---+---
 3 | 4 | 5
---+---+---
 6 | 7 | 8
```

X 為先手，O 為後手。

## 使用方式

```bash
# 人類 vs AI（預設）
python play.py

# 自訂模式
python play.py P C  # 人類先手 vs 電腦
python play.py C C  # 電腦對戰
python play.py C P  # 電腦先手 vs 人類
```

## 與 train.py 的關係

- 載入 `train.py` 訓練的模型 `model_tictactoe.pth`
- 使用相同的 TicTacToe、ResNet、MCTS 類別
- MCTS 模擬次數設為 200（較高以獲得更強 AI）

## 程式碼架構

```
main()
├── 解析命令列參數
├── 初始化遊戲、模型、MCTS
├── while True:
│   ├── 顯示棋盤
│   ├── 根據玩家類型選擇動作
│   │   ├── human: get_human_move()
│   │   └── computer: get_ai_move() → MCTS.search()
│   ├── 執行動作
│   ├── check_win() 或 is_full()
│   └── 切換玩家
└── 輸出結果
```

## 安全性

使用 PyTorch 新版安全性功能載入模型：

```python
torch.load("model.pth", weights_only=True)
```

這防止執行任意程式碼。