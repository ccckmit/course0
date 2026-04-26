# Linear Regression

## 原理

線性回歸是一種最基本的回歸算法，假設目標變量與特徵之間存在線性關係。它透過最小化預測值與真實值之間的均方誤差來找到最佳的線性組合參數。

### 算法步驟

1. 建立線性模型：$\hat{y} = w_0 + w_1 x_1 + w_2 x_2 + ...$
2. 定義損失函數：$MSE = \frac{1}{n}\sum(y_i - \hat{y}_i)^2$
3. 使用最小二乘法或梯度下降求解參數

### 數學公式

**線性模型**:
$$\hat{y} = Xw$$

**最小二乘法解**:
$$w = (X^T X)^{-1} X^T y$$

**均方誤差**:
$$MSE = \frac{1}{n} \sum_{i=1}^{n}(y_i - \hat{y}_i)^2$$

## Python 實作對照

### 均值計算

```python
def mean(values):
    return sum(values) / len(values)
```

計算平均值用於中心化數據。

### 最小二乘法

```python
def linear_regression(data):
    xs = [p[0] for p in data]
    ys = [p[1] for p in data]
    x_mean = mean(xs)
    y_mean = mean(ys)
    numerator = sum((xs[i] - x_mean) * (ys[i] - y_mean) for i in range(n))
    denominator = sum((xs[i] - x_mean) ** 2 for i in range(n))
    slope = numerator / denominator
    intercept = y_mean - slope * x_mean
    return slope, intercept
```

單變量線性回歸的閉式解：
$$w_1 = \frac{\sum(x_i - \bar{x})(y_i - \bar{y})}{\sum(x_i - \bar{x})^2}$$
$$w_0 = \bar{y} - w_1 \bar{x}$$

### 評估指標

```python
def mse(data, slope, intercept):
    return sum((y - predict(x, slope, intercept)) ** 2 for x, y in data) / len(data)
```

計算 MSE 評估模型效果。

```python
def r2_score(data, slope, intercept):
    ss_res = sum((y - predict(x, slope, intercept)) ** 2 for x, y in data)
    ss_tot = sum((y - y_mean) ** 2 for x, y in data)
    return 1 - ss_res / ss_tot
```

計算 R² 分數。

## 使用場景

- 預測連續值
- 分析變量關係
- 趨勢分析

## 優缺點

| 優點 | 缺點 |
|------|------|
| 簡單快速 | 只能處理線性關係 |
| 可解釋性強 | 對異常值敏感 |
| 閉式解精確 | 容易欠擬合 |