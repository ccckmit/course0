# Support Vector Machine (SVM)

## 原理

SVM 是一種強大的分類算法，核心思想是找到一個最大間隔的超平面來分隔不同類別的數據。這個超平面稱為最大間隔分類器，支持向量是離超平面最近的數據點。

### 算法步驟

1. 寻找能够分隔两类的超平面 $w \cdot x + b = 0$
2. 最大化间隔：$margin = \frac{2}{||w||}$
3. 求解优化问题：$\min_{w,b} \frac{||w||^2}{2}$ s.t. $y_i(w \cdot x_i + b) \geq 1$

### 數學公式

**决策函数**:
$$f(x) = w \cdot x + b$$

**间隔**:
$$\gamma = \frac{y_i(w \cdot x_i + b)}{||w||}$$

**合页损失**:
$$L = \frac{||w||^2}{2} + C \sum_i \max(0, 1 - y_i(w \cdot x_i + b))$$

## Python 實作對照

### 預測

```python
def predict_svm(x, y, w, b):
    return w[0] * x + w[1] * y + b
```

决策函数：$w \cdot x + b$

### 訓練（Hinge Loss）

```python
def train_svm(data, lr=0.01, epochs=1000, C=1.0):
    w = [0, 0]
    b = 0
    for _ in range(epochs):
        for x, y, label in data:
            val = label * predict_svm(x, y, w, b)
            if val >= 1:
                w[0] -= lr * w[0]
                w[1] -= lr * w[1]
            else:
                w[0] -= lr * (w[0] - C * label * x)
                w[1] -= lr * (w[1] - C * label * y)
                b -= lr * (-C * label)
    return w, b
```

實現合页损失：
- 若 $y_i(w \cdot x_i + b) \geq 1$：只更新正則化項
- 若 $y_i(w \cdot x_i + b) < 1$：同時更新間隔約束

## 使用場景

- 高維數據
- 文本分類
- 圖像分類

## 優缺點

| 優點 | 缺點 |
|------|------|
| 高維效果 好 | 對大規模數據慢 |
| 内存高效 | 對噪聲敏感 |
| 泛化能力强 | 需要核函数處理非线性 |