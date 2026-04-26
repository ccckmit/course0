"""
微積分範例
展示微分、積分、微分方程和數值方法的原理與應用
"""

import math
import sympy as sp
import numpy as np
from typing import Callable, Tuple, List
from dataclasses import dataclass


class SymbolicCalculus:
    """符號微積分"""
    
    @staticmethod
    def differentiate(expr: str, var: str = 'x') -> str:
        """符號微分"""
        x = sp.symbols(var)
        f = sp.sympify(expr)
        return str(sp.diff(f, x))
    
    @staticmethod
    def integrate(expr: str, var: str = 'x') -> str:
        """符號積分 (不定積分)"""
        x = sp.symbols(var)
        f = sp.sympify(expr)
        return str(sp.integrate(f, x))
    
    @staticmethod
    def definite_integrate(expr: str, a: float, b: float, 
                          var: str = 'x') -> float:
        """定積分"""
        x = sp.symbols(var)
        f = sp.sympify(expr)
        return float(sp.integrate(f, (x, a, b)))
    
    @staticmethod
    def limit(expr: str, var: str = 'x', value: str = '0') -> str:
        """極限"""
        x = sp.symbols(var)
        f = sp.sympify(expr)
        if value == 'oo':
            return str(sp.limit(f, x, sp.oo))
        return str(sp.limit(f, x, sp.sympify(value)))
    
    @staticmethod
    def series(expr: str, var: str = 'x', order: int = 4) -> str:
        """泰勒級數"""
        x = sp.symbols(var)
        f = sp.sympify(expr)
        return str(sp.series(f, x, 0, order))


class NumericalDifferentiation:
    """數值微分"""
    
    @staticmethod
    def forward_diff(f: Callable, x: float, h: float = 1e-5) -> float:
        """前向差分"""
        return (f(x + h) - f(x)) / h
    
    @staticmethod
    def backward_diff(f: Callable, x: float, h: float = 1e-5) -> float:
        """後向差分"""
        return (f(x) - f(x - h)) / h
    
    @staticmethod
    def central_diff(f: Callable, x: float, h: float = 1e-5) -> float:
        """中央差分 (更高精度)"""
        return (f(x + h) - f(x - h)) / (2 * h)
    
    @staticmethod
    def second_derivative(f: Callable, x: float, h: float = 1e-5) -> float:
        """二階導數"""
        return (f(x + h) - 2*f(x) + f(x - h)) / (h ** 2)


class NumericalIntegration:
    """數值積分"""
    
    @staticmethod
    def trapezoid(f: Callable, a: float, b: float, n: int = 1000) -> float:
        """梯形法則"""
        h = (b - a) / n
        result = 0.5 * (f(a) + f(b))
        for i in range(1, n):
            result += f(a + i * h)
        return result * h
    
    @staticmethod
    def simpson(f: Callable, a: float, b: float, n: int = 1000) -> float:
        """辛普森法則 (需要 n 為偶數)"""
        if n % 2 == 1:
            n += 1
        h = (b - a) / n
        result = f(a) + f(b)
        
        for i in range(1, n):
            coeff = 4 if i % 2 == 1 else 2
            result += coeff * f(a + i * h)
        
        return result * h / 3
    
    @staticmethod
    def romberg(f: Callable, a: float, b: float, 
               max_iter: int = 5) -> float:
        """Romberg 積分"""
        def richardson(R, R_prev, k):
            return (4**k * R - R_prev) / (4**k - 1)
        
        R = [[0] * max_iter for _ in range(max_iter)]
        
        for i in range(max_iter):
            n = 2**i
            R[i][0] = NumericalIntegration.trapezoid(f, a, b, n)
            
            for j in range(1, i + 1):
                R[i][j] = richardson(R[i][j-1], R[i-1][j-1], j)
        
        return R[max_iter-1][max_iter-1]


