import gymnasium as gym
import torch
import torch.optim as optim
import torch.nn.functional as F
import numpy as np
from model import ActorCritic, RunningMeanStd

if torch.cuda.is_available():
    device = torch.device("cuda")
elif torch.backends.mps.is_available():
    device = torch.device("mps")
else:
    device = torch.device("cpu")

device = torch.device("cpu") # 沒用 GPU 反而比較快
print(f"使用 device: {device}")

def train():
    env = gym.make("Walker2d-v5")
    state_dim = env.observation_space.shape[0]
    action_dim = env.action_space.shape[0]
    
    policy = ActorCritic(state_dim, action_dim).to(device)
    optimizer = optim.Adam(policy.parameters(), lr=3e-4)
    # optimizer = optim.Adam(policy.parameters(), lr=1e-3)
    rms = RunningMeanStd(state_dim) # 狀態歸一化器
    
    print("開始訓練 (優化版)...")

    for i_update in range(1000): # 原為 2000 次更新，改為 1000 次以加快訓練
        buffer_s, buffer_a, buffer_lp, buffer_r, buffer_d = [], [], [], [], []
        state, _ = env.reset()
        
        # 1. 收集數據
        for t in range(2048):
            rms.update(state.reshape(1, -1))
            # 歸一化狀態：(s - mean) / std
            norm_s = (state - rms.mean) / np.sqrt(rms.var + 1e-8)
            
            s_ts = torch.FloatTensor(norm_s).unsqueeze(0).to(device)
            action, logprob = policy.get_action(s_ts)
            
            next_state, reward, term, trunc, _ = env.step(action.cpu().numpy().flatten())
            
            # 獎勵縮放：MuJoCo 獎勵通常很大，除以 10 會更穩定
            buffer_s.append(s_ts)
            buffer_a.append(action)
            buffer_lp.append(logprob)
            buffer_r.append(reward / 10.0) 
            buffer_d.append(term or trunc)
            
            state = next_state
            if term or trunc: state, _ = env.reset()

        # 2. 計算 Returns & Advantages
        s_batch = torch.cat(buffer_s)
        a_batch = torch.cat(buffer_a)
        lp_batch = torch.cat(buffer_lp).detach()
        
        with torch.no_grad():
            _, v_batch, _ = policy.evaluate(s_batch, a_batch)
            v_batch = v_batch.squeeze()
            
            returns, advs = [], []
            gae = 0
            for r, d in zip(reversed(buffer_r), reversed(buffer_d)):
                if d: gae = 0
                # 這裡使用簡化版 Advantage
                delta = r + (0.99 * gae * (1-d)) # 這裡簡化處理
                returns.insert(0, r) # 實際上應算 Discounted Return
            
            # 重新計算正確的 Returns
            ret = 0
            returns = []
            for r, d in zip(reversed(buffer_r), reversed(buffer_d)):
                if d: ret = 0
                ret = r + 0.99 * ret
                returns.insert(0, ret)
            
            returns = torch.FloatTensor(returns).to(device)
            advantages = (returns - v_batch)
            advantages = (advantages - advantages.mean()) / (advantages.std() + 1e-8)

        # 3. PPO 更新
        for _ in range(10):
            new_lp, v_s, entropy = policy.evaluate(s_batch, a_batch)
            ratio = torch.exp(new_lp - lp_batch)
            surr1 = ratio * advantages
            surr2 = torch.clamp(ratio, 0.8, 1.2) * advantages
            
            loss = -torch.min(surr1, surr2) + 0.5 * F.mse_loss(v_s.squeeze(), returns) - 0.01 * entropy
            optimizer.zero_grad()
            loss.mean().backward()
            optimizer.step()

        if i_update % 20 == 0:
            print(f"Update {i_update} | Mean Reward: {np.mean(buffer_r)*10:.2f}")
            # 儲存模型與歸一化參數
            torch.save({
                'model': policy.state_dict(),
                'rms_mean': rms.mean,
                'rms_var': rms.var
            }, "ppo_walker_diy.pth")

if __name__ == "__main__":
    train()