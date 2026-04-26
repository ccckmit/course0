"""
空間複雜度範例
展示各種空間複雜度的計算與分析
"""

import sys
import random
from typing import List, Set, Dict, Tuple
from functools import lru_cache


class SpaceComplexity:
    """空間複雜度分析"""
    
    @staticmethod
    def o_1() -> int:
        """O(1) - 常數空間"""
        return 42
    
    @staticmethod
    def o_n(n: int) -> List:
        """O(n) - 線性空間"""
        return list(range(n))
    
    @staticmethod
    def o_n_2(n: int) -> List[List]:
        """O(n²) - 平方空間"""
        return [[0] * n for _ in range(n)]
    
    @staticmethod
    def o_log_n(n: int) -> int:
        """O(log n) - 對數空間 (遞迴深度)"""
        if n <= 1:
            return 0
        return 1 + SpaceComplexity.o_log_n(n // 2)
    
    @staticmethod
    def o_2_n(n: int) -> List[List]:
        """O(2ⁿ) - 指數空間 (幂集)"""
        result = [[]]
        for _ in range(n):
            result = result + [subset + [len(result)] for subset in result]
        return result


class MemoryTracker:
    """記憶體追蹤"""
    
    @staticmethod
    def get_size(obj) -> int:
        """取得物件大小 (bytes)"""
        return sys.getsizeof(obj)
    
    @staticmethod
    def track_list(n: int) -> Tuple[List, int]:
        """追蹤列表空間"""
        arr = list(range(n))
        return arr, MemoryTracker.get_size(arr)
    
    @staticmethod
    def track_dict(n: int) -> Tuple[Dict, int]:
        """追蹤字典空間"""
        d = {i: i * 2 for i in range(n)}
        return d, MemoryTracker.get_size(d)
    
    @staticmethod
    def track_set(n: int) -> Tuple[Set, int]:
        """追蹤集合空間"""
        s = set(range(n))
        return s, MemoryTracker.get_size(s)


def demo_space_analysis():
    """空間分析範例"""
    print("=" * 50)
    print("1. 空間複雜度分析")
    print("=" * 50)
    
    print("\nO(1) - 常數空間:")
    result = SpaceComplexity.o_1()
    print(f"  傳回值: {result}, 大小: {MemoryTracker.get_size(result)} bytes")
    
    print("\nO(n) - 線性空間:")
    for n in [10, 100, 1000]:
        arr, size = MemoryTracker.track_list(n)
        print(f"  n={n}: 列表大小 = {size} bytes, 平均每元素 = {size/n:.1f} bytes")
    
    print("\nO(n²) - 平方空間:")
    for n in [5, 10, 20]:
        matrix = SpaceComplexity.o_n_2(n)
        size = MemoryTracker.get_size(matrix)
        print(f"  n={n}: 矩陣大小 = {size} bytes")


def demo_recursive_space():
    """遞迴空間範例"""
    print("\n" + "=" * 50)
    print("2. 遞迴空間")
    print("=" * 50)
    
    print("\n遞迴深度分析:")
    for n in [10, 100, 1000, 10000]:
        depth = SpaceComplexity.o_log_n(n)
        print(f"  n={n}: 遞迴深度 = {depth}")


def memo_fibonacci(n: int, memo: Dict) -> int:
    """使用記憶體優化的費波那契"""
    if n in memo:
        return memo[n]
    if n <= 1:
        return n
    memo[n] = memo_fibonacci(n - 1, memo) + memo_fibonacci(n - 2, memo)
    return memo[n]


def naive_fibonacci(n: int) -> int:
    """暴力費波那契"""
    if n <= 1:
        return n
    return naive_fibonacci(n - 1) + naive_fibonacci(n - 2)


def demo_memoization():
    """記憶化範例"""
    print("\n" + "=" * 50)
    print("3. 記憶化空間優化")
    print("=" * 50)
    
    n = 20
    
    print(f"\n計算 F({n}):")
    
    import time
    memo = {}
    
    start = time.perf_counter()
    result = memo_fibonacci(n, memo)
    t1 = time.perf_counter() - start
    
    start = time.perf_counter()
    result2 = naive_fibonacci(n)
    t2 = time.perf_counter() - start
    
    print(f"  記憶化結果: {result}, 時間: {t1:.6f} 秒")
    print(f"  暴力結果: {result2}, 時間: {t2:.6f} 秒")
    print(f"  加速比: {t2/t1:.1f}x")
    print(f"  記憶體使用: {len(memo)} 個条目")


def demo_matrix_space():
    """矩陣空間範例"""
    print("\n" + "=" * 50)
    print("4. 矩陣空間使用")
    print("=" * 50)
    
    print("\n不同矩陣類型的空間使用:")
    print(f"{'類型':<20} {'n':<8} {'空間':<15}")
    print("-" * 43)
    
    for n in [10, 50, 100]:
        dense = [[1] * n for _ in range(n)]
        print(f"{'密集矩陣':<20} {n:<8} {MemoryTracker.get_size(dense):<15}")
        
        sparse = {(i, i): 1 for i in range(n)}
        print(f"{'稀疏矩陣':<20} {n:<8} {MemoryTracker.get_size(sparse):<15}")


def demo_generator_vs_list():
    """生成器 vs 列表"""
    print("\n" + "=" * 50)
    print("5. 生成器 vs 列表")
    print("=" * 50)
    
    def generate_list(n: int) -> List:
        return list(range(n))
    
    def generate_range(n: int):
        for i in range(n):
            yield i
    
    n = 10000
    
    print(f"\n生成 {n} 個整數:")
    
    lst = generate_list(n)
    print(f"  列表: 大小 = {MemoryTracker.get_size(lst)} bytes")
    
    gen = generate_range(n)
    print(f"  生成器: 大小 = {MemoryTracker.get_size(gen)} bytes")
    print(f"  取出所有元素: {len(list(gen))} 個")


def demo_stack_space():
    """堆疊空間範例"""
    print("\n" + "=" * 50)
    print("6. 遞迴堆疊空間")
    print("=" * 50)
    
    sys.setrecursionlimit(100)
    
    def stack_depth(n: int) -> int:
        if n <= 0:
            return 0
        return 1 + stack_depth(n - 1)
    
    print("\n遞迴深度測試:")
    for n in [10, 50]:
        depth = stack_depth(n)
        print(f"  n={n}: 堆疊深度 = {depth}")


def demo_space_tradeoff():
    """空間與時間權衡"""
    print("\n" + "=" * 50)
    print("7. 空間與時間權衡")
    print("=" * 50)
    
    n = 1000
    
    arr = list(range(n))
    
    print(f"\n查詢陣列中是否存在 {n//2}:")
    
    print("\n方式1: O(n) 時間, O(1) 空間")
    start = (n//2 in arr)
    print(f"  結果: {start}")
    
    print("\n方式2: 建立雜湊表 O(1) 時間, O(n) 空間")
    s = set(arr)
    start = (n//2 in s)
    print(f"  結果: {start}")
    print(f"  雜湊表大小: {MemoryTracker.get_size(s)} bytes")


if __name__ == "__main__":
    print("空間複雜度 Python 範例")
    print("=" * 50)
    
    demo_space_analysis()
    demo_recursive_space()
    demo_memoization()
    demo_matrix_space()
    demo_generator_vs_list()
    demo_stack_space()
    demo_space_tradeoff()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
