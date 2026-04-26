# Gym

## 概述

Gym 是 OpenAI 開發的強化學習 (Reinforcement Learning, RL) 環境工具包，提供標準化的介面用於開發和比較 RL 演算法。

## 基本資訊

| 項目 | 內容 |
|------|------|
| 開發者 | OpenAI |
| 類型 | 強化學習環境 |
| GitHub | https://github.com/openai/gym |
| Python | 3.8+ |
| 官方網站 | https://www.gymlibrary.dev |

## 安裝

```bash
pip install gym

# 安裝完整環境（包含所有依賴）
pip install "gym[all]"

# 安裝特定環境
pip install "gym[classic_control]"
pip install "gym[box2d]"
pip install "gym[mujoco]"
```

## 核心 API

### 環境互動

```python
import gym

# 建立環境
env = gym.make('CartPole-v1')

# 重置環境，獲得初始觀察
observation, info = env.reset()

# 執行一個時間步
for _ in range(1000):
    # 渲染環境
    env.render()
    
    # 採樣動作（隨機）
    action = env.action_space.sample()
    
    # 執行動作
    observation, reward, terminated, truncated, info = env.step(action)
    
    # 檢查是否結束
    if terminated or truncated:
        observation, info = env.reset()

env.close()
```

### 環境結構

```
┌─────────────────────────────────────────────────────────────┐
│                      Gym 環境                               │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐    action     ┌─────────────────────┐    │
│  │   Agent     │ ────────────→ │                   │    │
│  │  (Policy)   │               │      Environment   │    │
│  └─────────────┘ ←──────────── │                   │    │
│        ↑          observation    │                   │    │
│        │              ↑        │                   │    │
│        └──────────────┘        └─────────────────────┘    │
│           reward (scalar)                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 回饋格式

```python
observation, reward, terminated, truncated, info = env.step(action)

# observation: 觀察空間（numpy array）
# reward: 獎勵值（float）
# terminated: 是否自然結束（bool）
# truncated: 是否因時間限制而結束（bool）
# info: 額外資訊（dict）
```

## 空間類型

### 離散空間 (Discrete)

```python
# 離散動作空間：0, 1, 2, ...
action_space = env.action_space
print(action_space)          # Discrete(2)
print(action_space.n)        # 動作數量：2

# 採樣
action = action_space.sample()  # 隨機選擇一個動作
```

### 盒子空間 (Box)

```python
# 連續觀察/動作空間
observation_space = env.observation_space
print(observation_space)  # Box(-3.4e+38, 3.4e+38, (4,), float32)

low = observation_space.low   # 觀察下限
high = observation_space.high # 觀察上限
shape = observation_space.shape  # 形狀：(4,)
```

### 其他空間

```python
from gym import spaces

# 多離散空間
multi_discrete = spaces.MultiDiscrete([5, 2, 2])

# 多二元空間
multi_binary = spaces.MultiBinary(3)

# 字典空間
dict_space = spaces.Dict({
    "position": spaces.Box(low=-1, high=1, shape=(2,)),
    "velocity": spaces.Box(low=-1, high=1, shape=(2,)),
})
```

## 經典控制環境

### CartPole

```python
env = gym.make('CartPole-v1')

# 觀察空間：(位置, 速度, 角度, 角速度)
# 動作空間：0(左), 1(右)
# 目標：保持桿子平衡 >= 195 步
```

### MountainCar

```python
env = gym.make('MountainCar-v0')

# 觀察空間：2D (位置, 速度)
# 動作空間：離散 3（倒車、空檔、加速）
# 目標：到達山頂
```

### Pendulum

```python
env = gym.make('Pendulum-v1')

# 觀察空間：(cos θ, sin θ, ω)
# 動作空間：連續扭矩 (-2, 2)
# 目標：將鐘擺直立並保持
```

## 常用環境列表

| 環境 | 說明 | 動作空間 |
|------|------|----------|
| CartPole-v1 | 平衡桿 | 離散 2 |
| MountainCar-v0 | 爬上山 | 離散 3 |
| Pendulum-v1 | 鐘擺控制 | 連續 1 |
| Acrobot-v1 | 雙連桿 | 離散 3 |
| LunarLander-v2 | 月球登陸 | 離散 4 |
| BipedalWalker-v3 | 雙足行走 | 連續 4 |

## 包裝器 (Wrappers)

### 動作/觀察正規化

```python
from gym.wrappers import NormalizeObservation, NormalizeReward
from gym.wrappers import TimeLimit

env = gym.make('CartPole-v1')
env = TimeLimit(env, max_episode_steps=500)
env = NormalizeObservation(env)
env = NormalizeReward(env)
```

### 幀疊加 (Frame Stacking)

```python
from gym.wrappers import FrameStack

env = gym.make('ALE/Pong-v5')
env = FrameStack(env, num_stack=4)  # 堆疊最近 4 幀
```

### 視覺化包裝

```python
from gym.wrappers import RecordVideo

env = gym.make('CartPole-v1')
env = RecordVideo(env, video_folder='./videos')
```

## 範例：DQN 訓練

```python
import gym
import numpy as np
import random

env = gym.make('CartPole-v1')
memory = []
learning_rate = 0.001
gamma = 0.95

def choose_action(state, epsilon):
    if random.random() < epsilon:
        return env.action_space.sample()
    else:
        # 這裡應該是訓練好的 Q-network
        return 0 if state[2] < 0 else 1

for episode in range(500):
    state, _ = env.reset()
    total_reward = 0
    
    while True:
        action = choose_action(state, epsilon=0.1)
        next_state, reward, terminated, truncated, _ = env.step(action)
        
        memory.append((state, action, reward, next_state, terminated))
        
        if len(memory) > 32:
            # 從記憶中採樣學習
            batch = random.sample(memory, 32)
            # 訓練 Q-network ...
        
        state = next_state
        total_reward += reward
        
        if terminated or truncated:
            print(f"Episode {episode}: {total_reward}")
            break
```

## Stable-Baselines3 整合

```python
from stable_baselines3 import DQN, PPO

# DQN 演算法
model = DQN("MlpPolicy", "CartPole-v1", verbose=1)
model.learn(total_timesteps=10000)

# 評估
obs, _ = env.reset()
for _ in range(1000):
    action, _ = model.predict(obs)
    obs, _, done, _, _ = env.step(action)
    if done:
        obs, _ = env.reset()

env.close()
```

## 環境註冊

```python
from gym.envs.registration import register

# 註冊自定義環境
register(
    id='MyEnv-v0',
    entry_point='my_env:MyEnv',
    max_episode_steps=1000,
)

# 使用自定義環境
env = gym.make('MyEnv-v0')
```

## 與 Gymnasium 的關係

> Gym 已於 2023 年更名為 **Gymnasium**，但 API 保持相容。

```python
# 新版本
import gymnasium as gym

# 舊版本（仍可使用）
import gym
```

## 相關資源

- GitHub：https://github.com/openai/gym
- 文件：https://www.gymlibrary.dev
- 相關概念：[強化學習](強化學習.md)
- 相關工具：[PyTorch](PyTorch.md)

## Tags

#強化學習 #Gym #OpenAI #環境 #RL
