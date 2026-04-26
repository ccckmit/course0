"""
迭代法範例
展示各種迭代技術和收斂方法
"""

import math
import random
from typing import List, Tuple, Callable


class FixedPointIteration:
    """定點迭代"""
    
    @staticmethod
    def solve(g: Callable, x0: float, tolerance: float = 1e-6,
             max_iter: int = 100) -> Tuple[float, int, List[float]]:
        """定點迭代法"""
        x = x0
        history = [x]
        
        for i in range(max_iter):
            x_new = g(x)
            history.append(x_new)
            
            if abs(x_new - x) < tolerance:
                return x_new, i + 1, history
            
            x = x_new
        
        return x, max_iter, history


class NewtonRaphson:
    """牛頓-拉弗森法"""
    
    @staticmethod
    def solve(f: Callable, df: Callable, x0: float,
             tolerance: float = 1e-6, max_iter: int = 100) -> Tuple[float, int, List[float]]:
        """牛頓法"""
        x = x0
        history = [x]
        
        for i in range(max_iter):
            fx = f(x)
            dfx = df(x)
            
            if abs(dfx) < 1e-10:
                break
            
            x_new = x - fx / dfx
            history.append(x_new)
            
            if abs(x_new - x) < tolerance:
                return x_new, i + 1, history
            
            x = x_new
        
        return x, max_iter, history


class SecantMethod:
    """割線法"""
    
    @staticmethod
    def solve(f: Callable, x0: float, x1: float,
             tolerance: float = 1e-6, max_iter: int = 100) -> Tuple[float, int]:
        """割線法"""
        
        for i in range(max_iter):
            f0, f1 = f(x0), f(x1)
            
            if abs(f1 - f0) < 1e-10:
                break
            
            x_new = x1 - f1 * (x1 - x0) / (f1 - f0)
            
            if abs(x_new - x1) < tolerance:
                return x_new, i + 1
            
            x0, x1 = x1, x_new
        
        return x1, max_iter


class BisectionMethod:
    """二分法"""
    
    @staticmethod
    def solve(f: Callable, a: float, b: float,
             tolerance: float = 1e-6, max_iter: int = 100) -> Tuple[float, int]:
        """二分搜尋求根"""
        
        fa, fb = f(a), f(b)
        
        if fa * fb >= 0:
            raise ValueError("f(a) 和 f(b) 必須異號")
        
        for i in range(max_iter):
            c = (a + b) / 2
            fc = f(c)
            
            if abs(fc) < tolerance or (b - a) / 2 < tolerance:
                return c, i + 1
            
            if fa * fc < 0:
                b, fb = c, fc
            else:
                a, fa = c, fc
        
        return c, max_iter


class GaussSeidel:
    """高斯-塞德爾迭代"""
    
    @staticmethod
    def solve(A: List[List[float]], b: List[float], 
             tolerance: float = 1e-6, max_iter: int = 100) -> Tuple[List[float], int]:
        """求解線性方程組"""
        n = len(b)
        x = [0] * n
        
        for iteration in range(max_iter):
            x_new = x.copy()
            
            for i in range(n):
                sum1 = sum(A[i][j] * x_new[j] for j in range(i))
                sum2 = sum(A[i][j] * x[j] for j in range(i + 1, n))
                x_new[i] = (b[i] - sum1 - sum2) / A[i][i]
            
            if all(abs(x_new[i] - x[i]) < tolerance for i in range(n)):
                return x_new, iteration + 1
            
            x = x_new
        
        return x, max_iter


def f1(x):
    """x^2 - 4 = 0 的根"""
    return x**2 - 4

def df1(x):
    return 2*x

def g1(x):
    """x = g(x) 形式"""
    return math.sqrt(4 - x)


def demo_fixed_point():
    """定點迭代"""
    print("=" * 50)
    print("1. 定點迭代法")
    print("=" * 50)
    
    g = lambda x: math.sqrt(4 - x)
    
    root, iterations, history = FixedPointIteration.solve(g, x0=1.0)
    
    print(f"\n求解 x = sqrt(4 - x)")
    print(f"初始值: 1.0")
    print(f"根: {root:.6f}")
    print(f"迭代次數: {iterations}")


