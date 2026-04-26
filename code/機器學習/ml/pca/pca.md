# Principal Component Analysis (PCA)

## 原理

PCA 是一種常用的降維方法，透過找到數據中方差最大的方向（主成分）來進行降維。它將高維數據投影到較低維的空間，同時盡可能保留最多的信息。

### 算法步驟

1. 數據中心化：減去均值
2. 計算共變異矩陣
3. 求共變異矩陣的特徵值和特徵向量
4. 選擇前 K 個最大的特徵值對應的特徵向量
5. 將數據投影到這些特徵向量構成的空間

### 數學公式

**共變異矩陣**:
$$\Sigma = \frac{1}{n-1} \sum_{i=1}^{n}(x_i - \bar{x})(x_i - \bar{x})^T$$

**特徵值分解**:
$$\Sigma v = \lambda v$$

**投影**:
$$y = V^T x$$

其中 $V$ 是選擇的特徵向量矩陣。

**變異解釋比例**:
$$\frac{\lambda_i}{\sum_j \lambda_j}$$

## Python 實作對照

### 共變異矩陣

```python
def covariance_matrix(data):
    x_mean = mean(xs)
    y_mean = mean(ys)
    var_xx = sum((x - x_mean) ** 2 for x in xs) / (n - 1)
    var_yy = sum((y - y_mean) ** 2 for y in ys) / (n - 1)
    var_xy = sum((xs[i] - x_mean) * (ys[i] - y_mean) for i in range(n)) / (n - 1)
    return [[var_xx, var_xy], [var_xy, var_yy]]
```

計算共變異矩陣 $\Sigma$。

### 特徵值和特徵向量

```python
def eigenvalue(matrix):
    a, b, c, d = matrix[0][0], matrix[0][1], matrix[1][0], matrix[1][1]
    trace = a + d
    det = a * d - b * c
    l1 = trace / 2 + math.sqrt(trace * trace / 4 - det)
    l2 = trace / 2 - math.sqrt(trace / 2 * trace / 2 - det)
    return l1, l2
```

使用特徵方程 $\lambda^2 - trace \cdot \lambda + det = 0$ 求特徵值。

### 變異解釋

```python
variance_explained = ev1 / (ev1 + ev2) * 100
```

計算第一主成分解釋的變異比例。

## 使用場景

- 降維
- 特徵壓縮
- 可視化
- 去相關

## 優缺點

| 優點 | 缺點 |
|------|------|
| 去除冗餘特徵 | 只能處理線性關係 |
| 壓縮數據 | 需要標準化 |
| 可視化高維數據 | 難以解釋主成分 |