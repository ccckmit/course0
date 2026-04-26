# train.py
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import numpy as np
import math
import random
import os
from collections import deque

# ----------------- 1. 遊戲邏輯 (Game) -----------------
class GomokuGame:
    def __init__(self, board_size=8, n_in_row=5):
        self.board_size = board_size
        self.n_in_row = n_in_row

    def get_initial_state(self):
        # 0: 空, 1: 當前玩家, -1: 對手
        return np.zeros((self.board_size, self.board_size), dtype=np.int8)

    def get_next_state(self, state, action):
        row = action // self.board_size
        col = action % self.board_size
        next_state = np.copy(state)
        next_state[row, col] = 1
        # 翻轉視角：下一個回合的 1 變成對手，所以把整個棋盤乘 -1
        return -next_state

    def get_valid_moves(self, state):
        return (state.flatten() == 0).astype(np.uint8)

    def check_win(self, state, action):
        if action == -1: return False
        row = action // self.board_size
        col = action % self.board_size
        player = state[row, col]

        directions = [(1, 0), (0, 1), (1, 1), (1, -1)]
        for dr, dc in directions:
            count = 1
            for step in [1, -1]:
                r, c = row, col
                while True:
                    r += dr * step
                    c += dc * step
                    if 0 <= r < self.board_size and 0 <= c < self.board_size and state[r, c] == player:
                        count += 1
                    else:
                        break
            if count >= self.n_in_row:
                return True
        return False

    def get_reward_and_terminated(self, state, action):
        if self.check_win(state, action):
            return 1, True
        if np.sum(self.get_valid_moves(state)) == 0:
            return 0, True # 平手
        return 0, False

# ----------------- 2. 神經網路 (Neural Network) -----------------
class ResBlock(nn.Module):
    def __init__(self, num_hidden):
        super().__init__()
        self.conv1 = nn.Conv2d(num_hidden, num_hidden, kernel_size=3, padding=1)
        self.bn1 = nn.BatchNorm2d(num_hidden)
        self.conv2 = nn.Conv2d(num_hidden, num_hidden, kernel_size=3, padding=1)
        self.bn2 = nn.BatchNorm2d(num_hidden)

    def forward(self, x):
        residual = x
        x = F.relu(self.bn1(self.conv1(x)))
        x = self.bn2(self.conv2(x))
        x += residual
        return F.relu(x)

class AlphaZeroNet(nn.Module):
    def __init__(self, game, num_resBlocks=4, num_hidden=64):
        super().__init__()
        self.device = torch.device("cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu")
        self.board_size = game.board_size
        self.action_size = game.board_size ** 2
        
        self.startBlock = nn.Sequential(
            nn.Conv2d(3, num_hidden, kernel_size=3, padding=1),
            nn.BatchNorm2d(num_hidden),
            nn.ReLU()
        )
        
        self.backBone = nn.ModuleList([ResBlock(num_hidden) for _ in range(num_resBlocks)])
        
        # Policy Head (輸出走法機率)
        self.policyHead = nn.Sequential(
            nn.Conv2d(num_hidden, 32, kernel_size=3, padding=1),
            nn.BatchNorm2d(32),
            nn.ReLU(),
            nn.Flatten(),
            nn.Linear(32 * self.board_size * self.board_size, self.action_size)
        )
        
        # Value Head (輸出勝率估計 -1 ~ 1)
        self.valueHead = nn.Sequential(
            nn.Conv2d(num_hidden, 3, kernel_size=3, padding=1),
            nn.BatchNorm2d(3),
            nn.ReLU(),
            nn.Flatten(),
            nn.Linear(3 * self.board_size * self.board_size, 1),
            nn.Tanh()
        )
        self.to(self.device)

    def forward(self, x):
        x = self.startBlock(x)
        for resBlock in self.backBone:
            x = resBlock(x)
        policy = self.policyHead(x)
        value = self.valueHead(x)
        return policy, value
    
    def predict(self, state):
        # 將狀態轉換為 3 channel: [當前玩家, 對手玩家, 1]
        board_tensor = torch.tensor(state, dtype=torch.float32).to(self.device)
        player_board = (board_tensor == 1).float()
        opponent_board = (board_tensor == -1).float()
        color_board = torch.ones_like(board_tensor)
        
        x = torch.stack([player_board, opponent_board, color_board]).unsqueeze(0)
        self.eval()
        with torch.no_grad():
            pi, v = self.forward(x)
            pi = torch.softmax(pi, dim=1).squeeze(0).cpu().numpy()
            v = v.squeeze(0).cpu().item()
        return pi, v

# ----------------- 3. 蒙地卡羅樹搜尋 (MCTS) -----------------
class Node:
    def __init__(self, parent=None, prior_prob=0.0):
        self.parent = parent
        self.children = {} # action: Node
        self.visit_count = 0
        self.value_sum = 0
        self.prior_prob = prior_prob
        self.state = None

    def is_expanded(self):
        return len(self.children) > 0

    def q_value(self):
        if self.visit_count == 0: return 0
        # 由於子節點是從對手視角看，對手的價值要乘 -1 才是我的價值
        return - (self.value_sum / self.visit_count) 

