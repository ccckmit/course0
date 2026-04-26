# AlphaZero 黑白棋遊戲 - 對戰程式說明

## 概述

此檔案為 AlphaZero 訓練好的黑白棋 AI 模型提供人機對戰介面。使用者可以與訓練好的模型進行對弈，或觀看兩個 AI 互相對戰。

## 理論基礎

### 1. 遊戲視角轉換

在 AlphaZero 架構中，神經網路始終從「當前玩家」的視角來評估盤面：

- **Canonical Form**: 將棋盤乘上 `player` (-1 或 1)，使當前玩家的棋子永遠視為 1，對手的棋子永遠視為 -1
- 這讓同一個神經網路可以處理雙方的回合，無需分別訓練

### 2. MCTS 搜尋結果的應用

訓練時：使用 MCTS 輸出的機率分佈 `π` 進行隨機採樣（溫度參數控制）
對戰時：直接選擇機率最高的動作（溫度 = 0），確保 AI 選擇最強應手

## 數學原理

### 動作選擇

```
action = argmax(π)
```

其中 `π` 是 MCTS 根據訪問次數計算出的策略分佈：

```
π[a] = N[a] / Σ N[b] for all b
```

`N[a]` 是動作 `a` 在 MCTS 模擬中被訪問的次數。

## 運作流程

### 主迴圈

1. 顯示目前棋盤
2. 根據 `player_turn` 決定誰下棋
3. 人類：輸入座標（如 34 代表第 3 列第 4 行）
4. AI：執行 MCTS 搜尋，選擇最佳動作
5. 更新棋盤並檢查是否有五子連線
6. 切換玩家

### 關鍵函式

- `human_turn()`: 處理人類輸入，包含合法性檢查
- `MCTS.search()`: 執行蒙地卡羅樹搜尋
- `game.check_win()`: 檢查五子連線（橫、直、對角線）

## 使用方式

```bash
python play.py
```

預設：人類執黑 (x)，電腦執白 (o)

## 程式碼架構

```
play()
├── 初始化 GomokuGame, AlphaZeroNet, MCTS
├── while True:
│   ├── human_turn() 或 MCTS.search()
│   ├── 更新 state
│   ├── check_win() 檢查勝負
│   └── 切換 player_turn
└── 遊戲結束
```

## 與 train.py 的關係

- 此程式載入 `train.py` 訓練好的模型 (`alphazero_gomoku.pth`)
- 使用相同的 GomokuGame、AlphaZeroNet、MCTS 類別
- MCTS 模擬次數可調高以獲得更強的 AI（預設 400 次）