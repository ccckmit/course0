import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
import numpy as np
import math
import random

# ==========================================
# 1. 遊戲環境 (Tic-Tac-Toe)
# ==========================================
class TicTacToe:
    def __init__(self):
        self.row_count = 3
        self.column_count = 3
        self.action_size = self.row_count * self.column_count

    def get_initial_state(self):
        return np.zeros((self.row_count, self.column_count))

    def get_next_state(self, state, action, player):
        row = action // self.column_count
        column = action % self.column_count
        new_state = np.copy(state)
        new_state[row, column] = player
        return new_state

    def get_valid_moves(self, state):
        return (state.reshape(-1) == 0).astype(np.uint8)

    def check_win(self, state, action):
        if action is None:
            return False
        row = action // self.column_count
        column = action % self.column_count
        player = state[row, column]
        # 檢查行、列、對角線
        if np.sum(state[row, :]) == player * self.column_count: return True
        if np.sum(state[:, column]) == player * self.row_count: return True
        if np.sum(np.diag(state)) == player * self.row_count: return True
        if np.sum(np.diag(np.flip(state, axis=1))) == player * self.row_count: return True
        return False

    def get_value_and_terminated(self, state, action):
        if self.check_win(state, action):
            return 1, True
        if np.sum(self.get_valid_moves(state)) == 0:
            return 0, True # 平手
        return 0, False

    def get_canonical_board(self, state, player):
        # 讓神經網路始終以「當前玩家」的視角來看棋盤 (自己的棋子是 1，對手是 -1)
        return state * player

# ==========================================
# 2. 神經網路 (微型 CNN)
# ==========================================
class ResNet(nn.Module):
    def __init__(self, game, num_channels=64):
        super().__init__()
        self.conv1 = nn.Conv2d(1, num_channels, 3, padding=1)
        self.conv2 = nn.Conv2d(num_channels, num_channels, 3, padding=1)
        
        # Policy Head (輸出每個合法步的機率)
        self.policy_head = nn.Linear(num_channels * 3 * 3, game.action_size)
        # Value Head (輸出當前局面的勝率預測 -1 ~ 1)
        self.value_head = nn.Linear(num_channels * 3 * 3, 1)

    def forward(self, x):
        x = x.view(-1, 1, 3, 3) # Reshape 確保輸入為 (batch, channels, height, width)
        x = F.relu(self.conv1(x))
        x = F.relu(self.conv2(x))
        x = x.view(x.size(0), -1) # 攤平
        
        policy = F.softmax(self.policy_head(x), dim=1)
        value = torch.tanh(self.value_head(x))
        return policy, value

# ==========================================
# 3. 蒙地卡羅樹搜尋 (MCTS)
# ==========================================
class Node:
    def __init__(self, game, args, state, parent=None, action_taken=None, prior=0):
        self.game = game
        self.args = args
        self.state = state
        self.parent = parent
        self.action_taken = action_taken
        self.children = []
        
        self.visit_count = 0
        self.value_sum = 0
        self.prior = prior

    def is_fully_expanded(self):
        return len(self.children) > 0

    def select(self):
        # 根據 PUCT 公式選擇最佳子節點
        best_child = None
        best_ucb = -np.inf
        for child in self.children:
            ucb = self.get_ucb(child)
            if ucb > best_ucb:
                best_child = child
                best_ucb = ucb
        return best_child

    def get_ucb(self, child):
        if child.visit_count == 0:
            q_value = 0
        else:
            # 將子節點的價值反轉，因為子節點的行動是由對手進行的
            q_value = 1 - ((child.value_sum / child.visit_count) + 1) / 2
        return q_value + self.args['c_puct'] * (math.sqrt(self.visit_count) / (child.visit_count + 1)) * child.prior

    def expand(self, policy):
        valid_moves = self.game.get_valid_moves(self.state)
        for action, prob in enumerate(policy):
            if valid_moves[action] > 0:
                child_state = self.game.get_next_state(self.state, action, 1) # MCTS 內部始終以玩家 1 視角模擬
                child_state = self.game.get_canonical_board(child_state, -1)  # 轉換視角給下一步
                child = Node(self.game, self.args, child_state, self, action, prob)
                self.children.append(child)

    def backpropagate(self, value):
        self.value_sum += value
        self.visit_count += 1
        if self.parent is not None:
            # 傳遞給父節點時，價值需要反轉 (零和遊戲)
            self.parent.backpropagate(-value)

