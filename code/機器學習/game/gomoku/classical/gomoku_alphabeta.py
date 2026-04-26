'''
增強版五子棋遊戲 - 命令列版本 (Minimax + Alpha-Beta Pruning + Pattern Matching)

使用方法：
人對人下  ：python gomoku_claude.py P P
人對電腦  ：python gomoku_claude.py P C
電腦對電腦：python gomoku_claude.py C C
'''

import sys
import time
import random

class Board:
    def __init__(self, size=15):
        self.size = size
        self.grid = [['-' for _ in range(size)] for _ in range(size)]
    
    def display(self):
        print('\n  ' + ' '.join([hex(i)[2:] for i in range(self.size)]))
        for i in range(self.size):
            print(hex(i)[2:] + ' ' + ' '.join(self.grid[i]) + ' ' + hex(i)[2:])
        print('  ' + ' '.join([hex(i)[2:] for i in range(self.size)]) + '\n')
    
    def is_valid_move(self, row, col):
        if row < 0 or row >= self.size or col < 0 or col >= self.size:
            return False
        return self.grid[row][col] == '-'
    
    def make_move(self, row, col, player):
        if self.is_valid_move(row, col):
            self.grid[row][col] = player
            return True
        return False
        
    def undo_move(self, row, col):
        self.grid[row][col] = '-'
    
    def check_win(self, row, col, player):
        directions = [[(0, 1), (0, -1)], [(1, 0), (-1, 0)], [(1, 1), (-1, -1)], [(1, -1), (-1, 1)]]
        for direction in directions:
            count = 1
            for dx, dy in direction:
                for step in range(1, 5):
                    r, c = row + dx * step, col + dy * step
                    if 0 <= r < self.size and 0 <= c < self.size and self.grid[r][c] == player:
                        count += 1
                    else:
                        break
            if count >= 5: return True
        return False
    
    def is_full(self):
        for row in self.grid:
            if '-' in row: return False
        return True

def human_turn(board, player):
    while True:
        try:
            move = input(f'請輸入{player}的位置 (例如: 88 或 8 8): ').replace(' ', '')
            if len(move) != 2:
                print('請輸入兩個字元，分別代表列和行')
                continue
            row, col = int(move[0], 16), int(move[1], 16)
            if board.make_move(row, col, player):
                return row, col
            else:
                print('無效的移動，請重試')
        except ValueError:
            print('請輸入有效的十六進制數字')

# ----------------- 增強版 AI 核心演算法 ----------------- #

def score_patterns(line_str, p, opp):
    """
    透過字串比對來評估一條線上的分數，精準識別五子棋常見棋型
    將線的兩端加上對手的棋子當作邊界(牆壁)，方便判定死活
    """
    s = opp + line_str + opp
    score = 0
    
    # 連五 (Win)
    if p * 5 in s:
        return 1000000
        
    # 活四 (-xxxx-)
    if f"-{p*4}-" in s:
        return 100000
        
    # 衝四/死四 (oxxxx-, -xxxxo, x-xxx, xx-xx, xxx-x)
    if f"{opp}{p*4}-" in s or f"-{p*4}{opp}" in s or \
       f"{p}{p}-{p}{p}" in s or f"{p}-{p}{p}{p}" in s or f"{p}{p}{p}-{p}" in s:
        score += 10000
        
    # 活三 (--xxx-, -xxx--, -x-xx-, -xx-x-)
    if f"--{p*3}-" in s or f"-{p*3}--" in s or \
       f"-{p}-{p*2}-" in s or f"-{p*2}-{p}-" in s:
        score += 5000
        
    # 眠三/死三 (有空間但已被堵住一邊的三子)
    score += s.count(f"{opp}{p*3}--") * 500
    score += s.count(f"--{p*3}{opp}") * 500
    score += s.count(f"{opp}-{p*3}-") * 500
    score += s.count(f"-{p*3}-{opp}") * 500
    score += s.count(f"{opp}{p*2}-{p}-") * 500
    score += s.count(f"-{p}-{p*2}{opp}") * 500
    score += s.count(f"{p}-{p}-{p}") * 500
    
    # 活二
    score += s.count(f"--{p*2}--") * 100
    score += s.count(f"-{p}-{p}--") * 50
    score += s.count(f"--{p}-{p}-") * 50
    
    return score

