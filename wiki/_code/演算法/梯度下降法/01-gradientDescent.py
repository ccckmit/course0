"""
梯度下降法範例
展示各種梯度下降優化技術
"""

import math
import random
from typing import List, Tuple, Callable


class GradientDescent:
    """梯度下降"""
    
    def __init__(self, learning_rate: float = 0.01):
        self.lr = learning_rate
    
    def optimize_1d(self, f: Callable, grad_f: Callable, 
                   x0: float, max_iter: int = 1000, 
                   tolerance: float = 1e-6) -> Tuple[float, List[float]]:
        """一元函數優化"""
        x = x0
        history = [x]
        
        for _ in range(max_iter):
            gradient = grad_f(x)
            x_new = x - self.lr * gradient
            
            if abs(x_new - x) < tolerance:
                break
            
            x = x_new
            history.append(x)
        
        return x, history
    
    def optimize_2d(self, f: Callable, grad_f: Callable,
                   x0: Tuple[float, float], max_iter: int = 1000,
                   tolerance: float = 1e-6) -> Tuple[Tuple[float, float], List]:
        """二元函數優化"""
        x, y = x0
        history = [(x, y)]
        
        for _ in range(max_iter):
            gx, gy = grad_f(x, y)
            x_new = x - self.lr * gx
            y_new = y - self.lr * gy
            
            if abs(x_new - x) < tolerance and abs(y_new - y) < tolerance:
                break
            
            x, y = x_new, y_new
            history.append((x, y))
        
        return (x, y), history


class MomentumGD:
    """動量梯度下降"""
    
    def __init__(self, learning_rate: float = 0.01, momentum: float = 0.9):
        self.lr = learning_rate
        self.momentum = momentum
        self.velocity = 0
    
    def optimize(self, f: Callable, grad_f: Callable,
                x0: float, max_iter: int = 1000) -> Tuple[float, List[float]]:
        """動量優化"""
        x = x0
        self.velocity = 0
        history = [x]
        
        for _ in range(max_iter):
            gradient = grad_f(x)
            self.velocity = self.momentum * self.velocity - self.lr * gradient
            x = x + self.velocity
            history.append(x)
        
        return x, history


class AdaptiveGD:
    """自適應學習率"""
    
    def __init__(self):
        self.epsilon = 1e-8
        self.sum_sq_grad = 0
    
    def optimize(self, f: Callable, grad_f: Callable,
                x0: float, max_iter: int = 1000) -> Tuple[float, List[float]]:
        """AdaGrad"""
        x = x0
        history = [x]
        self.sum_sq_grad = 0
        
        for _ in range(max_iter):
            gradient = grad_f(x)
            self.sum_sq_grad += gradient ** 2
            
            adjusted_lr = self.lr / (self.epsilon + math.sqrt(self.sum_sq_grad))
            x = x - adjusted_lr * gradient
            history.append(x)
        
        return x, history


def f1(x):
    """x^2 + 2x + 1"""
    return x**2 + 2*x + 1

def grad_f1(x):
    """2x + 2"""
    return 2*x + 2

def f2(x, y):
    """x^2 + y^2"""
    return x**2 + y**2

def grad_f2(x, y):
    return 2*x, 2*y


def demo_basic_gd():
    """基本梯度下降"""
    print("=" * 50)
    print("1. 基本梯度下降")
    print("=" * 50)
    
    gd = GradientDescent(learning_rate=0.1)
    
    x_opt, history = gd.optimize_1d(f1, grad_f1, x0=5.0)
    
    print(f"\n最小化 f(x) = x² + 2x + 1")
    print(f"初始: x = 5.0")
    print(f"最優解: x = {x_opt:.4f}")
    print(f"迭代次數: {len(history)}")
    print(f"最終 f(x) = {f1(x_opt):.4f}")


def demo_2d_gd():
    """二元函數梯度下降"""
    print("\n" + "=" * 50)
    print("2. 二元函數梯度下降")
    print("=" * 50)
    
    gd = GradientDescent(learning_rate=0.1)
    
    (x_opt, y_opt), history = gd.optimize_2d(f2, grad_f2, (5.0, 5.0))
    
    print(f"\n最小化 f(x,y) = x² + y²")
    print(f"初始: (5.0, 5.0)")
    print(f"最優解: ({x_opt:.4f}, {y_opt:.4f})")
    print(f"迭代次數: {len(history)}")


