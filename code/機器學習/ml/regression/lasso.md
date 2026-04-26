# Lasso Regression

## 原理

Lasso 是在线性回归的基础上添加 L1 正则化项的回归方法。L1 正则化具有稀疏性特性，可以将一些权重正好设为零，从而实现特征选择。

### 算法步驟

1. 建立带 L1 正则化的目标函数
2. 使用坐标下降法求解
3. 预测

### 數學公式

**目标函数**:
$$J(w) = \sum_{i=1}^{n}(y_i - \hat{y}_i)^2 + \lambda \sum_{j=1}^{m} |w_j|$$

**软阈值算子**:
$$S_{\lambda}(a) = \begin{cases} a - \lambda & if a > \lambda \\\ 0 & if |a| \leq \lambda \\\ a + \lambda & if a < -\lambda \end{cases}$$

## Python 實作對照

### 软阈值

```python
def soft_threshold(a, b):
    if b > abs(a):
        return 0
    elif a > 0:
        return a - b
    else:
        return a + b
```

实现软阈值算子 $S_{\lambda}(a)$。

### 坐标下降

```python
def lasso_coordinate_descent(data, lambda_reg, max_iter=500):
    w0, w1 = 0.0, 0.0
    for _ in range(max_iter):
        rho = sum(x * (y - w0) for x, y in data)
        if rho > lambda_reg:
            w1 = (rho - lambda_reg) / sum(x * x for x, y in data)
        elif rho < -lambda_reg:
            w1 = (rho + lambda_reg) / sum(x * x for x, y in data)
        else:
            w1 = 0
    return [w0, w1]
```

对每个坐标使用软阈值更新：
- 如果 $\rho > \lambda$：$w_j = \frac{\rho - \lambda}{||x_j||^2}$
- 如果 $\rho < -\lambda$：$w_j = \frac{\rho + \lambda}{||x_j||^2}$
- 否则：$w_j = 0$

## 使用場景

- 特征选择
- 高维稀疏数据
- 可解释性

## 優缺點

| 優點 | 缺點 |
|------|------|
| 特征选择 | 求解不稳定 |
| 稀疏解 | 需要选择 $\lambda$ |
| 可解释性 | 对相关特征处理不佳 |