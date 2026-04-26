import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
import random
from collections import deque
import copy

# ==========================================
# 1. 遊戲環境 (Tic-Tac-Toe Environment)
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
                return self.board[a] # 回傳贏家 (1 或 -1)
        if 0 not in self.board:
            return 0 # 平手
        return None # 遊戲繼續

    def step(self, action, player):
        # 違規移動 (在外部就會被擋下，這裡做雙重保險)
        if self.board[action] != 0:
            return self.board.copy(), -10.0, True

        self.board[action] = player
        winner = self.check_winner()

        if winner == player:
            reward = 1.0    # 贏了
            done = True
        elif winner is not None:
            reward = 0.0    # 平手
            done = True
        else:
            reward = 0.0    # 遊戲繼續
            done = False
        
        return self.board.copy(), reward, done

# ==========================================
# 2. 神經網路模型 (Deep Q-Network)
# ==========================================
class DQN(nn.Module):
    def __init__(self):
        super(DQN, self).__init__()
        # 增加神經元數量以提高學習能力
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
# 3. 訓練邏輯與超參數
# ==========================================
env = TicTacToeEnv()
policy_net = DQN()
target_net = DQN()
target_net.load_state_dict(policy_net.state_dict()) # 初始化 Target Network
target_net.eval()

optimizer = optim.Adam(policy_net.parameters(), lr=0.001)
loss_fn = nn.MSELoss()
memory = deque(maxlen=20000)

EPISODES = 20000       # 增加訓練回合數
GAMMA = 0.99           # 折扣因子
BATCH_SIZE = 128
TARGET_UPDATE = 500    # 每 500 回合更新一次 Target Net
epsilon = 1.0
epsilon_min = 0.05
epsilon_decay = 0.9998

print("開始進行強化自我對弈 (Self-Play) 訓練...")

for episode in range(EPISODES):
    state = env.reset()
    current_player = 1 # 1 先手, -1 後手
    
    while True:
        # 視角轉換：對神經網路來說，自己的棋子永遠是 1，對手的永遠是 -1
        # 例如 current_player 是 -1 時，把盤面乘上 -1 餵給網路
        relative_state = state * current_player 
        state_tensor = torch.FloatTensor(relative_state).unsqueeze(0)
        
        available_moves = np.where(state == 0)[0]
        
        # --- 選擇動作 (Epsilon-Greedy) ---
        if random.random() < epsilon:
            action = random.choice(available_moves)
        else:
            with torch.no_grad():
                q_values = policy_net(state_tensor).squeeze().clone()
                invalid_moves = (relative_state != 0)
                q_values[invalid_moves] = -1e9
                action = torch.argmax(q_values).item()
        
        # --- 執行動作 ---
        next_state, reward, done = env.step(action, current_player)
        
        # 儲存經驗：(狀態, 動作, 獎勵, 下一狀態, 是否結束)
        next_relative_state = next_state * (-current_player)
        memory.append((relative_state.copy(), action, reward, next_relative_state.copy(), done))
        
        # 若遊戲結束且非平手，對手(上一步的玩家)得 -1 獎勵
        if done and reward == 1.0:
            prev_relative_state = state * (-current_player)
            available = np.where(state == 0)[0]
            if len(available) > 0:
                # 對手的視角：上一步的可行動作
                for prev_action in available:
                    memory.append((prev_relative_state.copy(), prev_action, -1.0, next_relative_state.copy(), True))
        
        # --- 神經網路學習 ---
        if len(memory) > BATCH_SIZE:
            batch = random.sample(memory, BATCH_SIZE)
            b_states = torch.FloatTensor(np.array([b[0] for b in batch]))
            b_actions = torch.LongTensor(np.array([b[1] for b in batch])).view(-1, 1)
            b_rewards = torch.FloatTensor(np.array([b[2] for b in batch])).view(-1, 1)
            b_next_states = torch.FloatTensor(np.array([b[3] for b in batch]))
            b_dones = torch.FloatTensor(np.array([b[4] for b in batch])).view(-1, 1)

            # 1. 計算目前的 Q 值
            curr_q = policy_net(b_states).gather(1, b_actions)
            
            # 2. 計算目標 Q 值
            with torch.no_grad():
                next_q_values = target_net(b_next_states)
                
                invalid_mask = (b_next_states != 0)
                next_q_values = next_q_values.clone()
                next_q_values[invalid_mask] = -1e9
                
                max_next_q = next_q_values.max(1)[0].view(-1, 1)
                
                target_q = b_rewards - GAMMA * max_next_q * (1 - b_dones)

            # 更新網路
            loss = loss_fn(curr_q, target_q)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

        if done:
            break
            
        state = next_state
        current_player *= -1 # 換對手下

    # --- 更新 Epsilon 與 Target Network ---
    if epsilon > epsilon_min:
        epsilon *= epsilon_decay

    if (episode + 1) % TARGET_UPDATE == 0:
        target_net.load_state_dict(policy_net.state_dict())

    if (episode + 1) % 1000 == 0:
        print(f"訓練回合: {episode + 1}/{EPISODES}, Epsilon: {epsilon:.3f}")

print("訓練完成！模型已具備超強智力（必定平手或獲勝）。")
torch.save(policy_net.state_dict(), "tictactoe_dqn_strong.pth")
print("模型已儲存為 tictactoe_dqn_strong.pth")