def demo_learning_rate():
    """學習率影響"""
    print("\n" + "=" * 50)
    print("3. 學習率影響")
    print("=" * 50)
    
    print("\n學習率          收斂迭代      最終x值")
    print("-" * 50)
    
    for lr in [0.001, 0.01, 0.1, 0.5]:
        gd = GradientDescent(learning_rate=lr)
        x_opt, history = gd.optimize_1d(f1, grad_f1, x0=5.0, max_iter=100)
        print(f"{lr:15.3f} {len(history):12d} {x_opt:12.4f}")


def demo_momentum():
    """動量梯度下降"""
    print("\n" + "=" * 50)
    print("4. 動量梯度下降")
    print("=" * 50)
    
    gd = GradientDescent(learning_rate=0.1)
    momentum = MomentumGD(learning_rate=0.1, momentum=0.9)
    
    x1, history1 = gd.optimize_1d(f1, grad_f1, x0=5.0, max_iter=50)
    x2, history2 = momentum.optimize(f1, grad_f1, x0=5.0, max_iter=50)
    
    print(f"\n初始: x = 5.0, 目標: x = -1")
    print(f"\n基本 GD: {len(history1)} 次迭代, x = {x1:.4f}")
    print(f"動量 GD: {len(history2)} 次迭代, x = {x2:.4f}")


def demo_convergence():
    """收斂過程"""
    print("\n" + "=" * 50)
    print("5. 收斂過程")
    print("=" * 50)
    
    gd = GradientDescent(learning_rate=0.1)
    x_opt, history = gd.optimize_1d(f1, grad_f1, x0=5.0)
    
    print(f"\n前10次迭代:")
    for i, x in enumerate(history[:10]):
        print(f"  迭代 {i}: x = {x:.4f}, f(x) = {f1(x):.4f}")


def demo_stochastic_gd():
    """隨機梯度下降"""
    print("\n" + "=" * 50)
    print("6. 隨機梯度下降 (SGD)")
    print("=" * 50)
    
    def sgd_optimize(f, grad_f, x0, n_iterations, batch_size):
        x = x0
        for _ in range(n_iterations):
            gradient = grad_f(x) + random.uniform(-0.1, 0.1)
            x = x - 0.1 * gradient
        return x
    
    x_opt = sgd_optimize(f1, grad_f1, 5.0, 100)
    print(f"\nSGD 結果: x = {x_opt:.4f}, f(x) = {f1(x_opt):.4f}")


def demo_applications():
    """應用範例"""
    print("\n" + "=" * 50)
    print("7. 應用: 線性回歸")
    print("=" * 50)
    
    X = [1, 2, 3, 4, 5]
    y = [2, 4, 5, 4, 5]
    
    def loss(w, b):
        return sum((w*x + b - yi)**2 for x, yi in zip(X, y)) / len(X)
    
    def grad_w(w, b):
        return 2 * sum((w*x + b - yi)*x for x, yi in zip(X, y)) / len(X)
    
    def grad_b(w, b):
        return 2 * sum(w*x + b - yi for x, yi in zip(X, y)) / len(X)
    
    w, b = 0.0, 0.0
    lr = 0.01
    
    for _ in range(1000):
        w = w - lr * grad_w(w, b)
        b = b - lr * grad_b(w, b)
    
    print(f"\n資料: X = {X}, y = {y}")
    print(f"擬合: y = {w:.2f}x + {b:.2f}")
    
    for x, yi in zip(X, y):
        pred = w * x + b
        print(f"  x={x}: 實際={yi}, 預測={pred:.2f}")


if __name__ == "__main__":
    print("梯度下降法 Python 範例")
    print("=" * 50)
    
    demo_basic_gd()
    demo_2d_gd()
    demo_learning_rate()
    demo_momentum()
    demo_convergence()
    demo_stochastic_gd()
    demo_applications()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