class MCTS:
    def __init__(self, game, model, num_simulations=100, c_puct=1.5):
        self.game = game
        self.model = model
        self.num_simulations = num_simulations
        self.c_puct = c_puct

    def search(self, state):
        root = Node()
        root.state = state
        
        # Expand root
        policy, _ = self.model.predict(state)
        valid_moves = self.game.get_valid_moves(state)
        policy = policy * valid_moves
        policy /= np.sum(policy)
        
        for action, prob in enumerate(policy):
            if prob > 0:
                root.children[action] = Node(parent=root, prior_prob=prob)

        # 模擬 (Simulations)
        for _ in range(self.num_simulations):
            node = root
            curr_state = np.copy(state)
            
            # 1. Select (PUCT)
            while node.is_expanded():
                best_action = -1
                best_ucb = -float("inf")
                
                for action, child in node.children.items():
                    u = self.c_puct * child.prior_prob * math.sqrt(node.visit_count) / (1 + child.visit_count)
                    ucb = child.q_value() + u
                    if ucb > best_ucb:
                        best_ucb = ucb
                        best_action = action
                
                curr_state = self.game.get_next_state(curr_state, best_action)
                node = node.children[best_action]
                
            # Check terminal
            reward, is_terminal = self.game.get_reward_and_terminated(curr_state, best_action)
            
            # 2. Expand & Evaluate
            if not is_terminal:
                policy, value = self.model.predict(curr_state)
                valid_moves = self.game.get_valid_moves(curr_state)
                policy = policy * valid_moves
                policy /= np.sum(policy)
                
                for action, prob in enumerate(policy):
                    if prob > 0:
                        node.children[action] = Node(parent=node, prior_prob=prob)
            else:
                value = reward

            # 3. Backpropagate
            while node is not None:
                node.visit_count += 1
                node.value_sum += value
                node = node.parent
                value = -value # 切換玩家視角

        # 計算 MCTS 策略分佈 (根據訪問次數)
        action_probs = np.zeros(self.game.board_size ** 2)
        for action, child in root.children.items():
            action_probs[action] = child.visit_count
            
        action_probs /= np.sum(action_probs)
        return action_probs

# ----------------- 4. 訓練迴圈 (Trainer) -----------------
class AlphaZeroTrainer:
    def __init__(self, game, model):
        self.game = game
        self.model = model
        self.optimizer = optim.Adam(self.model.parameters(), lr=0.001, weight_decay=1e-4)
        self.mcts = MCTS(game, model, num_simulations=100)
        self.memory = deque(maxlen=10000)

    def self_play(self):
        state = self.game.get_initial_state()
        history = []
        player_turn = 1
        
        while True:
            # 獲取 MCTS 走法機率
            pi = self.mcts.search(state)
            
            # 儲存 (狀態, MCTS機率, 當前玩家)，方便後續指派輸贏
            history.append((np.copy(state), np.copy(pi), player_turn))
            
            # 根據機率採樣一個動作 (探索)
            action = np.random.choice(len(pi), p=pi)
            
            state = self.game.get_next_state(state, action)
            player_turn *= -1
            
            reward, is_terminal = self.game.get_reward_and_terminated(state, action)
            if is_terminal:
                # 遊戲結束，根據輸贏指派 value 給歷史紀錄
                for hist_state, hist_pi, hist_player in history:
                    # 如果當前勝利者等於歷史步驟的玩家，value = 1，否則 = -1 (平手則皆為 0)
                    v = reward if hist_player == player_turn * -1 else -reward
                    self.memory.append((hist_state, hist_pi, v))
                return

    def train_network(self, batch_size=64):
        if len(self.memory) < batch_size: return
        
        batch = random.sample(self.memory, batch_size)
        states, target_pis, target_vs = zip(*batch)
        
        # 準備資料
        states_tensor = []
        for state in states:
            player_board = (state == 1).astype(np.float32)
            opp_board = (state == -1).astype(np.float32)
            color_board = np.ones_like(state, dtype=np.float32)
            states_tensor.append(np.stack([player_board, opp_board, color_board]))
            
        states_tensor = torch.tensor(np.array(states_tensor)).to(self.model.device)
        target_pis = torch.tensor(np.array(target_pis), dtype=torch.float32).to(self.model.device)
        target_vs = torch.tensor(np.array(target_vs), dtype=torch.float32).unsqueeze(1).to(self.model.device)
        
        # 預測
        out_pi, out_v = self.model(states_tensor)
        
        # 損失函數 Loss = (v - target_v)^2 - target_pi * log(out_pi)
        loss_v = F.mse_loss(out_v, target_vs)
        loss_pi = -torch.sum(target_pis * F.log_softmax(out_pi, dim=1)) / batch_size
        total_loss = loss_v + loss_pi
        
        # 優化
        self.optimizer.zero_grad()
        total_loss.backward()
        self.optimizer.step()
        
        return total_loss.item()

if __name__ == "__main__":
    game = GomokuGame(board_size=8, n_in_row=5)
    model = AlphaZeroNet(game)
    trainer = AlphaZeroTrainer(game, model)
    
    epochs = 10
    episodes = 20 # 每個 epoch 玩幾局
    
    print(f"開始 AlphaZero 自我學習 (設備: {model.device})...")
    
    for epoch in range(epochs):
        print(f"Epoch {epoch+1}/{epochs} - 自我對弈中...")
        # 1. 產生自我對弈資料
        for _ in range(episodes):
            trainer.self_play()
            
        # 2. 根據這批資料訓練神經網路
        print(f"Epoch {epoch+1}/{epochs} - 網路訓練中...")
        loss = 0
        for _ in range(40): # 從記憶庫採樣訓練 40 次
            loss = trainer.train_network()
            
        print(f"Epoch {epoch+1}/{epochs} 完成, Loss: {loss:.4f}")
        
        # 儲存模型
        torch.save(model.state_dict(), "alphazero_gomoku.pth")
    
    print("訓練完成，模型已儲存為 alphazero_gomoku.pth")