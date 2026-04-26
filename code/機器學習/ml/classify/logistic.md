# Logistic Regression

## 原理

邏輯回歸是一種用於二分類的線性模型，雖然名字中有「回歸」，但實際上是分類算法。它使用 Sigmoid 函數將線性組合映射到 [0, 1] 區間，表示屬於正類的概率。

### 算法步驟

1. 計算輸入的線性組合：$z = w_0 + w_1 x_1 + w_2 x_2 + ...$
2. 通過 Sigmoid 函數計算概率：$p = \sigma(z) = \frac{1}{1 + e^{-z}}$
3. 根據閾值（通常 0.5）決定類別

### 數學公式

**Sigmoid 函數**:
$$\sigma(z) = \frac{1}{1 + e^{-z}}$$

**交叉熵損失**:
$$L = -\sum_{i}[y_i \log(\hat{y}_i) + (1-y_i)\log(1-\hat{y}_i)]$$

**梯度下降更新**:
$$w_j := w_j - \alpha \cdot \frac{\partial L}{\partial w_j}$$

## Python 實作對照

### Sigmoid 函數

```python
def sigmoid(z):
    return 1 / (1 + math.exp(-z))
```

對應公式中的 $\sigma(z)$。

### 預測概率

```python
def predict_prob(x, y, weights):
    z = weights[0] + weights[1] * x + weights[2] * y
    return sigmoid(z)
```

對應步驟 1-2：計算線性組合後通過 Sigmoid。

### 訓練過程

```python
def train_logistic(data, lr=0.1, epochs=1000):
    weights = [0, 0, 0]
    for _ in range(epochs):
        for x, y, label in data:
            prob = predict_prob(x, y, weights)
            error = label - prob
            weights[0] += lr * error
            weights[1] += lr * error * x
            weights[2] += lr * error * y
    return weights
```

使用梯度下降更新權重：$w_j := w_j + \alpha \cdot error \cdot x_j$

## 使用場景

- 二分類問題
- 需要概率輸出
- 線性可分的數據

## 優缺點

| 優點 | 缺點 |
|------|------|
| 輸出概率 | 只能處理線性可分 |
| 訓練速度快 | 對異常值敏感 |
| 可解釋性強 | 容易欠擬合 |