import torch
import numpy as np
import sys
from train import TicTacToe, ResNet, MCTS

def print_board(state):
    symbols = {1: 'X', -1: 'O', 0: ' '}
    print("\n-------------")
    for row in range(3):
        row_str = "| "
        for col in range(3):
            row_str += symbols[state[row, col]] + " | "
        print(row_str)
        print("-------------")

def get_human_move(game, state, player_symbol):
    valid_moves = game.get_valid_moves(state)
    while True:
        try:
            move = int(input(f"輪到你了 ({player_symbol})。請輸入位置 (0-8)："))
            if 0 <= move <= 8 and valid_moves[move] == 1:
                return move
            else:
                print("無效的位置或該處已有棋子，請重新輸入！")
        except ValueError:
            print("請輸入有效的數字 (0-8)！")

def get_ai_move(mcts, game, state, player):
    canonical_state = game.get_canonical_board(state, player)
    # 實際對戰時溫度設為 0，直接選最強的一步
    action_probs = mcts.search(canonical_state)
    return np.argmax(action_probs)

def main():
    # 預設模式：玩家先手 (P C)
    p1_type = 'P'
    p2_type = 'C'

    if len(sys.argv) == 3:
        p1_type = sys.argv[1].upper()
        p2_type = sys.argv[2].upper()
    else:
        print("使用方式: python play.py [P1] [P2]")
        print("P: 人類 (Person), C: 電腦 (Computer)")
        print("範例: python play.py C C (電腦對戰)")
        print("預設使用 P C 模式啟動...\n")

    game = TicTacToe()
    model = ResNet(game)
    
    try:
        # 使用 weights_only=True 是為了安全性，符合新版 PyTorch 規範
        model.load_state_dict(torch.load("model_tictactoe.pth", weights_only=True, map_location=torch.device('cpu')))
        model.eval()
    except FileNotFoundError:
        print("錯誤：找不到 model_tictactoe.pth，請先執行 train.py 訓練模型！")
        return

    args = {'num_mcts_sims': 200, 'c_puct': 1.0}
    mcts = MCTS(game, args, model)
    
    state = game.get_initial_state()
    player = 1 # 1 是 X (先手), -1 是 O (後手)
    
    # 定義玩家類型對照表
    player_types = {1: p1_type, -1: p2_type}
    player_symbols = {1: 'X', -1: 'O'}

    while True:
        print_board(state)
        current_type = player_types[player]
        current_symbol = player_symbols[player]

        if current_type == 'P':
            action = get_human_move(game, state, current_symbol)
        else:
            print(f"AI ({current_symbol}) 思考中...")
            action = get_ai_move(mcts, game, state, player)
            print(f"AI 下在位置：{action}")

        state = game.get_next_state(state, action, player)
        value, is_terminal = game.get_value_and_terminated(state, action)
        
        if is_terminal:
            print_board(state)
            if value == 1:
                winner_type = "人類" if current_type == 'P' else "AI"
                print(f"遊戲結束！勝者是 {current_symbol} ({winner_type})")
            else:
                print("遊戲結束！平手！")
            break
            
        player *= -1

if __name__ == "__main__":
    main()