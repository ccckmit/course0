"""
爬山演算法範例
展示局部搜索和 hill climbing 技術
"""

import random
import math
from typing import List, Tuple, Callable


class HillClimbing:
    """爬山演算法"""
    
    def __init__(self):
        self.steps = 0
    
    def climb(self, f: Callable, x0: float, step_size: float = 0.1,
             max_iter: int = 1000) -> Tuple[float, float, int]:
        """基本爬山"""
        self.steps = 0
        x = x0
        current_val = f(x)
        
        for _ in range(max_iter):
            self.steps += 1
            
            candidates = [x - step_size, x + step_size]
            best_candidate = x
            best_value = current_val
            
            for c in candidates:
                val = f(c)
                if val < best_value:
                    best_candidate = c
                    best_value = val
            
            if best_value >= current_val:
                break
            
            x = best_candidate
            current_val = best_value
        
        return x, current_val, self.steps
    
    def climb_stochastic(self, f: Callable, x0: float, 
                        max_iter: int = 1000) -> Tuple[float, float, int]:
        """隨機爬山"""
        self.steps = 0
        x = x0
        current_val = f(x)
        
        for _ in range(max_iter):
            self.steps += 1
            
            delta = random.uniform(-0.5, 0.5)
            new_x = x + delta
            new_val = f(new_x)
            
            if new_val < current_val:
                x = new_x
                current_val = new_val
        
        return x, current_val, self.steps


class SimulatedAnnealing:
    """模擬退火"""
    
    def __init__(self):
        self.steps = 0
    
    def anneal(self, f: Callable, x0: float, initial_temp: float = 100,
              cooling_rate: float = 0.95, 
              min_temp: float = 0.01) -> Tuple[float, float, int]:
        """模擬退火"""
        self.steps = 0
        x = x0
        current_val = f(x)
        temp = initial_temp
        
        while temp > min_temp:
            self.steps += 1
            
            delta = random.uniform(-temp/10, temp/10)
            new_x = x + delta
            new_val = f(new_x)
            
            delta_val = new_val - current_val
            
            if delta_val < 0 or random.random() < math.exp(-delta_val / temp):
                x = new_x
                current_val = new_val
            
            temp *= cooling_rate
        
        return x, current_val, self.steps


class BeamSearch:
    """束搜索"""
    
    def __init__(self, beam_width: int = 3):
        self.beam_width = beam_width
    
    def search(self, f: Callable, x_range: Tuple[float, float],
              max_iter: int = 100) -> Tuple[float, float]:
        """束搜索"""
        population = [(random.uniform(*x_range), f(random.uniform(*x_range))) 
                     for _ in range(self.beam_width)]
        
        for _ in range(max_iter):
            candidates = []
            
            for x, val in population:
                for delta in [-0.1, 0.1]:
                    new_x = x + delta
                    new_val = f(new_x)
                    candidates.append((new_x, new_val))
            
            candidates.sort(key=lambda x: x[1])
            population = candidates[:self.beam_width]
        
        return population[0]


def f1(x):
    """目標函數: x^2 - 4*x + 3"""
    return x**2 - 4*x + 3

def f2(x):
    """多峰函數: sin(x) + x/5"""
    return math.sin(x) + x/5

def f3(x, y):
    """Rastrigin 函數簡化版"""
    return x**2 + y**2 - 10*math.cos(2*math.pi*x) - 10*math.cos(2*math.pi*y)


def demo_basic_climbing():
    """基本爬山"""
    print("=" * 50)
    print("1. 基本爬山演算法")
    print("=" * 50)
    
    hc = HillClimbing()
    x_opt, val, steps = hc.climb(f1, x0=5.0, step_size=0.1)
    
    print(f"\n最小化 f(x) = x² - 4x + 3")
    print(f"初始: x = 5.0")
    print(f"結果: x = {x_opt:.4f}, f(x) = {val:.4f}")
    print(f"迭代次數: {steps}")


