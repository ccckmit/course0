#!/usr/bin/env python3
"""
線性回歸 - 監督式學習基礎範例
展示線性回歸的理論與實作
"""

import math


class LinearRegression:
    """簡單線性回歸"""
    
    def __init__(self):
        self.weights = None
        self.bias = None
    
    def fit(self, X: list, y: list) -> None:
        """訓練模型 - 最小二乘法"""
        n = len(X)
        mean_x = sum(X) / n
        mean_y = sum(y) / n
        
        numerator = sum((X[i] - mean_x) * (y[i] - mean_y) for i in range(n))
        denominator = sum((X[i] - mean_x) ** 2 for i in range(n))
        
        self.weights = numerator / denominator
        self.bias = mean_y - self.weights * mean_x
    
    def predict(self, X: list) -> list:
        """預測"""
        return [self.weights * x + self.bias for x in X]
    
    def predict_single(self, x: float) -> float:
        """單一預測"""
        return self.weights * x + self.bias


class MultipleLinearRegression:
    """多元線性回歸"""
    
    def __init__(self):
        self.weights = None
    
    def fit(self, X: list, y: list) -> None:
        """
        訓練 - 閉式解
        X: 2D list (樣本數 x 特徵數)
        y: 1D list
        """
        import numpy as np
        
        X = np.array(X)
        y = np.array(y)
        
        # 閉式解: w = (X^T X)^(-1) X^T y
        XTX = X.T @ X
        XTX_inv = np.linalg.inv(XTX)
        XTy = X.T @ y
        
        self.weights = XTX_inv @ XTy
    
    def predict(self, X: list) -> list:
        """預測"""
        import numpy as np
        X = np.array(X)
        return (X @ self.weights).tolist()


class RidgeRegression:
    """嶺回歸（加入正則化）"""
    
    def __init__(self, alpha: float = 1.0):
        self.alpha = alpha
        self.weights = None
    
    def fit(self, X: list, y: list) -> None:
        """訓練 - 加入 L2 正則化"""
        import numpy as np
        
        X = np.array(X)
        y = np.array(y)
        n_samples, n_features = X.shape
        
        # 嶺回歸閉式解: w = (X^T X + αI)^(-1) X^T y
        XTX = X.T @ X
        ridge = self.alpha * np.eye(n_features)
        self.weights = np.linalg.inv(XTX + ridge) @ X.T @ y
    
    def predict(self, X: list) -> list:
        import numpy as np
        X = np.array(X)
        return (X @ self.weights).tolist()


def mean_squared_error(y_true: list, y_pred: list) -> float:
    """MSE 損失函數"""
    return sum((y_true[i] - y_pred[i]) ** 2 for i in range(len(y_true))) / len(y_true)


def r_squared(y_true: list, y_pred: list) -> float:
    """R² 評估指標"""
    mean_y = sum(y_true) / len(y_true)
    ss_res = sum((y_true[i] - y_pred[i]) ** 2 for i in range(len(y_true)))
    ss_tot = sum((y_true[i] - mean_y) ** 2 for i in range(len(y_true)))
    return 1 - ss_res / ss_tot


if __name__ == "__main__":
    # 簡單線性回歸測試
    print("=== 簡單線性回歸 ===")
    X = [1, 2, 3, 4, 5]
    y = [2, 4, 5, 4, 5]
    
    model = LinearRegression()
    model.fit(X, y)
    
    print(f"權重: {model.weights:.4f}")
    print(f"偏差: {model.bias:.4f}")
    
    X_test = [6, 7]
    y_pred = model.predict(X_test)
    print(f"預測 X={X_test}: y={y_pred}")
    
    # MSE 和 R²
    y_pred_train = model.predict(X)
    mse = mean_squared_error(y, y_pred_train)
    r2 = r_squared(y, y_pred_train)
    print(f"MSE: {mse:.4f}")
    print(f"R²: {r2:.4f}")
    
    # 多元線性回歸測試
    print("\n=== 多元線性回歸 ===")
    X_multi = [
        [1, 2],
        [2, 3],
        [3, 4],
        [4, 5],
        [5, 6]
    ]
    y_multi = [3, 5, 7, 9, 11]
    
    model_multi = MultipleLinearRegression()
    model_multi.fit(X_multi, y_multi)
    print(f"權重: {model_multi.weights}")
