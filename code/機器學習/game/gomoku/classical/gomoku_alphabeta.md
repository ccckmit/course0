# 黑白棋（五子棋）- 進階 AI（Minimax + Alpha-Beta 剪枝）

## 概述

此檔案是黑白棋的進階 AI 版本，結合了以下技術：
- **Minimax 搜尋**：完整的遊戲樹搜尋
- **Alpha-Beta 剪枝**：減少需要評估的節點數
- **Pattern Matching**：精確的棋型識別

## 理論基礎

### 1. 遊戲樹搜尋

將棋盤遊戲視為 minimax 問題：
- MAX 玩家（AI）嘗試最大化評分
- MIN 玩家（對手）嘗試最小化評分
- 遞迴搜尋到設定深度

```
       MAX
      /   \
   MIN     MIN
    /\      /\
 MAX ... MAX ... 
```

### 2. Alpha-Beta 剪枝

利用上下界排除不可能更好的分支：

- **α**：MAX 的目前最佳選擇（下界）
- **β**：MIN 的目前最佳選擇（上界）

剪枝規則：
- 若 `α >= β`：剪掉該分支

### 3. 候選移動排序

良好的排序可以讓 Alpha-Beta 更快觸發剪枝：
- 首先檢查能產生高分的位置
- 周圍已有棋子的位置優先
- 只取最好的 15 個候選

### 4. Pattern Matching（棋型識別）

將一條線轉為字串，識別常見模式：
- 連五：xxxxx
- 活四：-xxxx-
- 衝四：oxxxx-, -xxxxo
- 活三：--xxx-, -xxx--
- 活二：--xx--

## 數學原理

### 1. 全局評分函數

```
evaluate_board() = Σ score_patterns(line, AI, opponent) 
              - 1.2 × Σ score_patterns(line, opponent, AI)
```

防守權重 1.2 讓 AI 傾向阻止對手的威脅。

### 2. Pattern 評分 (score_patterns)

| 模式 | 分數 | 說明 |
|------|------|------|
| xxxxx（連五） | 1,000,000 | 必勝 |
| -xxxx-（活四） | 100,000 | 必勝 |
| oxxxx-, -xxxxo（衝四） | 10,000 | 威脅 |
| --xxx-, -xxx--（活三） | 5,000 | 強威脅 |
| oxxx--, --xxxo 等（眠三） | 500 | 中威脅 |
| --xx--（活二） | 100 | 潛力 |
| -x-x-（活二） | 50 | 小潛力 |

### 3. 單點評分

候選位置 (r, c) 的價值：
- 攻擊分：若自己下在此處獲得的分數
- 防禦分：若對手下在此處獲得的分數 × 1.2

```
score(r,c) = offense(r,c, AI) + defense(r,c, opponent) × 1.2
```

若進攻分 >= 1,000,000，設為無限大（ Immediate Win）。

### 4. Minimax 遞迴

```
def minimax(depth, alpha, beta, is_maximizing):
    if depth == 0 or abs(score) >= 900000:
        return evaluate_board()

    candidates = get_candidate_moves()

    if is_maximizing:
        max_eval = -∞
        for move in candidates:
            make_move(move)
            eval = minimax(depth-1, alpha, beta, False)
            undo_move(move)
            max_eval = max(max_eval, eval)
            alpha = max(alpha, eval)
            if beta <= alpha: break  # Beta 剪枝
        return max_eval
    else:
        min_eval = +∞
        for move in candidates:
            make_move(move)
            eval = minimax(depth-1, alpha, beta, True)
            undo_move(move)
            min_eval = min(min_eval, eval)
            beta = min(beta, eval)
            if beta <= alpha: break  # Alpha 剪枝
        return min_eval
```

## 運作流程

### 電腦回合 (computer_turn)

```
1. 設定搜尋深度 DEPTH = 3
2. 呼叫 get_candidate_moves() 取得候選位置
3. 對每個候選呼叫 minimax()
4. 選擇最高分的移動
5. 執行落子
```

### 候選移動生成 (get_candidate_moves)

```
1. 掃描所有已有棋子的周圍半徑 2 的範圍
2. 產生候選位置集合
3. 對每個位置評分（攻擊 + 防禦）
4. 排序，取最好的 15 個
```

### 模式評分 (score_patterns)

```
1. 組合字串：opponent + line + opponent
2. 使用字串匹配檢測模式
3. 根據匹配的模式累加分數
```

## 程式碼架構

```
gomoku_alphabeta.py
├── Board
│   ├── display(), is_valid_move(), make_move()
│   ├── undo_move(), check_win(), is_full()
│
├── human_turn()
│
├── score_patterns()      # 棋型識別
├── get_all_lines()       # 全局線
├── get_lines_at()       # 單點線
├── evaluate_board()     # 全局評分
├── get_candidate_moves()  # 候選生成
│
├── minimax()          # Minimax 搜尋
├── computer_turn()    # AI 回合
│
└── play_game()
```

## 使用方式

```bash
# 人對人
python gomoku_alphabeta.py P P

# 人對電腦（預設）
python gomoku_alphabeta.py P C

# 電腦對電腦
python gomoku_alphabeta.py C C
```

## 超參數

| 參數 | 值 | 說明 |
|------|-----|------|
| DEPTH | 3 | Minimax 搜尋深度 |
| board_size | 15 | 棋盤大小 |
| n_in_row | 5 | 連子獲勝數 |
| candidates | 15 | 候選位置數 |
| defense_weight | 1.2 | 防守權重 |

## 效能優化

### 剪枝效果

- 純 Minimax：O(b^d)，b 是分支數
- Alpha-Beta：O(b^(d/2)) 在最佳排序下
- 實際約 O(b^(d/2)) 到 O(b^(3d/4))

### 候選排序

- 先檢查高分位置
- 讓 Alpha-Beta 更早觸發剪枝

### 深度選擇

- DEPTH = 3：在 Python 中約 1-4 秒
- DEPTH = 2：更快但更弱
- 可根據效能調整

## 局限性

- 仍是純搜尋，沒有學習
- 深度受限於運算時間
- 模式庫不完整