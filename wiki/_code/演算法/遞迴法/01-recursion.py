"""
遞迴法範例
展示各種遞迴技術和演算法
"""

import math
from typing import List, Tuple


class Recursion:
    """遞迴方法"""
    
    @staticmethod
    def factorial(n: int) -> int:
        """階層"""
        if n <= 1:
            return 1
        return n * Recursion.factorial(n - 1)
    
    @staticmethod
    def fibonacci(n: int) -> int:
        """費波那契"""
        if n <= 1:
            return n
        return Recursion.fibonacci(n - 1) + Recursion.fibonacci(n - 2)
    
    @staticmethod
    def power(base: float, exp: int) -> float:
        """快速冪"""
        if exp == 0:
            return 1
        if exp < 0:
            return 1 / Recursion.power(base, -exp)
        
        half = Recursion.power(base, exp // 2)
        
        if exp % 2 == 0:
            return half * half
        return base * half * half
    
    @staticmethod
    def gcd(a: int, b: int) -> int:
        """最大公因數"""
        if b == 0:
            return a
        return Recursion.gcd(b, a % b)
    
    @staticmethod
    def reverse_string(s: str) -> str:
        """反轉字串"""
        if len(s) <= 1:
            return s
        return s[-1] + Recursion.reverse_string(s[:-1])
    
    @staticmethod
    def sum_array(arr: List) -> int:
        """陣列總和"""
        if not arr:
            return 0
        return arr[0] + Recursion.sum_array(arr[1:])
    
    @staticmethod
    def binary_search(arr: List, target, left: int, right: int) -> int:
        """二分搜尋"""
        if left > right:
            return -1
        
        mid = (left + right) // 2
        
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            return Recursion.binary_search(arr, target, mid + 1, right)
        else:
            return Recursion.binary_search(arr, target, left, mid - 1)
    
    @staticmethod
    def tower_of_hanoi(n: int, source: str, target: str, aux: str) -> List[Tuple[str, str]]:
        """河內塔"""
        if n == 1:
            return [(source, target)]
        
        moves = Recursion.tower_of_hanoi(n - 1, source, aux, target)
        moves.append((source, target))
        moves.extend(Recursion.tower_of_hanoi(n - 1, aux, target, source))
        
        return moves


def demo_factorial():
    """階層"""
    print("=" * 50)
    print("1. 階層")
    print("=" * 50)
    
    for n in [1, 5, 10, 20]:
        print(f"{n}! = {Recursion.factorial(n)}")


def demo_fibonacci():
    """費波那契"""
    print("\n" + "=" * 50)
    print("2. 費波那契")
    print("=" * 50)
    
    for n in [0, 1, 5, 10]:
        print(f"F({n}) = {Recursion.fibonacci(n)}")


def demo_power():
    """快速冪"""
    print("\n" + "=" * 50)
    print("3. 快速冪")
    print("=" * 50)
    
    print(f"2^10 = {Recursion.power(2, 10)}")
    print(f"3^5 = {Recursion.power(3, 5)}")
    print(f"2^-3 = {Recursion.power(2, -3)}")


def demo_gcd():
    """最大公因數"""
    print("\n" + "=" * 50)
    print("4. 最大公因數")
    print("=" * 50)
    
    pairs = [(48, 18), (100, 25), (17, 13)]
    
    for a, b in pairs:
        print(f"gcd({a}, {b}) = {Recursion.gcd(a, b)}")


def demo_reverse():
    """反轉字串"""
    print("\n" + "=" * 50)
    print("5. 反轉字串")
    print("=" * 50)
    
    s = "Hello, World!"
    print(f"'{s}' -> '{Recursion.reverse_string(s)}'")


def demo_sum():
    """陣列總和"""
    print("\n" + "=" * 50)
    print("6. 陣列總和")
    print("=" * 50)
    
    arr = [1, 2, 3, 4, 5]
    print(f"{arr} 的總和 = {Recursion.sum_array(arr)}")


def demo_binary_search():
    """二分搜尋"""
    print("\n" + "=" * 50)
    print("7. 二分搜尋")
    print("=" * 50)
    
    arr = [1, 3, 5, 7, 9, 11, 13, 15]
    
    for target in [7, 8]:
        idx = Recursion.binary_search(arr, target, 0, len(arr) - 1)
        print(f"搜尋 {target}: 位置 = {idx}")


def demo_hanoi():
    """河內塔"""
    print("\n" + "=" * 50)
    print("8. 河內塔")
    print("=" * 50)
    
    n = 3
    moves = Recursion.tower_of_hanoi(n, "A", "C", "B")
    
    print(f"\nn = {n} 的移動步驟 ({len(moves)} 步):")
    for i, (source, target) in enumerate(moves):
        print(f"  {i+1}: {source} -> {target}")


def demo_recursion_depth():
    """遞迴深度"""
    print("\n" + "=" * 50)
    print("9. 遞迴深度")
    print("=" * 50)
    
    import sys
    
    print(f"預設遞迴限制: {sys.getrecursionlimit()}")
    
    def count_depth(n):
        try:
            return count_depth(n + 1) + 1
        except RecursionError:
            return n
    
    depth = count_depth(0)
    print(f"實際遞迴深度: {depth}")


def memo_fibonacci(n: int, memo: dict = None) -> int:
    """記憶化費波那契"""
    if memo is None:
        memo = {}
    
    if n in memo:
        return memo[n]
    
    if n <= 1:
        return n
    
    memo[n] = memo_fibonacci(n - 1, memo) + memo_fibonacci(n - 2, memo)
    return memo[n]


def demo_memoization():
    """記憶化"""
    print("\n" + "=" * 50)
    print("10. 記憶化優化")
    print("=" * 50)
    
    import time
    
    n = 30
    
    print(f"\n計算 F({n}):")
    
    start = time.perf_counter()
    result1 = Recursion.fibonacci(n)
    t1 = time.perf_counter() - start
    
    start = time.perf_counter()
    result2 = memo_fibonacci(n)
    t2 = time.perf_counter() - start
    
    print(f"暴力遞迴: {t1:.4f} 秒")
    print(f"記憶化: {t2:.6f} 秒")
    print(f"加速比: {t1/t2:.0f}x")


if __name__ == "__main__":
    print("遞迴法 Python 範例")
    print("=" * 50)
    
    demo_factorial()
    demo_fibonacci()
    demo_power()
    demo_gcd()
    demo_reverse()
    demo_sum()
    demo_binary_search()
    demo_hanoi()
    demo_recursion_depth()
    demo_memoization()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