def demo_newton_raphson():
    """牛頓-拉弗森法"""
    print("\n" + "=" * 50)
    print("2. 牛頓-拉弗森法")
    print("=" * 50)
    
    root, iterations, history = NewtonRaphson.solve(f1, df1, x0=3.0)
    
    print(f"\n求解 x^2 - 4 = 0")
    print(f"初始值: 3.0")
    print(f"根: {root:.6f}")
    print(f"迭代次數: {iterations}")
    
    print(f"\n收斂過程:")
    for i, x in enumerate(history[:6]):
        print(f"  迭代 {i}: x = {x:.6f}")


def demo_secant():
    """割線法"""
    print("\n" + "=" * 50)
    print("3. 割線法")
    print("=" * 50)
    
    root, iterations = SecantMethod.solve(f1, x0=2.0, x1=4.0)
    
    print(f"\n求解 x^2 - 4 = 0")
    print(f"初始值: x0=2.0, x1=4.0")
    print(f"根: {root:.6f}")
    print(f"迭代次數: {iterations}")


def demo_bisection():
    """二分法"""
    print("\n" + "=" * 50)
    print("4. 二分法")
    print("=" * 50)
    
    root, iterations = BisectionMethod.solve(f1, a=0, b=5)
    
    print(f"\n求解 x^2 - 4 = 0, [0, 5]")
    print(f"根: {root:.6f}")
    print(f"迭代次數: {iterations}")


def demo_compare():
    """方法比較"""
    print("\n" + "=" * 50)
    print("5. 方法比較")
    print("=" * 50)
    
    print("\n求解 x^2 - 4 = 0:")
    print(f"{'方法':<20} {'迭代次數':<12} {'根':<12}")
    print("-" * 45)
    
    root, iter1, _ = NewtonRaphson.solve(f1, df1, x0=3.0)
    print(f"{'牛頓-拉弗森':<20} {iter1:<12} {root:<12.6f}")
    
    root, iter2 = SecantMethod.solve(f1, 2.0, 4.0)
    print(f"{'割線法':<20} {iter2:<12} {root:<12.6f}")
    
    root, iter3 = BisectionMethod.solve(f1, 0, 5)
    print(f"{'二分法':<20} {iter3:<12} {root:<12.6f}")


def demo_gauss_seidel():
    """高斯-塞德爾"""
    print("\n" + "=" * 50)
    print("6. 高斯-塞德爾迭代")
    print("=" * 50)
    
    A = [[10, -1, 2],
         [-1, 11, -1],
         [2, -1, 10]]
    b = [6, 25, -11]
    
    solution, iterations = GaussSeidel.solve(A, b)
    
    print(f"\n求解線性方程組:")
    print(f"A = {A}")
    print(f"b = {b}")
    print(f"解: {solution}")
    print(f"迭代次數: {iterations}")
    
    Ax = sum(A[i][j] * solution[j] for j in range(len(b))) for i in range(len(b))]
    print(f"驗證 Ax = {Ax}")


def demo_iteration_convergence():
    """迭代收斂"""
    print("\n" + "=" * 50)
    print("7. 收斂速度")
    print("=" * 50)
    
    f = lambda x: math.exp(-x) - x
    df = lambda x: -math.exp(-x) - 1
    
    x_newton, iter_n, _ = NewtonRaphson.solve(f, df, x0=1.0, max_iter=10)
    x_bisect, iter_b, _ = BisectionMethod.solve(f, 0, 2, max_iter=10)
    
    print(f"\n求解 e^(-x) - x = 0:")
    print(f"牛頓法 10 次迭代: x = {x_newton:.6f}")
    print(f"二分法 10 次迭代: x = {x_bisect:.6f}")


if __name__ == "__main__":
    print("迭代法 Python 範例")
    print("=" * 50)
    
    demo_fixed_point()
    demo_newton_raphson()
    demo_secant()
    demo_bisection()
    demo_compare()
    demo_gauss_seidel()
    demo_iteration_convergence()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