def demo_stochastic_climbing():
    """隨機爬山"""
    print("\n" + "=" * 50)
    print("2. 隨機爬山演算法")
    print("=" * 50)
    
    random.seed(42)
    hc = HillClimbing()
    x_opt, val, steps = hc.climb_stochastic(f1, x0=5.0, max_iter=1000)
    
    print(f"\n最小化 f(x) = x² - 4x + 3")
    print(f"結果: x = {x_opt:.4f}, f(x) = {val:.4f}")
    print(f"迭代次數: {steps}")


def demo_multimodal():
    """多峰函數"""
    print("\n" + "=" * 50)
    print("3. 多峰函數優化")
    print("=" * 50)
    
    hc = HillClimbing()
    
    print(f"\nf(x) = sin(x) + x/5")
    
    for start in [-5.0, 0.0, 5.0]:
        x_opt, val, steps = hc.climb(f2, x0=start, step_size=0.2, max_iter=500)
        print(f"  起始 x = {start}: x = {x_opt:.2f}, f(x) = {val:.2f}")


def demo_simulated_annealing():
    """模擬退火"""
    print("\n" + "=" * 50)
    print("4. 模擬退火")
    print("=" * 50)
    
    random.seed(42)
    sa = SimulatedAnnealing()
    
    x_opt, val, steps = sa.anneal(f1, x0=5.0, initial_temp=100, 
                                  cooling_rate=0.95, min_temp=0.01)
    
    print(f"\n最小化 f(x) = x² - 4x + 3")
    print(f"初始溫度: 100, 冷卻率: 0.95")
    print(f"結果: x = {x_opt:.4f}, f(x) = {val:.4f}")
    print(f"迭代次數: {steps}")


def demo_beam_search():
    """束搜索"""
    print("\n" + "=" * 50)
    print("5. 束搜索")
    print("=" * 50)
    
    random.seed(42)
    bs = BeamSearch(beam_width=3)
    
    x_opt, val = bs.search(f1, (-10, 10), max_iter=100)
    
    print(f"\n最小化 f(x) = x² - 4x + 3")
    print(f"結果: x = {x_opt:.4f}, f(x) = {val:.4f}")


def demo_comparison():
    """方法比較"""
    print("\n" + "=" * 50)
    print("6. 方法比較")
    print("=" * 50)
    
    random.seed(42)
    
    print("\n方法              結果x       f(x)        迭代次數")
    print("-" * 60)
    
    hc = HillClimbing()
    x, val, steps = hc.climb(f1, x0=5.0, max_iter=100)
    print(f"基本爬山         {x:8.4f}    {val:8.4f}    {steps}")
    
    random.seed(42)
    x, val, steps = hc.climb_stochastic(f1, x0=5.0, max_iter=100)
    print(f"隨機爬山         {x:8.4f}    {val:8.4f}    {steps}")
    
    random.seed(42)
    sa = SimulatedAnnealing()
    x, val, steps = sa.anneal(f1, x0=5.0, max_iter=100)
    print(f"模擬退火         {x:8.4f}    {val:8.4f}    {steps}")


def demo_restart():
    """多次隨機重啟"""
    print("\n" + "=" * 50)
    print("7. 多次隨機重啟")
    print("=" * 50)
    
    print("\n對多峰函數 f(x) = sin(x) + x/5:")
    
    results = []
    for _ in range(10):
        hc = HillClimbing()
        x, val, _ = hc.climb(f2, x0=random.uniform(-10, 10), max_iter=200)
        results.append((x, val))
    
    results.sort(key=lambda x: x[1])
    print(f"\n找到的最佳: x = {results[0][0]:.4f}, f(x) = {results[0][1]:.4f}")


if __name__ == "__main__":
    print("爬山演算法 Python 範例")
    print("=" * 50)
    
    demo_basic_climbing()
    demo_stochastic_climbing()
    demo_multimodal()
    demo_simulated_annealing()
    demo_beam_search()
    demo_comparison()
    demo_restart()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
