# Adam (Adaptive Moment Estimation)

## 概述

Adam 是深度學習中最常用的優化器之一，結合了動量（Momentum）和 RMSProp 的優點，透過自適應學習率加速收斂。

## 演算法

### 核心公式

```python
# Adam 演算法
# 輸入：學習率 α、β₁=0.9、β₂=0.999、ε=1e-8、初始參數 θ

# 時間步 t
t = 0

# 初始化
m = 0  # 第一動量（均值估計）
v = 0  # 第二動量（非中心化方差估計）

while not converged:
    t += 1
    
    # 計算梯度
    g = gradient(θ)
    
    # 更新第一動量（類似動量）
    m = β₁ * m + (1 - β₁) * g
    
    # 更新第二動量（類似 RMSProp）
    v = β₂ * v + (1 - β₂) * (g ** 2)
    
    # 偏差校正（Bias Correction）
    m_hat = m / (1 - β₁ ** t)
    v_hat = v / (1 - β₂ ** t)
    
    # 更新參數
    θ = θ - α * m_hat / (sqrt(v_hat) + ε)
```

### 與其他優化器比較

```
普通 SGD                    帶動量的 SGD              Adam
    ↓                          ↓                        ↓
   ╱╲                        ╱╲                       ╱╲
  ╱  ╲                      ╱──╲                     ╱──╲
 ╱    ╲                    ╱    ╲                   ╱    ╲
╱      ╲                  ╱      ╲                 ╱      ╲
                                                                      ↓
普通收斂                 加速收斂                  自適應學習率收斂
```

## PyTorch 實現

```python
import torch
import torch.nn as nn

model = nn.Sequential(
    nn.Linear(784, 256),
    nn.ReLU(),
    nn.Linear(256, 10)
)

optimizer = torch.optim.Adam(
    model.parameters(),
    lr=0.001,
    betas=(0.9, 0.999),
    eps=1e-8,
    weight_decay=0  # L2 正則化
)

criterion = nn.CrossEntropyLoss()

for epoch in range(num_epochs):
    for batch_x, batch_y in dataloader:
        optimizer.zero_grad()      # 清除梯度
        output = model(batch_x)     # 前饋
        loss = criterion(output, batch_y)
        loss.backward()             # 反向傳播
        optimizer.step()            # 更新參數
```

## Adam 的組成

### 1. 動量 (Momentum)
```python
# 動量：累積歷史梯度方向
m = β₁ * m + (1 - β₁) * g
```
- 類似物理學中的動量
- 幫助穿越震盪區域
- 加速收斂

### 2. 自適應學習率 (RMSProp)
```python
# RMSProp：根據梯度大小調整學習率
v = β₂ * v + (1 - β₂) * (g ** 2)
```
- 梯度大時，學習率變小
- 梯度小時，學習率變大
- 適合稀疏梯度

### 3. 偏差校正 (Bias Correction)
```python
# 問題：m, v 初始值為 0，會偏向 0
# 解決：除以 (1 - β^t) 進行校正
m_hat = m / (1 - β₁ ** t)
v_hat = v / (1 - β₂ ** t)
```

## 超參數設定

### 推薦值
| 參數 | 推薦值 | 說明 |
|------|--------|------|
| α | 3e-4 | 學習率（可調整） |
| β₁ | 0.9 | 第一動量衰減率 |
| β₂ | 0.999 | 第二動量衰減率 |
| ε | 1e-8 | 數值穩定性 |

### 學習率影響
```python
# 學習率過高
optimizer = torch.optim.Adam(model.parameters(), lr=1e-2)  # 可能震盪/發散

# 學習率過低
optimizer = torch.optim.Adam(model.parameters(), lr=1e-6)  # 收斂太慢

# 推薦範圍
optimizer = torch.optim.Adam(model.parameters(), lr=3e-4)  # 預設較好起點
```

## Adam vs 其他優化器

