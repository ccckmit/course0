import torch
import torch.nn as nn
import numpy as np
import time

# ==========================================
# 1. 遊戲環境與模型結構 (必須與訓練時完全一致)
# ==========================================
class TicTacToeEnv:
    def __init__(self):
        self.board = np.zeros(9, dtype=np.float32)

    def reset(self):
        self.board = np.zeros(9, dtype=np.float32)
        return self.board.copy()

    def check_winner(self):
        win_conditions = [(0,1,2), (3,4,5), (6,7,8), (0,3,6), (1,4,7), (2,5,8), (0,4,8), (2,4,6)]
        for a, b, c in win_conditions:
            if self.board[a] == self.board[b] == self.board[c] and self.board[a] != 0:
                return self.board[a]
        if 0 not in self.board:
            return 0 # 平手
        return None

    def step(self, action, player):
        self.board[action] = player
        return self.board.copy(), self.check_winner()

class DQN(nn.Module):
    def __init__(self):
        super(DQN, self).__init__()
        self.net = nn.Sequential(
            nn.Linear(9, 128),
            nn.ReLU(),
            nn.Linear(128, 128),
            nn.ReLU(),
            nn.Linear(128, 9)
        )
    def forward(self, x):
        return self.net(x)

# ==========================================
# 2. 輔助函式 (顯示與 AI 決策)
# ==========================================
def print_board(board):
    """將陣列轉換為視覺化的井字棋盤"""
    symbols = {1: '⭕', -1: '❌', 0: '  '}
    print("\n")
    for row in range(3):
        r = [symbols[board[row*3 + i]] for i in range(3)]
        print(f" {r[0]} | {r[1]} | {r[2]} ")
        if row < 2:
            print("----+----+----")
    print("\n")

def get_ai_action(model, board, player):
    """AI 根據當前盤面选出最佳動作 (不再探索，純利用)"""
    relative_board = board * player
    state_tensor = torch.FloatTensor(relative_board).unsqueeze(0)
    with torch.no_grad():
        q_values = model(state_tensor).clone()
        invalid_moves = (board != 0)
        q_values[0][invalid_moves] = -1e9
        action = torch.argmax(q_values).item()
    return action

# ==========================================
# 3. 遊戲模式邏輯
# ==========================================
def human_vs_ai(env, model):
    print("\n=== 人機對戰開始 ===")
    print("輸入對應的數字 (0-8) 來下棋：")
    print(" 0 | 1 | 2 \n---+---+---\n 3 | 4 | 5 \n---+---+---\n 6 | 7 | 8 ")
    
    board = env.reset()
    turn = 1 # 1: 人類(⭕), -1: AI(❌)
    
    while True:
        print_board(board)
        
        if turn == 1:
            # 人類回合
            try:
                move = int(input("換你了 (⭕)，請輸入位置 (0-8): "))
                if move < 0 or move > 8 or board[move] != 0:
                    print("⚠️ 無效的位置，請重新輸入！")
                    continue
            except ValueError:
                print("⚠️ 請輸入數字！")
                continue
        else:
            # AI 回合
            print("AI (❌) 思考中...")
            time.sleep(1)
            move = get_ai_action(model, board, turn)
            print(f"AI 選擇了位置: {move}")

        # 執行落子
        board, winner = env.step(move, turn)
        
        # 檢查勝負
        if winner is not None:
            print_board(board)
            if winner == 1:
                print("🎉 恭喜你，你贏了！")
            elif winner == -1:
                print("🤖 AI 贏了！弱小的人類啊。")
            else:
                print("🤝 平手！勢均力敵。")
            break
            
        turn *= -1 # 換對手

def ai_vs_ai(env, model):
    print("\n=== 機機對戰開始 ===")
    board = env.reset()
    turn = 1
    
    while True:
        player_name = "AI-1 (⭕)" if turn == 1 else "AI-2 (❌)"
        print(f"{player_name} 思考中...")
        time.sleep(1)
        
        move = get_ai_action(model, board, turn)
        print(f"{player_name} 選擇了位置: {move}")
        
        board, winner = env.step(move, turn)
        print_board(board)
        
        if winner is not None:
            if winner == 1:
                print("🏆 AI-1 (⭕) 獲勝！")
            elif winner == -1:
                print("🏆 AI-2 (❌) 獲勝！")
            else:
                print("🤝 完美的防守，平手結束！")
            break
            
        turn *= -1

# ==========================================
# 4. 主程式啟動與選單
# ==========================================
if __name__ == "__main__":
    # 初始化環境與模型
    env = TicTacToeEnv()
    model = DQN()
    
    # 載入權重
    try:
        model.load_state_dict(torch.load("tictactoe_dqn_strong.pth"))
        model.eval()
        print("✅ 成功載入模型權重 (tictactoe_dqn_strong.pth)")
    except FileNotFoundError:
        print("❌ 找不到 tictactoe_dqn_strong.pth，將使用隨機初始化的智障 AI。")

    # 主選單
    while True:
        print("\n" + "="*20)
        print(" 井字遊戲 強化學習展示 ")
        print("="*20)
        print("1. 人機對戰 (你 vs AI)")
        print("2. 機機對戰 (AI vs AI)")
        print("3. 離開遊戲")
        
        choice = input("請選擇模式 (1/2/3): ")
        
        if choice == '1':
            human_vs_ai(env, model)
        elif choice == '2':
            ai_vs_ai(env, model)
        elif choice == '3':
            print("遊戲結束，下次見！")
            break
        else:
            print("⚠️ 無效的選擇，請輸入 1, 2 或 3。")