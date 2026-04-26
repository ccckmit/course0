# Polynomial Regression

## 原理

多項式回歸是線性回歸的扩展，通过将特征转换为多项式项来处理非线性关系。本质上仍然是一个线性模型，只是对输入特征进行了多项式变换。

### 算法步驟

1. 將特徵轉換為多項式：$[1, x, x^2, ..., x^d]$
2. 使用線性回歸的最小二乘法求解權重
3. 預測時將權重與多項式特徵相乘

### 數學公式

**多項式模型**:
$$\hat{y} = w_0 + w_1 x + w_2 x^2 + ... + w_d x^d$$

**特徵矩陣**:
$$X = \begin{bmatrix} 1 & x_1 & x_1^2 & ... & x_1^d \\\\ 1 & x_2 & x_2^2 & ... & x_2^d \\\\ ... \\\\ 1 & x_n & x_n^2 & ... & x_n^d \end{bmatrix}$$

**權重求解**:
$$w = (X^T X)^{-1} X^T y$$

## Python 實作對照

### 特徵擴展

```python
def poly_features(x, degree):
    return [x ** i for i in range(degree + 1)]
```

將 x 擴展為多項式特徵 $[1, x, x^2, ..., x^d]$。

### 多項式回歸

```python
def polynomial_regression(data, degree):
    X = [poly_features(x, degree) for x, y in data]
    Y = [[y] for x, y in data]
    Xt = transpose(X)
    XtX = mat_mul(Xt, X)
    XtX_inv = mat_inv(XtX)
    XtY = mat_vec_mul(Xt, [y for x, y in data])
    weights = mat_vec_mul(XtX_inv, XtY)
    return weights
```

使用閉式解 $(X^T X)^{-1} X^T y$ 求解權重。

### 矩陣求逆

```python
def mat_inv(A):
    n = len(A)
    M = [row[:] + [1 if i == j else 0 for j in range(n)] for i, row in enumerate(A)]
    # 高斯-喬旦消元法
    for i in range(n):
        pivot = M[i][i]
        for j in range(2 * n):
            M[i][j] /= pivot
        for k in range(n):
            if k != i:
                factor = M[k][i]
                for j in range(2 * n):
                    M[k][j] -= factor * M[i][j]
    return [row[n:] for row in M]
```

使用高斯-喬旦消元法求矩陣逆。

## 使用場景

- 非線性關係
- 曲線擬合
- 時間趨勢

## 優缺點

| 優點 | 缺點 |
|------|------|
| 可處理曲線關係 | 高次多項式容易過擬合 |
| 保持線性模型結構 | 需要特徵縮放 |
| 閉式解精確 | 計算複雜度較高 |