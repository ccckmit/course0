# https://zhiqingxiao.github.io/rl-book/en2024/code/CartPole-v0_VPG_torch.html
# VPG (Vanilla Policy Gradient / REINFORCE) 實作，用於 CartPole-v1
import sys
import logging
import itertools

import numpy as np
np.random.seed(0)
# import pandas as pd
import gymnasium as gym
import matplotlib.pyplot as plt
import torch
torch.manual_seed(0)
import torch.nn as nn
import torch.optim as optim
import torch.distributions as distributions

# 設定 logging 輸出格式
logging.basicConfig(level=logging.INFO,
                    format='%(asctime)s [%(levelname)s] %(message)s',
                    stream=sys.stdout, datefmt='%H:%M:%S')

# 建立 CartPole 環境並輸出環境資訊
env = gym.make('CartPole-v1')
for key in vars(env):
    logging.info('%s: %s', key, vars(env)[key])
for key in vars(env.spec):
    logging.info('%s: %s', key, vars(env.spec)[key])

# VPGAgent：使用策略梯度（REINFORCE）演算法的智能體
class VPGAgent:
    def __init__(self, env):
        self.action_n = env.action_space.n
        self.gamma = 0.99
        # 策略網路：輸入狀態，輸出各動作的機率（經 Softmax 歸一化）
        self.policy_net = self.build_net(
            input_size=env.observation_space.shape[0],
            hidden_sizes=[],
            output_size=self.action_n,
            output_activator=nn.Softmax(dim=1)
        )
        self.optimizer = optim.Adam(self.policy_net.parameters(), lr=0.005)

    def build_net(self, input_size, hidden_sizes, output_size, output_activator=None, use_bias=False):
        """建立多層感知器網路，可選最終激活函數（如 Softmax）"""
        layers = []
        for input_size, output_size in zip(
                [input_size] + hidden_sizes, hidden_sizes + [output_size]):
            layers.append(nn.Linear(input_size, output_size, bias=use_bias))
            layers.append(nn.ReLU())
        layers = layers[:-1]  # 移除最後一層 ReLU
        if output_activator:
            layers.append(output_activator)
        model = nn.Sequential(*layers)
        return model

    def reset(self, mode=None):
        """重置智能體狀態，訓練模式時初始化軌跡記錄"""
        self.mode = mode
        if self.mode == 'train':
            self.trajectory = []

    def step(self, observation, reward, terminated):
        """根據當前觀測選擇動作，訓練模式下記錄軌跡"""
        state_tensor = torch.as_tensor(observation, dtype=torch.float).unsqueeze(0)
        prob_tensor = self.policy_net(state_tensor)
        action_tensor = distributions.Categorical(prob_tensor).sample()
        action = action_tensor.item()
        if self.mode == 'train':
            self.trajectory += [observation, reward, terminated, action]
        return action

    def close(self):
        """回合結束時若為訓練模式則進行學習"""
        if self.mode == 'train':
            self.learn()

    def learn(self):
        """計算折扣回報 G_t，更新策略網路（REINFORCE 演算法核心）"""
        state_tensor = torch.as_tensor(self.trajectory[0::4], dtype=torch.float)
        reward_tensor = torch.as_tensor(self.trajectory[1::4], dtype=torch.float)
        action_tensor = torch.as_tensor(self.trajectory[3::4], dtype=torch.long)
        # 建立折扣因子序列 [γ^0, γ^1, ..., γ^{T-1}]
        arange_tensor = torch.arange(state_tensor.shape[0], dtype=torch.float)
        discount_tensor = self.gamma ** arange_tensor
        # 計算折扣回報：G_t = Σ_{k=t}^{T} γ^{k-t} r_k
        discounted_reward_tensor = discount_tensor * reward_tensor
        discounted_return_tensor = discounted_reward_tensor.flip(0).cumsum(0).flip(0)
        # 計算損失：-G_t log π(a_t|s_t)
        all_pi_tensor = self.policy_net(state_tensor)
        pi_tensor = torch.gather(all_pi_tensor, 1, action_tensor.unsqueeze(1)).squeeze(1)
        log_pi_tensor = torch.log(torch.clamp(pi_tensor, 1e-6, 1.))
        loss_tensor = -(discounted_return_tensor * log_pi_tensor).mean()
        self.optimizer.zero_grad()
        loss_tensor.backward()
        self.optimizer.step()

# 建立智能體實例
agent = VPGAgent(env)

def play_episode(env, agent, seed=None, mode=None, render=False):
    """在環境中執行一個完整回合，可選訓練模式或渲染模式"""
    observation, info = env.reset(seed=seed)
    reward, terminated, truncated = 0., False, False
    agent.reset(mode=mode)
    episode_reward, elapsed_steps = 0., 0
    while True:
        action = agent.step(observation, reward, terminated)
        if render:
            env.render()
        if terminated or truncated:
            break
        observation, reward, terminated, truncated, info = env.step(action)
        episode_reward += reward
        elapsed_steps += 1
    agent.close()
    return episode_reward, elapsed_steps

# ── 訓練階段 ──────────────────────────────
logging.info('==== train ====')
episode_rewards = []
for episode in itertools.count():
    episode_reward, elapsed_steps = play_episode(env, agent, seed=episode, mode='train')
    episode_rewards.append(episode_reward)
    logging.info('train episode %d: reward = %.2f, steps = %d',
                 episode, episode_reward, elapsed_steps)
    # 當最近 20 回合平均報酬超過 199 時停止訓練
    if np.mean(episode_rewards[-20:]) > 199:
        break

# 繪製訓練過程的報酬曲線
plt.plot(episode_rewards)
plt.xlabel('Episodes')
plt.ylabel('Episode Reward')
plt.title('Training Rewards')
plt.show()

# ── 測試階段 ──────────────────────────────
logging.info('==== test ====')
episode_rewards = []
for episode in range(100):
    episode_reward, elapsed_steps = play_episode(env, agent)
    episode_rewards.append(episode_reward)
    logging.info('test episode %d: reward = %.2f, steps = %d',
                 episode, episode_reward, elapsed_steps)

logging.info('average episode reward = %.2f ± %.2f',
             np.mean(episode_rewards), np.std(episode_rewards))

# ── 渲染展示 ──────────────────────────────
env = gym.make('CartPole-v1', render_mode="human")
episode_reward, elapsed_steps = play_episode(env, agent, render=True)
env.close()