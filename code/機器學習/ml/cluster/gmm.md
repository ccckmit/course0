# Gaussian Mixture Model (GMM)

## 原理

高斯混合模型是一種概率模型，假设数据由多个高斯分布混合而成。它是一种软聚类方法，每个点属于每个簇的概率由后验概率决定。

### 算法步驟

1. 初始化 K 个高斯分布的参数（均值、方差、先验权重）
2. E步：计算每个点属于每个簇的后验概率
3. M步：更新参数使期望最大化
4. 重复 E-M 步骤直到收敛

### 數學公式

**多元高斯分布**:
$$P(x | \mu, \Sigma) = \frac{1}{(2\pi)^{d/2}|Sigma|^{1/2}} e^{-\frac{1}{2}(x-\mu)^T \Sigma^{-1}(x-\mu)}$$

**后验概率**:
$$P(k | x) = \frac{P(x | k) P(k)}{\sum_j P(x | j) P(j)}$$

**M步更新**:
$$\mu_k = \frac{\sum_i P(k | x_i) x_i}{\sum_i P(k | x_i)}$$

$$\Sigma_k = \frac{\sum_i P(k | x_i) (x_i - \mu_k)(x_i - \mu_k)^T}{\sum_i P(k | x_i)}$$

$$weight_k = \frac{\sum_i P(k | x_i)}{N}$$

## Python 實作對照

### 高斯概率密度

```python
def gaussian(self, x, mean, cov):
    det = cov[0][0] * cov[1][1] - cov[0][1] * cov[1][0]
    inv = [[cov[1][1] / det, -cov[0][1] / det],
           [-cov[1][0] / det, cov[0][0] / det]]
    dx = x[0] - mean[0]
    dy = x[1] - mean[1]
    z = dx * (inv[0][0] * dx + inv[0][1] * dy) + dy * (inv[1][0] * dx + inv[1][1] * dy)
    return math.exp(-0.5 * z) / (2 * math.pi * math.sqrt(det))
```

計算多元高斯分佈的概率密度函數。

### E步

```python
for x in data:
    probs = [self.weights[k] * self.gaussian(x, self.means[k], self.covars[k])
            for k in range(self.n_clusters)]
    total = sum(probs)
    responsibilities.append([p / total for p in probs])
```

計算每個點對每個簇的後驗概率。

### M步

```python
for k in range(self.n_clusters):
    Nk = sum(r[k] for r in responsibilities)
    self.weights[k] = Nk / n
    new_mean = [sum(r[i][k] * x[i] for i in range(n)) / Nk for dim in range(2)]
    self.means[k] = new_mean
    # 更新方差
```

根據後驗概率更新參數（均值、方差、權重）。

## 使用場景

- 橢圓形簇
- 軟聚類
- 概率輸出

## 優缺點

| 優點 | 缺點 |
|------|------|
| 發現橢圓形簇 | 需要多次運行 |
| 軟聚類輸出 | 計算量大 |
| 可以估計概率 | 可能收斂到局部最優 |