class DifferentialEquation:
    """微分方程求解"""
    
    @staticmethod
    def euler(f: Callable, y0: float, x0: float, 
             h: float, n: int) -> Tuple[List[float], List[float]]:
        """尤拉法 (一階微分方程)"""
        x_values = [x0]
        y_values = [y0]
        
        for i in range(n):
            y_next = y_values[-1] + h * f(x_values[-1], y_values[-1])
            x_next = x_values[-1] + h
            
            x_values.append(x_next)
            y_values.append(y_next)
        
        return x_values, y_values
    
    @staticmethod
    def runge_kutta_4(f: Callable, y0: float, x0: float,
                     h: float, n: int) -> Tuple[List[float], List[float]]:
        """四階龍格-庫塔法"""
        x_values = [x0]
        y_values = [y0]
        
        for i in range(n):
            x = x_values[-1]
            y = y_values[-1]
            
            k1 = h * f(x, y)
            k2 = h * f(x + h/2, y + k1/2)
            k3 = h * f(x + h/2, y + k2/2)
            k4 = h * f(x + h, y + k3)
            
            y_next = y + (k1 + 2*k2 + 2*k3 + k4) / 6
            x_next = x + h
            
            x_values.append(x_next)
            y_values.append(y_next)
        
        return x_values, y_values


class GradientDescent:
    """梯度下降"""
    
    def __init__(self, learning_rate: float = 0.1):
        self.lr = learning_rate
    
    def minimize(self, f: Callable, grad_f: Callable, 
                x0: float, max_iter: int = 100, 
                tolerance: float = 1e-6) -> float:
        """一元函數梯度下降"""
        x = x0
        
        for i in range(max_iter):
            gradient = grad_f(x)
            x_new = x - self.lr * gradient
            
            if abs(x_new - x) < tolerance:
                print(f"收斂於第 {i+1} 次迭代")
                break
            
            x = x_new
        
        return x
    
    def minimize_2d(self, f: Callable, grad_f: Callable,
                    x0: tuple, max_iter: int = 100,
                    tolerance: float = 1e-6) -> tuple:
        """二元函數梯度下降"""
        x, y = x0
        
        for i in range(max_iter):
            gx, gy = grad_f(x, y)
            x_new = x - self.lr * gx
            y_new = y - self.lr * gy
            
            if abs(x_new - x) < tolerance and abs(y_new - y) < tolerance:
                print(f"收斂於第 {i+1} 次迭代")
                break
            
            x, y = x_new, y_new
        
        return x, y


class PartialDerivative:
    """偏導數"""
    
    @staticmethod
    def partial_diff(f: Callable, x: float, y: float,
                    h: float = 1e-5) -> Tuple[float, float]:
        """數值偏導數"""
        df_dx = (f(x + h, y) - f(x - h, y)) / (2 * h)
        df_dy = (f(x, y + h) - f(x, y - h)) / (2 * h)
        return df_dx, df_dy


def demo_symbolic_calculus():
    """符號微積分範例"""
    print("=" * 50)
    print("1. 符號微積分")
    print("=" * 50)
    
    x = sp.symbols('x')
    
    expressions = ['x**2', 'sin(x)', 'exp(x)', 'log(x)']
    
    for expr in expressions:
        f = sp.sympify(expr)
        derivative = sp.diff(f, x)
        integral = sp.integrate(f, x)
        
        print(f"\nf(x) = {expr}")
        print(f"  f'(x) = {derivative}")
        print(f"  ∫f(x)dx = {integral}")
    
    limit_expr = 'sin(x)/x'
    limit_result = sp.limit(sp.sympify(limit_expr), x, 0)
    print(f"\nlim(x→0) {limit_expr} = {limit_result}")


def demo_numerical_differentiation():
    """數值微分範例"""
    print("\n" + "=" * 50)
    print("2. 數值微分")
    print("=" * 50)
    
    f = lambda x: x**3 + 2*x**2 + 3*x + 4
    f_prime = lambda x: 3*x**2 + 4*x + 3
    
    x = 2.0
    
    exact = f_prime(x)
    forward = NumericalDifferentiation.forward_diff(f, x)
    backward = NumericalDifferentiation.backward_diff(f, x)
    central = NumericalDifferentiation.central_diff(f, x)
    
    print(f"\nf(x) = x³ + 2x² + 3x + 4 在 x = {x}")
    print(f"精確導數: {exact}")
    print(f"前向差分: {forward:.10f} (誤差: {abs(exact - forward):.2e})")
    print(f"後向差分: {backward:.10f} (誤差: {abs(exact - backward):.2e})")
    print(f"中央差分: {central:.10f} (誤差: {abs(exact - central):.2e})")


