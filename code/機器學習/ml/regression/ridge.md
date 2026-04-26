# Ridge Regression

## 原理

岭回归是在线性回归的基础上添加 L2 正则化项的回归方法，用于处理多重共线性（特征高度相关）和防止过拟合。L2 正则化会将权重收缩到接近零但不会完全为零。

### 算法步驟

1. 建立带正则化的目标函数
2. 求解 $(X^T X + \lambda I)^{-1} X^T y$
3. 预测

### 數學公式

**目标函数**:
$$J(w) = \sum_{i=1}^{n}(y_i - \hat{y}_i)^2 + \lambda \sum_{j=1}^{m} w_j^2$$

**闭式解**:
$$w = (X^T X + \lambda I)^{-1} X^T y$$

其中 $\lambda$ 是正则化参数，$I$ 是单位矩阵。

## Python 實作對照

### 加入正则化

```python
def ridge_regression(data, lambda_reg):
    X = [[1, x] for x, y in data]
    Y = [y for x, y in data]
    Xt = transpose(X)
    XtX = mat_mul(Xt, X)

    for i in range(len(XtX)):
        XtX[i][i] += lambda_reg
```

在 $X^T X$ 的对角线上加入 $\lambda$：

$$X^T X + \lambda I$$

### 完整求解

```python
XtX_inv = mat_inv(XtX)
XtY = mat_vec_mul(Xt, Y)
weights = mat_vec_mul(XtX_inv, XtY)
```

对应闭式解 $(X^T X + \lambda I)^{-1} X^T y$。

## 使用場景

- 特征高度相关
- 防止过拟合
- 高维数据

## 優缺點

| 優點 | 缺點 |
|------|------|
| 稳定求解 | 所有特征都有非零权重 |
| 防止多重共线性 | 需要选择 $\lambda$ |
| 处理高维数据 | 不具备特征选择能力 |