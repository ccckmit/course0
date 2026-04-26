# play.py
import sys
import torch
import numpy as np
from train import GomokuGame, AlphaZeroNet, MCTS # 引入剛寫好的架構

def print_board(state, game):
    chars = {0: '-', 1: 'x', -1: 'o'}
    print('\n  ' + ' '.join([str(i) for i in range(game.board_size)]))
    for i in range(game.board_size):
        row_str = ' '.join([chars[state[i, j]] for j in range(game.board_size)])
        print(f"{i} {row_str} {i}")
    print('  ' + ' '.join([str(i) for i in range(game.board_size)]) + '\n')

def human_turn(state, game):
    valid_moves = game.get_valid_moves(state)
    while True:
        try:
            move = input('請輸入行列 (例: 34 代表第3列第4行): ')
            r, c = int(move[0]), int(move[1])
            action = r * game.board_size + c
            if 0 <= r < game.board_size and 0 <= c < game.board_size and valid_moves[action]:
                return action
            print("無效的移動，該位置已被佔用或超出範圍。")
        except:
            print("格式錯誤。請輸入兩個數字。")

def play():
    # 注意：這裡的 size 要跟訓練時一樣
    game = GomokuGame(board_size=8, n_in_row=5) 
    
    # 載入模型
    model = AlphaZeroNet(game)
    try:
        model.load_state_dict(torch.load("alphazero_gomoku.pth", map_location=model.device))
        print("成功載入訓練模型！")
    except FileNotFoundError:
        print("找不到 alphazero_gomoku.pth，將使用隨機初始化的未訓練 AI (非常笨)。")
        
    model.eval()
    
    # 遊玩時，MCTS 模擬次數可以提高，AI 會更強 (預設 400 次)
    mcts = MCTS(game, model, num_simulations=400, c_puct=1.0)
    
    state = game.get_initial_state()
    player_turn = 1 # 1=人類(x), -1=電腦(o)
    
    print("遊戲開始！你執黑 (x)，電腦執白 (o)。")
    print_board(state, game)
    
    last_action = -1
    while True:
        if player_turn == 1:
            action = human_turn(state, game)
        else:
            print("AI 思考中...")
            # 注意：MCTS 搜尋永遠是從「當前視角 (1)」來搜尋
            # 所以傳入 MCTS 時，要將 state 轉化為讓電腦以為自己是 1
            ai_view_state = state * player_turn
            
            # 獲取機率 (因為是遊玩，直接選機率最高的走法，不進行隨機探索)
            pi = mcts.search(ai_view_state)
            action = np.argmax(pi)
            r, c = action // game.board_size, action % game.board_size
            print(f"AI 下在: {r}{c}")
            
        # 更新狀態 (這裡使用原視角的 state 下棋)
        r, c = action // game.board_size, action % game.board_size
        state[r, c] = player_turn
        
        print_board(state, game)
        
        # 檢查輸贏
        # 因為 check_win 是以當前下的子為準，我們需先還原視角檢查
        ai_view_state = state * player_turn 
        if game.check_win(ai_view_state, action):
            winner = "你 (x)" if player_turn == 1 else "AI (o)"
            print(f"===== 遊戲結束，{winner} 獲勝！ =====")
            break
            
        if np.sum(game.get_valid_moves(state)) == 0:
            print("===== 遊戲結束，平局！ =====")
            break
            
        player_turn *= -1

if __name__ == "__main__":
    play()