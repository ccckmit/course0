#!/usr/bin/env python3
"""
梯度下降法 - 優化演算法範例
展示 GD、SGD、牛頓法等優化方法
"""

import random
import math


def gradient_descent(f, grad_f, x0, learning_rate=0.1, tolerance=1e-6, max_iter=1000):
    """
    批量梯度下降
    
    參數:
        f: 目標函數
        grad_f: 梯度函數
        x0: 初始點
        learning_rate: 學習率
        tolerance: 收斂容忍值
        max_iter: 最大迭代次數
    
    返回: (最優解, 最小值)
    """
    x = x0
    
    for i in range(max_iter):
        gradient = grad_f(x)
        new_x = x - learning_rate * gradient
        
        if abs(new_x - x) < tolerance:
            print(f"收斂於迭代 {i}")
            break
        
        x = new_x
    
    return x, f(x)


def stochastic_gradient_descent(X, y, loss_func, grad_func, 
                                learning_rate=0.01, epochs=100):
    """
    隨機梯度下降
    
    參數:
        X: 訓練數據
        y: 標籤
        loss_func: 損失函數
        grad_func: 梯度函數
    """
    n = len(X)
    weights = [0.0] * len(X[0])
    
    for epoch in range(epochs):
        # 打亂數據
        indices = list(range(n))
        random.shuffle(indices)
        
        total_loss = 0
        for i in indices:
            xi = X[i]
            yi = y[i]
            
            # 計算單個樣本的梯度
            gradient = grad_func(xi, yi, weights)
            
            # 更新權重
            for j in range(len(weights)):
                weights[j] -= learning_rate * gradient[j]
            
            total_loss += loss_func(xi, yi, weights)
        
        if epoch % 10 == 0:
            print(f"Epoch {epoch}, Loss: {total_loss/n:.4f}")
    
    return weights


def momentum(x0, grad_f, f, gamma=0.9, lr=0.1, iterations=100):
    """
    Momentum 動量法
    
    加入動量項來加速收斂
    """
    x = x0
    v = 0
    
    for _ in range(iterations):
        gradient = grad_f(x)
        v = gamma * v + lr * gradient
        x -= v
    
    return x, f(x)


def adam(x0, grad_f, f, alpha=0.001, beta1=0.9, beta2=0.999, 
         epsilon=1e-8, iterations=100):
    """
    Adam 優化器
    
    自適應學習率方法
    """
    x = x0
    m = 0  # 一階矩估計
    v = 0  # 二階矩估計
    
    for t in range(1, iterations + 1):
        gradient = grad_f(x)
        
        # 更新一階矩估計
        m = beta1 * m + (1 - beta1) * gradient
        # 更新二階矩估計
        v = beta2 * v + (1 - beta2) * gradient ** 2
        
        # 偏差校正
        m_hat = m / (1 - beta1 ** t)
        v_hat = v / (1 - beta2 ** t)
        
        # 更新參數
        x -= alpha * m_hat / (math.sqrt(v_hat) + epsilon)
    
    return x, f(x)


# 損失函數示例：線性回歸 MSE
def linear_mse_loss(X, y, weights):
    n = len(X)
    total = 0
    for xi, yi in zip(X, y):
        pred = sum(w * xj for w, xj in zip(weights, xi))
        total += (yi - pred) ** 2
    return total / n


def linear_mse_gradient(X, y, weights):
    n = len(X)
    grad = [0] * len(weights)
    for xi, yi in zip(X, y):
        pred = sum(w * xj for w, xj in zip(weights, xi))
        error = pred - yi
        for j, xj in enumerate(xi):
            grad[j] += error * xj
    return [g / n for g in grad]


# 示例：f(x) = x^2
def f1(x):
    return x ** 2

def grad_f1(x):
    return 2 * x


# 示例：Rosenbrock 函數
def rosenbrock(x, y, a=1, b=100):
    return (a - x) ** 2 + b * (y - x ** 2) ** 2


def rosenbrock_grad(x, y, a=1, b=100):
    dx = -2 * (a - x) - 4 * b * x * (y - x ** 2)
    dy = 2 * b * (y - x ** 2)
    return dx, dy


if __name__ == "__main__":
    # 測試梯度下降 - 最小化 x^2
    print("=== 梯度下降 (x^2) ===")
    x0 = 10.0
    result = gradient_descent(f1, grad_f1, x0, learning_rate=0.1)
    print(f"最優解: x = {result[0]:.6f}, f(x) = {result[1]:.6f}")
    
    # 測試 Momentum
    print("\n=== Momentum (x^2) ===")
    x0 = 10.0
    result = momentum(x0, grad_f1, f1)
    print(f"最優解: x = {result[0]:.6f}")
    
    # 測試 Adam
    print("\n=== Adam (x^2) ===")
    x0 = 10.0
    result = adam(x0, grad_f1, f1)
    print(f"最優解: x = {result[0]:.6f}")
    
    # 測試 SGD - 線性回歸
    print("\n=== SGD 線性回歸 ===")
    X = [[1, 1], [1, 2], [1, 3], [1, 4], [1, 5]]
    y = [2, 4, 5, 4, 5]
    weights = stochastic_gradient_descent(X, y, linear_mse_loss, linear_mse_gradient)
    print(f"學習到的權重: {weights}")