def demo_numerical_integration():
    """數值積分範例"""
    print("\n" + "=" * 50)
    print("3. 數值積分")
    print("=" * 50)
    
    f = lambda x: math.sin(x)
    a, b = 0, math.pi
    
    exact = 2.0
    
    trapezoid = NumericalIntegration.trapezoid(f, a, b, 100)
    simpson = NumericalIntegration.simpson(f, a, b, 100)
    romberg = NumericalIntegration.romberg(f, a, b, 5)
    
    print(f"\n∫₀^π sin(x) dx")
    print(f"精確值: {exact}")
    print(f"梯形法則: {trapezoid:.10f} (誤差: {abs(exact - trapezoid):.2e})")
    print(f"辛普森法則: {simpson:.10f} (誤差: {abs(exact - simpson):.2e})")
    print(f"Romberg: {romberg:.10f} (誤差: {abs(exact - romberg):.2e})")


def demo_differential_equation():
    """微分方程範例"""
    print("\n" + "=" * 50)
    print("4. 微分方程求解")
    print("=" * 50)
    
    dy_dx = lambda x, y: -0.5 * y
    
    y0 = 1.0
    x0 = 0.0
    h = 0.1
    n = 20
    
    x_euler, y_euler = DifferentialEquation.euler(dy_dx, y0, x0, h, n)
    x_rk4, y_rk4 = DifferentialEquation.runge_kutta_4(dy_dx, y0, x0, h, n)
    
    exact = lambda x: math.exp(-0.5 * x)
    
    print(f"\ndy/dx = -0.5y, y(0) = 1")
    print(f"在 x = {x_rk4[-1]}:")
    print(f"  精確解: {exact(x_rk4[-1]):.6f}")
    print(f"  尤拉法: {y_euler[-1]:.6f} (誤差: {abs(exact(x_rk4[-1]) - y_euler[-1]):.6f})")
    print(f"  RK4: {y_rk4[-1]:.6f} (誤差: {abs(exact(x_rk4[-1]) - y_rk4[-1]):.6f})")


def demo_gradient_descent():
    """梯度下降範例"""
    print("\n" + "=" * 50)
    print("5. 梯度下降")
    print("=" * 50)
    
    f = lambda x: x**2 + 2*x + 1
    grad_f = lambda x: 2*x + 2
    
    gd = GradientDescent(learning_rate=0.1)
    result = gd.minimize(f, grad_f, x0=5.0)
    
    print(f"\nf(x) = x² + 2x + 1 = (x+1)²")
    print(f"最小值位置: x = {result:.6f}")
    print(f"最小值: f(x) = {f(result):.6f}")
    
    f2d = lambda x, y: x**2 + y**2
    grad_2d = lambda x, y: (2*x, 2*y)
    
    result_2d = gd.minimize_2d(f2d, grad_2d, (5.0, 5.0))
    print(f"\nf(x,y) = x² + y²")
    print(f"最小值位置: ({result_2d[0]:.6f}, {result_2d[1]:.6f})")


def demo_partial_derivative():
    """偏導數範例"""
    print("\n" + "=" * 50)
    print("6. 偏導數")
    print("=" * 50)
    
    f = lambda x, y: x**2 * y + x * y**2
    
    x, y = 2.0, 3.0
    
    df_dx, df_dy = PartialDerivative.partial_diff(f, x, y)
    
    exact_dx = 2*x*y + y**2
    exact_dy = x**2 + 2*x*y
    
    print(f"\nf(x,y) = x²y + xy² 在 (x,y) = ({x}, {y})")
    print(f"∂f/∂x = {df_dx:.6f} (精確: {exact_dx})")
    print(f"∂f/∂y = {df_dy:.6f} (精確: {exact_dy})")


def demo_taylor_series():
    """泰勒級數範例"""
    print("\n" + "=" * 50)
    print("7. 泰勒級數")
    print("=" * 50)
    
    expr = 'exp(x)'
    series = sp.series(sp.sympify(expr), sp.symbols('x'), 0, 5)
    
    print(f"\nexp(x) 的泰勒級數 (x=0, 到 x⁴):")
    print(f"  {series}")
    
    x = 0.5
    exact = math.exp(x)
    approx = 1 + x + x**2/2 + x**3/6 + x**4/24
    
    print(f"\n在 x = {x}:")
    print(f"  精確值: {exact:.6f}")
    print(f"  近似值: {approx:.6f}")
    print(f"  誤差: {abs(exact - approx):.6f}")


if __name__ == "__main__":
    print("微積分 Python 範例")
    print("=" * 50)
    
    demo_symbolic_calculus()
    demo_numerical_differentiation()
    demo_numerical_integration()
    demo_differential_equation()
    demo_gradient_descent()
    demo_partial_derivative()
    demo_taylor_series()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