def get_all_lines(board):
    """獲取棋盤上所有長度 >= 5 的橫線、直線、對角線字串"""
    lines = []
    size = board.size
    
    for r in range(size):
        lines.append("".join(board.grid[r])) # 橫
    for c in range(size):
        lines.append("".join(board.grid[r][c] for r in range(size))) # 直
        
    for d in range(-size + 5, size - 4): # 主對角線 (\)
        diag = [board.grid[r][r - d] for r in range(size) if 0 <= r - d < size]
        lines.append("".join(diag))
    for d in range(4, 2 * size - 5): # 副對角線 (/)
        diag = [board.grid[r][d - r] for r in range(size) if 0 <= d - r < size]
        lines.append("".join(diag))
        
    return lines

def get_lines_at(board, r, c):
    """只獲取通過 (r, c) 這個點的 4 條線，用來快速評估單點價值"""
    size = board.size
    lines = []
    lines.append("".join(board.grid[r])) # 橫
    lines.append("".join(board.grid[i][c] for i in range(size))) # 直
    
    # 主對角線
    offset = min(r, c)
    start_r, start_c = r - offset, c - offset
    diag1 = []
    while start_r < size and start_c < size:
        diag1.append(board.grid[start_r][start_c])
        start_r += 1; start_c += 1
    lines.append("".join(diag1))
    
    # 副對角線
    offset = min(r, size - 1 - c)
    start_r, start_c = r - offset, c + offset
    diag2 = []
    while start_r < size and start_c >= 0:
        diag2.append(board.grid[start_r][start_c])
        start_r += 1; start_c -= 1
    lines.append("".join(diag2))
    
    return lines

def evaluate_board(board, ai_player, opponent):
    """全局評估函數"""
    lines = get_all_lines(board)
    ai_score = sum(score_patterns(line, ai_player, opponent) for line in lines)
    opp_score = sum(score_patterns(line, opponent, ai_player) for line in lines)
    
    # 乘以 1.2 讓 AI 性格稍微偏向防守，遇到同等威脅時優先破壞對手陣型
    return ai_score - opp_score * 1.2

