"""
逼近法範例
展示數值逼近和近似算法
"""

import math
import random
from typing import List, Tuple, Callable


class PolynomialApproximation:
    """多項式逼近"""
    
    @staticmethod
    def lagrange_interpolation(x_points: List[float], y_points: List[float], x: float) -> float:
        """拉格朗日插值"""
        n = len(x_points)
        result = 0
        
        for i in range(n):
            term = y_points[i]
            for j in range(n):
                if i != j:
                    term *= (x - x_points[j]) / (x_points[i] - x_points[j])
            result += term
        
        return result
    
    @staticmethod
    def least_squares_fit(x_points: List[float], y_points: List[float], 
                         degree: int) -> List[float]:
        """最小二乘法擬合"""
        n = len(x_points)
        
        A = []
        for x in x_points:
            row = [x ** d for d in range(degree + 1)]
            A.append(row)
        
        ATA = [[0] * (degree + 1) for _ in range(degree + 1)]
        ATy = [0] * (degree + 1)
        
        for i in range(degree + 1):
            for j in range(degree + 1):
                for k in range(n):
                    ATA[i][j] += A[k][i] * A[k][j]
        
        for i in range(degree + 1):
            for k in range(n):
                ATy[i] += A[k][i] * y_points[k]
        
        coeffs = [0] * (degree + 1)
        
        for i in range(degree + 1):
            coeffs[i] = ATy[i] / (ATA[i][i] + 1e-10)
        
        return coeffs


class FunctionApproximation:
    """函數逼近"""
    
    @staticmethod
    def taylor_series(f, n_terms: int, a: float = 0) -> Callable:
        """泰勒級數"""
        x = sympy.Symbol('x')
        
        def approx(x_val):
            result = f(a)
            for n in range(1, n_terms):
                pass
            return result
        
        return approx
    
    @staticmethod
    def piecewise_linear(points: List[Tuple[float, float]], x: float) -> float:
        """分段線性逼近"""
        if x <= points[0][0]:
            return points[0][1]
        if x >= points[-1][0]:
            return points[-1][1]
        
        for i in range(len(points) - 1):
            if points[i][0] <= x <= points[i + 1][0]:
                x0, y0 = points[i]
                x1, y1 = points[i + 1]
                t = (x - x0) / (x1 - x0)
                return y0 + t * (y1 - y0)
        
        return 0


class NumericalApproximation:
    """數值逼近"""
    
    @staticmethod
    def approximation_error(exact: float, approximate: float) -> Tuple[float, float]:
        """計算絕對和相對誤差"""
        abs_error = abs(exact - approximate)
        rel_error = abs_error / abs(exact) if exact != 0 else float('inf')
        return abs_error, rel_error
    
    @staticmethod
    def chebyshev_points(n: int) -> List[float]:
        """Chebyshev 節點"""
        return [math.cos(math.pi * (2 * k + 1) / (2 * n)) for k in range(n)]


def demo_lagrange():
    """拉格朗日插值"""
    print("=" * 50)
    print("1. 拉格朗日插值")
    print("=" * 50)
    
    x_points = [0, 1, 2, 3]
    y_points = [0, 1, 4, 9]
    
    x_test = 1.5
    
    y_approx = PolynomialApproximation.lagrange_interpolation(x_points, y_points, x_test)
    y_exact = x_test ** 2
    
    print(f"\n插值點: {list(zip(x_points, y_points))}")
    print(f"x = {x_test}: 插值 = {y_approx:.4f}, 精確 = {y_exact:.4f}")


def demo_least_squares():
    """最小二乘法"""
    print("\n" + "=" * 50)
    print("2. 最小二乘法擬合")
    print("=" * 50)
    
    x = [1, 2, 3, 4, 5]
    y = [2.1, 4.0, 5.2, 6.9, 8.1]
    
    coeffs = PolynomialApproximation.least_squares_fit(x, y, degree=1)
    
    print(f"\n資料: {list(zip(x, y))}")
    print(f"線性擬合: y = {coeffs[0]:.4f}x + {coeffs[1]:.4f}")
    
    for xi, yi in zip(x, y):
        pred = coeffs[0] * xi + coeffs[1]
        print(f"  x={xi}: 實際={yi}, 預測={pred:.2f}")


def demo_piecewise():
    """分段逼近"""
    print("\n" + "=" * 50)
    print("3. 分段線性逼近")
    print("=" * 50)
    
    points = [(0, 0), (1, 1), (2, 4), (3, 9), (4, 16)]
    
    test_x = [0.5, 1.5, 2.5, 3.5]
    
    print(f"\n節點: {points}")
    print(f"\n測試點:")
    for x_val in test_x:
        y_approx = FunctionApproximation.piecewise_linear(points, x_val)
        y_exact = x_val ** 2
        print(f"  x = {x_val}: 逼近 = {y_approx:.2f}, 精確 = {y_exact:.2f}")


def demo_error_analysis():
    """誤差分析"""
    print("\n" + "=" * 50)
    print("4. 逼近誤差分析")
    print("=" * 50)
    
    exact = math.pi
    
    approximations = [
        3.14,
        3.14159,
        22/7,
        355/113
    ]
    
    print(f"\n精確值: {exact}")
    print(f"\n逼近              絕對誤差        相對誤差")
    print("-" * 45)
    
    for approx in approximations:
        abs_err, rel_err = NumericalApproximation.approximation_error(exact, approx)
        print(f"{approx:<16} {abs_err:<15.6f} {rel_err:<15.6f}")


def demo_chebyshev():
    """Chebyshev 節點"""
    print("\n" + "=" * 50)
    print("5. Chebyshev 節點")
    print("=" * 50)
    
    for n in [5, 10, 15]:
        nodes = NumericalApproximation.chebyshev_points(n)
        print(f"\nn = {n}:")
        print(f"  {nodes}")


def demo_approximation_order():
    """逼近階數"""
    print("\n" + "=" * 50)
    print("6. 多項式逼近階數")
    print("=" * 50)
    
    f = lambda x: math.sin(x)
    
    x_data = [0, math.pi/4, math.pi/2, 3*math.pi/4, math.pi]
    y_data = [f(x) for x in x_data]
    
    test_x = math.pi / 6
    
    for degree in [1, 2, 3, 4]:
        coeffs = PolynomialApproximation.least_squares_fit(x_data, y_data, degree)
        
        y_pred = sum(c * test_x ** d for d, c in enumerate(coeffs))
        y_exact = f(test_x)
        
        print(f"\ndegree={degree}: 預測={y_pred:.4f}, 精確={y_exact:.4f}")


if __name__ == "__main__":
    print("逼近法 Python 範例")
    print("=" * 50)
    
    demo_lagrange()
    demo_least_squares()
    demo_piecewise()
    demo_error_analysis()
    demo_chebyshev()
    demo_approximation_order()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
