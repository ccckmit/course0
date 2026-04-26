"""
蒙地卡羅法範例
展示隨機模擬和 Monte Carlo 方法
"""

import random
import math
from typing import Tuple, List


class MonteCarlo:
    """蒙地卡羅方法"""
    
    @staticmethod
    def estimate_pi(n_points: int = 100000) -> float:
        """估計 π 值"""
        inside = 0
        
        for _ in range(n_points):
            x = random.random()
            y = random.random()
            
            if x**2 + y**2 <= 1:
                inside += 1
        
        return 4 * inside / n_points
    
    @staticmethod
    def estimate_integral(f, a: float, b: float, n: int = 100000) -> float:
        """估計定積分"""
        total = 0
        
        for _ in range(n):
            x = random.uniform(a, b)
            total += f(x)
        
        return total * (b - a) / n
    
    @staticmethod
    def estimate_area(n: int = 100000) -> float:
        """估計圓面積"""
        inside = 0
        
        for _ in range(n):
            x = random.uniform(-1, 1)
            y = random.uniform(-1, 1)
            
            if x**2 + y**2 <= 1:
                inside += 1
        
        return inside / n * 4
    
    @staticmethod
    def bradley_estimation(p_success: float, n: int, confidence: float = 0.95) -> Tuple[float, float]:
        """Bradley 估計"""
        from scipy import stats
        
        z = stats.norm.ppf((1 + confidence) / 2)
        
        p_hat = p_success
        margin = z * math.sqrt(p_hat * (1 - p_hat) / n)
        
        return p_hat - margin, p_hat + margin


def estimate_integration_mc():
    """蒙地卡羅積分"""
    print("=" * 50)
    print("1. 蒙地卡羅積分")
    print("=" * 50)
    
    f = lambda x: x**2
    
    for n in [100, 1000, 10000, 100000]:
        result = MonteCarlo.estimate_integral(f, 0, 1, n)
        error = abs(result - 1/3)
        print(f"n = {n:6d}: ∫x²dx = {result:.6f}, 誤差 = {error:.6f}")
    
    print(f"\n精確值: 1/3 ≈ 0.333333")


def estimate_pi_mc():
    """估計 π"""
    print("\n" + "=" * 50)
    print("2. 蒙地卡羅估計 π")
    print("=" * 50)
    
    for n in [100, 1000, 10000, 100000]:
        pi_est = MonteCarlo.estimate_pi(n)
        error = abs(pi_est - math.pi)
        print(f"n = {n:6d}: π ≈ {pi_est:.6f}, 誤差 = {error:.6f}")


def estimate_area_mc():
    """估計面積"""
    print("\n" + "=" * 50)
    print("3. 蒙地卡羅估計面積")
    print("=" * 50)
    
    for n in [100, 1000, 10000, 100000]:
        area = MonteCarlo.estimate_area(n)
        print(f"n = {n:6d}: 圓面積 ≈ {area:.6f}, 精確 = {math.pi:.6f}")


def random_walk():
    """隨機漫步"""
    print("\n" + "=" * 50)
    print("4. 隨機漫步")
    print("=" * 50)
    
    n_steps = 1000
    n_walks = 10
    
    print(f"\n{n_walks} 次隨機漫步, 每次 {n_steps} 步:")
    
    for walk in range(n_walks):
        position = 0
        positions = [position]
        
        for _ in range(n_steps):
            position += random.choice([-1, 1])
            positions.append(position)
        
        final = positions[-1]
        print(f"  漫步 {walk + 1}: 終點 = {final:4d}, 距離原點 = {abs(final)}")


def buffon_needle():
    """Buffon 針問題"""
    print("\n" + "=" * 50)
    print("5. Buffon 針問題")
    print("=" * 50)
    
    L = 1.0
    d = 2.0
    
    n = 10000
    crosses = 0
    
    for _ in range(n):
        x = random.uniform(0, d/2)
        theta = random.uniform(0, math.pi/2)
        
        if x <= (L/2) * math.sin(theta):
            crosses += 1
    
    pi_estimate = (2 * L * n) / (d * crosses)
    
    print(f"\nBuffon 針問題 (L={L}, d={d}):")
    print(f"n = {n}: π ≈ {pi_estimate:.4f}, 精確 = {math.pi:.4f}")


def gambler_ruin():
    """賭徒破產問題"""
    print("\n" + "=" * 50)
    print("6. 賭徒破產問題")
    print("=" * 50)
    
    initial_money = 100
    target = 200
    p = 0.5
    
    n_simulations = 10000
    wins = 0
    
    for _ in range(n_simulations):
        money = initial_money
        
        while 0 < money < target:
            if random.random() < p:
                money += 1
            else:
                money -= 1
        
        if money == target:
            wins += 1
    
    prob = wins / n_simulations
    
    print(f"\n初始: {initial_money}, 目標: {target}, p = {p}")
    print(f"模擬 {n_simulations} 次: 獲勝機率 = {prob:.4f}")
    print(f"理論值 (p=0.5): {initial_money/target:.4f}")


def rejection_sampling():
    """拒絕採樣"""
    print("\n" + "=" * 50)
    print("7. 拒絕採樣")
    print("=" * 50)
    
    def target_pdf(x):
        return math.exp(-x**2 / 2) / math.sqrt(2 * math.pi)
    
    def proposal_sample():
        return random.uniform(-3, 3)
    
    M = 1.0
    samples = []
    n = 10000
    rejected = 0
    
    for _ in range(n):
        x = proposal_sample()
        u = random.random()
        
        if u * M <= target_pdf(x):
            samples.append(x)
        else:
            rejected += 1
    
    mean = sum(samples) / len(samples)
    print(f"\n從標準常態採樣 {len(samples)} 個樣本")
    print(f"平均值: {mean:.4f} (目標: 0)")
    print(f"拒絕率: {rejected/n*100:.1f}%")


def demo_monte_carlo_tree_search():
    """MCTS 簡化版"""
    print("\n" + "=" * 50)
    print("8. 蒙地卡羅樹搜索概念")
    print("=" * 50)
    
    def simulate():
        state = 0
        for _ in range(10):
            state += random.choice([-1, 1])
        return state
    
    n_simulations = 1000
    wins = 0
    
    for _ in range(n_simulations):
        if simulate() > 0:
            wins += 1
    
    print(f"\n模擬 {n_simulations} 次遊戲:")
    print(f"勝利次數: {wins}")
    print(f"估計勝率: {wins/n_simulations:.4f}")


if __name__ == "__main__":
    random.seed(42)
    
    print("蒙地卡羅法 Python 範例")
    print("=" * 50)
    
    estimate_integration_mc()
    estimate_pi_mc()
    estimate_area_mc()
    random_walk()
    buffon_needle()
    gambler_ruin()
    rejection_sampling()
    demo_monte_carlo_tree_search()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
