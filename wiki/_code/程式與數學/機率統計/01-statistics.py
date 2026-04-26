#!/usr/bin/env python3
"""
機率統計 - 基礎統計分析程式
展示常見的機率分佈與統計分析
"""

import random
import math
from collections import Counter


def mean(data: list) -> float:
    """算術平均數"""
    return sum(data) / len(data)


def variance(data: list, sample: bool = True) -> float:
    """變異數"""
    m = mean(data)
    n = len(data) - 1 if sample else len(data)
    return sum((x - m) ** 2 for x in data) / n


def std_dev(data: list, sample: bool = True) -> float:
    """標準差"""
    return math.sqrt(variance(data, sample))


def median(data: list) -> float:
    """中位數"""
    sorted_data = sorted(data)
    n = len(sorted_data)
    if n % 2 == 0:
        return (sorted_data[n//2 - 1] + sorted_data[n//2]) / 2
    return sorted_data[n//2]


def mode(data: list):
    """眾數"""
    counter = Counter(data)
    max_count = max(counter.values())
    return [k for k, v in counter.items() if v == max_count]


def normal_pdf(x: float, mu: float = 0, sigma: float = 1) -> float:
    """常態分佈機率密度函數"""
    return (1 / (sigma * math.sqrt(2 * math.pi))) * \
           math.exp(-0.5 * ((x - mu) / sigma) ** 2)


def normal_cdf(x: float, mu: float = 0, sigma: float = 1) -> float:
    """常態分佈累積分布函數（近似）"""
    return 0.5 * (1 + math.erf((x - mu) / (sigma * math.sqrt(2))))


def binomial_probability(n: int, p: float, k: int) -> float:
    """二項分佈"""
    from math import comb
    return comb(n, k) * (p ** k) * ((1 - p) ** (n - k))


def poisson_probability(lambda_: float, k: int) -> float:
    """帕松分佈"""
    from math import factorial
    return (lambda_ ** k * math.exp(-lambda_)) / factorial(k)


def bootstrap_ci(data: list, confidence: float = 0.95, n_bootstrap: int = 1000) -> tuple:
    """Bootstrap 信頼區間"""
    random.seed(42)
    means = []
    for _ in range(n_bootstrap):
        sample = random.choices(data, k=len(data))
        means.append(mean(sample))
    
    alpha = (1 - confidence) / 2
    return sorted(means)[int(n_bootstrap * alpha)], sorted(means)[int(n_bootstrap * (1 - alpha))]


def chi_square_test(observed: list, expected: list) -> float:
    """卡方檢定"""
    return sum((o - e) ** 2 / e for o, e in zip(observed, expected))


def t_test(sample1: list, sample2: list) -> float:
    """獨立樣本 t 檢定（簡化版）"""
    n1, n2 = len(sample1), len(sample2)
    m1, m2 = mean(sample1), mean(sample2)
    v1, v2 = variance(sample1), variance(sample2)
    
    pooled_var = ((n1 - 1) * v1 + (n2 - 1) * v2) / (n1 + n2 - 2)
    se = math.sqrt(pooled_var * (1/n1 + 1/n2))
    
    return (m1 - m2) / se


def linear_regression(x: list, y: list) -> tuple:
    """簡單線性回歸"""
    n = len(x)
    m_x, m_y = mean(x), mean(y)
    
    numerator = sum((x[i] - m_x) * (y[i] - m_y) for i in range(n))
    denominator = sum((x[i] - m_x) ** 2 for i in range(n))
    
    slope = numerator / denominator
    intercept = m_y - slope * m_x
    
    return slope, intercept


if __name__ == "__main__":
    # 測試資料
    data = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
    
    print("=== 基本統計量 ===")
    print(f"平均數: {mean(data):.2f}")
    print(f"變異數: {variance(data):.2f}")
    print(f"標準差: {std_dev(data):.2f}")
    print(f"中位數: {median(data)}")
    print(f"眾數: {mode(data)}")
    
    # 常態分佈
    print("\n=== 常態分佈 ===")
    x = 0
    print(f"PDF(0, μ=0, σ=1): {normal_pdf(x):.4f}")
    print(f"CDF(0, μ=0, σ=1): {normal_cdf(x):.4f}")
    
    # 二項分佈
    print("\n=== 二項分佈 ===")
    n, p, k = 10, 0.5, 5
    print(f"Binomial(n={n}, p={p}, k={k}): {binomial_probability(n, p, k):.4f}")
    
    # 帕松分佈
    print("\n=== 帕松分佈 ===")
    lambda_, k = 3, 2
    print(f"Poisson(λ={lambda_}, k={k}): {poisson_probability(lambda_, k):.4f}")
    
    # Bootstrap 信賴區間
    print("\n=== Bootstrap 信賴區間 ===")
    data = [random.gauss(50, 10) for _ in range(100)]
    ci = bootstrap_ci(data)
    print(f"95% CI: ({ci[0]:.2f}, {ci[1]:.2f})")
    
    # 線性回歸
    print("\n=== 線性回歸 ===")
    x = [1, 2, 3, 4, 5]
    y = [2, 4, 5, 4, 5]
    slope, intercept = linear_regression(x, y)
    print(f"y = {slope:.2f}x + {intercept:.2f}")