class MCTS:
    def __init__(self, game, args, model):
        self.game = game
        self.args = args
        self.model = model

    @torch.no_grad()
    def search(self, state):
        root = Node(self.game, self.args, state)
        
        for _ in range(self.args['num_mcts_sims']):
            node = root
            # 1. Select
            while node.is_fully_expanded():
                node = node.select()
                
            value, is_terminal = self.game.get_value_and_terminated(node.state, node.action_taken)
            
            # ❌ 刪除這行錯誤程式碼：
            # value = self.game.get_canonical_board(node.state, -1) 
            
            # ✅ 新增這段終局邏輯：
            if is_terminal:
                # 如果遊戲結束，代表「上一步」的玩家下出了致勝步 (value=1)。
                # 對於「當前」這個節點 (準備要下棋卻發現輸了的玩家) 來說，價值應該是 -1
                value = -value 
            else:
                # 2. Evaluate & Expand
                state_tensor = torch.tensor(node.state, dtype=torch.float32).unsqueeze(0)
                policy, value_tensor = self.model(state_tensor)
                policy = policy.squeeze(0).numpy()
                
                # 從網路預測結果取出純量數值
                value = value_tensor.item() 
                
                valid_moves = self.game.get_valid_moves(node.state)
                policy *= valid_moves # 遮罩非法步
                policy /= np.sum(policy)
                
                node.expand(policy)
                
            # 3. Backpropagate
            node.backpropagate(value)

        # 收集 MCTS 算出來的政策機率
        action_probs = np.zeros(self.game.action_size)
        for child in root.children:
            action_probs[child.action_taken] = child.visit_count
        action_probs /= np.sum(action_probs)
        return action_probs

# ==========================================
# 4. 訓練迴圈 (Self-Play & Train)
# ==========================================
def self_play(game, args, model):
    memory = []
    player = 1
    state = game.get_initial_state()
    mcts = MCTS(game, args, model)
    
    while True:
        canonical_state = game.get_canonical_board(state, player)
        action_probs = mcts.search(canonical_state)
        memory.append((canonical_state, action_probs, player))
        
        # 加上溫度參數進行探索
        action = np.random.choice(game.action_size, p=action_probs)
        state = game.get_next_state(state, action, player)
        
        value, is_terminal = game.get_value_and_terminated(state, action)
        if is_terminal:
            returnMemory = []
            for hist_state, hist_probs, hist_player in memory:
                # 若平手 value 為 0，否則根據當前玩家是否為贏家給予 1 或 -1
                hist_outcome = value if hist_player == player else -value
                returnMemory.append((hist_state, hist_probs, hist_outcome))
            return returnMemory
        
        player *= -1

def train(model, optimizer, memory):
    model.train()
    random.shuffle(memory)
    
    # 批次處理資料
    states = torch.tensor(np.array([m[0] for m in memory]), dtype=torch.float32)
    policies = torch.tensor(np.array([m[1] for m in memory]), dtype=torch.float32)
    values = torch.tensor(np.array([m[2] for m in memory]), dtype=torch.float32).unsqueeze(1)
    
    out_policies, out_values = model(states)
    
    # Policy Loss (Cross Entropy) + Value Loss (MSE)
    policy_loss = F.cross_entropy(out_policies, policies)
    value_loss = F.mse_loss(out_values, values)
    loss = policy_loss + value_loss
    
    optimizer.zero_grad()
    loss.backward()
    optimizer.step()
    
    return loss.item()

if __name__ == "__main__":
    game = TicTacToe()
    model = ResNet(game)
    optimizer = optim.Adam(model.parameters(), lr=0.001)
    
    args = {
        'num_iterations': 10,     # 大迴圈次數 (在井字遊戲中，10~20次即可看到成效)
        'num_self_play': 50,      # 每次迭代進行幾場自我對弈
        'num_mcts_sims': 100,     # MCTS 每步搜尋次數
        'c_puct': 1.0             # 探索參數
    }
    
    print("開始訓練 AlphaZero 井字遊戲模型...")
    for iteration in range(args['num_iterations']):
        print(f"Iteration {iteration + 1}/{args['num_iterations']}")
        memory = []
        
        # 1. 自我對弈收集數據
        model.eval()
        for i in range(args['num_self_play']):
            memory += self_play(game, args, model)
            
        # 2. 訓練網路
        loss = train(model, optimizer, memory)
        print(f"  Loss: {loss:.4f}")
        
    torch.save(model.state_dict(), "model_tictactoe.pth")
    print("訓練完成！模型已儲存為 model_tictactoe.pth")