def get_candidate_moves(board, ai_player, opponent, is_maximizing):
    """獲取候選步，並加入強大的攻防啟發式排序，確保關鍵防守步不會被修剪"""
    candidates = set()
    for r in range(board.size):
        for c in range(board.size):
            if board.grid[r][c] != '-':
                # 掃描周圍半徑為2的格子
                for dr in [-2, -1, 0, 1, 2]:
                    for dc in [-2, -1, 0, 1, 2]:
                        nr, nc = r + dr, c + dc
                        if 0 <= nr < board.size and 0 <= nc < board.size and board.grid[nr][nc] == '-':
                            candidates.add((nr, nc))
    
    if not candidates:
        return [(board.size // 2, board.size // 2)]
        
    move_scores = []
    current_player = ai_player if is_maximizing else opponent
    other_player = opponent if is_maximizing else ai_player
    
    for r, c in candidates:
        # 評分邏輯：該點的價值 = 我下這步獲得的攻擊分 + 阻止對手下這步的防禦分
        
        # 1. 測量攻擊分
        board.grid[r][c] = current_player
        lines_curr = get_lines_at(board, r, c)
        offense = sum(score_patterns(line, current_player, other_player) for line in lines_curr)
        
        # 2. 測量防禦分 (如果換對手下在這裡)
        board.grid[r][c] = other_player
        lines_other = get_lines_at(board, r, c)
        defense = sum(score_patterns(line, other_player, current_player) for line in lines_other)
        
        board.grid[r][c] = '-' # 復原
        
        # 若能自己直接贏，設為無限大優先級
        if offense >= 1000000:
            score = float('inf')
        else:
            score = offense + defense * 1.2 # 防守權重略高，確保不漏算致命威脅
            
        move_scores.append((score, (r, c)))
        
    move_scores.sort(key=lambda x: x[0], reverse=True)
    return [move for score, move in move_scores[:15]] # 只取最好的15步給Alpha-Beta深入評估

def minimax(board, depth, alpha, beta, is_maximizing, ai_player, opponent):
    board_score = evaluate_board(board, ai_player, opponent)
    
    # 停止條件：深度達標，或是已經出現勝負 (分數超過900000)
    if depth == 0 or abs(board_score) >= 900000 or board.is_full():
        return board_score

    candidates = get_candidate_moves(board, ai_player, opponent, is_maximizing)
    
    if is_maximizing:
        max_eval = -float('inf')
        for r, c in candidates:
            board.make_move(r, c, ai_player)
            eval_score = minimax(board, depth - 1, alpha, beta, False, ai_player, opponent)
            board.undo_move(r, c)
            max_eval = max(max_eval, eval_score)
            alpha = max(alpha, eval_score)
            if beta <= alpha: break
        return max_eval
    else:
        min_eval = float('inf')
        for r, c in candidates:
            board.make_move(r, c, opponent)
            eval_score = minimax(board, depth - 1, alpha, beta, True, ai_player, opponent)
            board.undo_move(r, c)
            min_eval = min(min_eval, eval_score)
            beta = min(beta, eval_score)
            if beta <= alpha: break
        return min_eval

def computer_turn(board, player):
    print(f'電腦({player}) 正在深思熟慮中...')
    opponent = 'o' if player == 'x' else 'x'
    
    # 由於優化了特徵識別與排序，搜尋深度可以設定為 3
    # 在純 Python 中運算時間約為 1~4 秒，若嫌慢可改回 2
    DEPTH = 3 
    
    best_score = -float('inf')
    best_move = None
    alpha = -float('inf')
    beta = float('inf')
    
    candidates = get_candidate_moves(board, player, opponent, True)
    
    if len(candidates) == 1 and candidates[0] == (board.size//2, board.size//2):
        best_move = candidates[0]
    else:
        for r, c in candidates:
            board.make_move(r, c, player)
            score = minimax(board, DEPTH - 1, alpha, beta, False, player, opponent)
            board.undo_move(r, c)
            
            if score > best_score:
                best_score = score
                best_move = (r, c)
            alpha = max(alpha, best_score)
            
    if not best_move: best_move = candidates[0]
        
    row, col = best_move
    board.make_move(row, col, player)
    print(f'電腦({player}) 決定下在: {hex(row)[2:]}{hex(col)[2:]}')
    return row, col

# ------------------------------------------------- #

def play_game(player_o, player_x):
    board = Board()
    board.display()
    
    while True:
        if player_o.upper() == 'P': row, col = human_turn(board, 'o')
        else: row, col = computer_turn(board, 'o')
        
        board.display()
        if board.check_win(row, col, 'o'):
            print('===== 恭喜，o 獲勝！ =====')
            break
        if board.is_full():
            print('===== 平局！ =====')
            break
            
        if player_x.upper() == 'P': row, col = human_turn(board, 'x')
        else: row, col = computer_turn(board, 'x')
        
        board.display()
        if board.check_win(row, col, 'x'):
            print('===== 恭喜，x 獲勝！ =====')
            break
        if board.is_full():
            print('===== 平局！ =====')
            break

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print('使用方式: python gomoku_claude.py [o玩家類型] [x玩家類型]')
        print('玩家類型: P=人類, C=電腦')
        print('預設使用: P C (人類先手, 電腦後手)')
        player_o, player_x = 'P', 'C'
    else:
        player_o, player_x = sys.argv[1], sys.argv[2]
    
    play_game(player_o, player_x)