| 特性 | SGD | SGD+Momentum | RMSProp | Adam |
|------|-----|--------------|---------|------|
| 自適應學習率 | 否 | 否 | 是 | 是 |
| 動量 | 否 | 是 | 否 | 是 |
| 偏差校正 | N/A | N/A | 否 | 是 |
| 收斂速度 | 慢 | 中 | 快 | 快 |
| 泛化能力 | 好 | 好 | 一般 | 一般 |
| 記憶體 | 低 | 低 | 中 | 中 |

## AdamW (Adam with Weight Decay)

AdamW 是 Adam 的改進版本，將權重衰減與 Adam 更新分離：

```python
import torch.optim as optim

# 標準 Adam（權重衰減效果不佳）
optimizer = optim.Adam(model.parameters(), lr=1e-3, weight_decay=1e-4)

# AdamW（正確的權重衰減）
optimizer = optim.AdamW(model.parameters(), lr=1e-3, weight_decay=1e-4)
```

### 數學差異
```
標準 Adam + L2:  θ = θ - α(m̂/(√v̂ + ε)) - αλθ

AdamW:           θ = θ - α(m̂/(√v̂ + ε)) - αλθ
                                   ↑           ↑
                               Adam 更新    權重衰減
```

## Learning Rate Finder

使用 Adam 時找最佳學習率：
```python
# PyTorch Lightning
from lightning.pytorch.callbacks import LearningRateFinder

# 或手動實現
lrs = []
losses = []

for lr in np.logspace(-6, 0, 100):
    optimizer = torch.optim.Adam(model.parameters(), lr=lr)
    loss = train_step(model, data, optimizer)
    lrs.append(lr)
    losses.append(loss)

# 繪製 loss vs lr 曲線，找最佳點
```

## 進階變體

### AMSGrad
```python
# 確保收斂性
v_hat = max(v_hat, v_hat_prev)
θ = θ - α * m_hat / (sqrt(v_hat) + ε)
```

### AdaBound
```python
# 結合 Adam 和 SGD
alpha_t = min(alpha, alpha / (1 - beta2**t) + epsilon)
```

### RAdam (Rectified Adam)
```python
# 根據方差自動調整學習率
if beta2 == 1:
    rho = inf
else:
    rho_t = 2 / (1 - beta2) - 1
rho_inf = 2 / (1 - beta2) - 2

if t < rho_inf:
    rho_t = 1
    ratio = 1.0
elif t <= rho_inf - 4:
    rho_t = (rho_inf - 4) * (rho_inf - 2) * rho_inf / ((rho_inf - 4) * t + 2)
    ratio = sqrt((rho_t - 4) * (rho_t - 2) * rho_inf / ((rho_inf - 4) * rho_t))
else:
    ratio = 1.0

m_hat = m / (1 - beta1**t)
theta = theta - lr * ratio * m_hat / (sqrt(v) / (1 - beta2**t) + eps)
```

## 使用建議

1. **預設首選**：大多數情況 Adam 是好的起點
2. **調整學習率**：從 3e-4 開始，根據任務調整
3. **長期訓練**：考慮後期切換到 SGD 或使用學習率排程
4. **泛化任務**：嘗試使用 SGD+Momentum 或 AdamW

## 實驗：收斂比較

```python
import matplotlib.pyplot as plt

optimizers = {
    'SGD': torch.optim.SGD(model.parameters(), lr=0.1),
    'SGD+Momentum': torch.optim.SGD(model.parameters(), lr=0.1, momentum=0.9),
    'Adam': torch.optim.Adam(model.parameters(), lr=0.001),
}

for name, opt in optimizers.items():
    losses = train_model(model, opt, dataloader)
    plt.plot(losses, label=name)

plt.legend()
plt.xlabel('Epoch')
plt.ylabel('Loss')
plt.title('Optimizer Comparison')
```

## 相關資源

- 相關概念：[梯度下降法](梯度下降法.md)
- 相關概念：[反傳遞演算法](反傳遞演算法.md)
- 相關概念：[深度學習](深度學習.md)

## Tags

#Adam #優化器 #深度學習 #自適應學習率
