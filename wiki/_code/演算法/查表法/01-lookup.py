"""
查表法範例
展示預先計算、緩存和查表技術
"""

import math
from typing import List, Dict, Callable, Any
from functools import lru_cache


class LookupTable:
    """查表法"""
    
    def __init__(self):
        self.table: Dict = {}
    
    def add(self, key: Any, value: Any):
        """新增條目"""
        self.table[key] = value
    
    def get(self, key: Any, default: Any = None) -> Any:
        """取得值"""
        return self.table.get(key, default)
    
    def has(self, key: Any) -> bool:
        """檢查是否存在"""
        return key in self.table


class FactorialTable:
    """階層表"""
    
    def __init__(self, max_n: int = 100):
        self.table = [1] * (max_n + 1)
        for i in range(1, max_n + 1):
            self.table[i] = self.table[i - 1] * i
    
    def get(self, n: int) -> int:
        if n >= len(self.table):
            raise ValueError(f"n 超出範圍: {n} > {len(self.table) - 1}")
        return self.table[n]


class SineTable:
    """正弦表 (預先計算)"""
    
    def __init__(self, precision: int = 360):
        self.table = {}
        for i in range(precision):
            angle = 2 * math.pi * i / precision
            self.table[angle] = math.sin(angle)
    
    def sin_approx(self, angle: float) -> float:
        """查表近似"""
        if angle in self.table:
            return self.table[angle]
        
        for key in self.table:
            if abs(key - angle) < 0.01:
                return self.table[key]
        
        return math.sin(angle)


class MemoizationCache:
    """記憶化緩存"""
    
    def __init__(self, func: Callable):
        self.func = func
        self.cache: Dict = {}
    
    def __call__(self, *args):
        if args in self.cache:
            return self.cache[args]
        result = self.func(*args)
        self.cache[args] = result
        return result


class DistanceTable:
    """距離表 (城市距離預先計算)"""
    
    def __init__(self, cities: List[str]):
        self.cities = cities
        n = len(cities)
        self.distances = [[0] * n for _ in range(n)]
    
    def set_distance(self, city1: str, city2: str, dist: float):
        i = self.cities.index(city1)
        j = self.cities.index(city2)
        self.distances[i][j] = dist
        self.distances[j][i] = dist
    
    def get_distance(self, city1: str, city2: str) -> float:
        i = self.cities.index(city1)
        j = self.cities.index(city2)
        return self.distances[i][j]


def fibonacci_memo(n: int, cache: Dict = None) -> int:
    """使用查表法的費波那契"""
    if cache is None:
        cache = {}
    
    if n in cache:
        return cache[n]
    
    if n <= 1:
        return n
    
    cache[n] = fibonacci_memo(n - 1, cache) + fibonacci_memo(n - 2, cache)
    return cache[n]


def memoized_factorial(n: int, table: FactorialTable) -> int:
    """查表法階層"""
    return table.get(n)


def demo_lookup_table():
    """基本查表範例"""
    print("=" * 50)
    print("1. 基本查表法")
    print("=" * 50)
    
    lut = LookupTable()
    
    lut.add("apple", 1)
    lut.add("banana", 2)
    lut.add("orange", 3)
    
    print(f"\n查詢 'apple': {lut.get('apple')}")
    print(f"查詢 'grape': {lut.get('grape', -1)}")
    print(f"'banana' 存在: {lut.has('banana')}")


def demo_factorial_table():
    """階層表範例"""
    print("\n" + "=" * 50)
    print("2. 階層表")
    print("=" * 50)
    
    table = FactorialTable(20)
    
    for n in [5, 10, 15, 20]:
        print(f"{n}! = {table.get(n)}")


def demo_sine_table():
    """正弦表範例"""
    print("\n" + "=" * 50)
    print("3. 正弦表")
    print("=" * 50)
    
    sine_table = SineTable(360)
    
    angles = [0, math.pi/6, math.pi/4, math.pi/2, math.pi]
    
    print("\n角度          查表值         實際值         誤差")
    print("-" * 55)
    for angle in angles:
        approx = sine_table.sin_approx(angle)
        actual = math.sin(angle)
        error = abs(approx - actual)
        print(f"{angle:12.4f} {approx:12.4f} {actual:12.4f} {error:.6f}")


def demo_memoization():
    """記憶化範例"""
    print("\n" + "=" * 50)
    print("4. 記憶化")
    print("=" * 50)
    
    import time
    
    n = 30
    
    cache = {}
    start = time.perf_counter()
    result1 = fibonacci_memo(n, cache)
    t1 = time.perf_counter() - start
    
    cache2 = {}
    start = time.perf_counter()
    result2 = fibonacci_memo(n, cache2)
    t2 = time.perf_counter() - start
    
    print(f"\n計算 F({n}) = {result1}")
    print(f"第一次: {t1:.6f} 秒, 第二次: {t2:.6f} 秒")
    print(f"加速比: {t1/t2:.1f}x")


def demo_distance_table():
    """距離表範例"""
    print("\n" + "=" * 50)
    print("5. 距離表")
    print("=" * 50)
    
    cities = ["Taipei", "Taichung", "Kaohsiung"]
    dist_table = DistanceTable(cities)
    
    dist_table.set_distance("Taipei", "Taichung", 170)
    dist_table.set_distance("Taipei", "Kaohsiung", 350)
    dist_table.set_distance("Taichung", "Kaohsiung", 190)
    
    print(f"\n城市: {cities}")
    print(f"台北-台中: {dist_table.get_distance('Taipei', 'Taichung')} km")
    print(f"台北-高雄: {dist_table.get_distance('Taipei', 'Kaohsiung')} km")
    print(f"台中-高雄: {dist_table.get_distance('Taichung', 'Kaohsiung')} km")


def demo_caching_decorator():
    """緩存裝飾器範例"""
    print("\n" + "=" * 50)
    print("6. @lru_cache 裝飾器")
    print("=" * 50)
    
    @lru_cache(maxsize=128)
    def expensive_function(n: int) -> int:
        result = 0
        for i in range(1000000):
            result += i
        return result + n
    
    import time
    
    n = 100
    
    start = time.perf_counter()
    for _ in range(100):
        expensive_function(n)
    t1 = time.perf_counter() - start
    
    start = time.perf_counter()
    for _ in range(100):
        expensive_function(n)
    t2 = time.perf_counter() - start
    
    print(f"\n呼叫昂貴函式 100 次:")
    print(f"  第一次執行後: {t1:.6f} 秒")
    print(f"  之後從緩存: {t2:.6f} 秒")
    print(f"  加速比: {t1/t2:.1f}x")


def demo_lookup_in_algorithms():
    """演算法中的查表範例"""
    print("\n" + "=" * 50)
    print("7. 演算法中的查表")
    print("=" * 50)
    
    ascii_table = {}
    for i in range(32, 127):
        ascii_table[chr(i)] = i
    
    chars = ['A', 'B', 'Z', 'a', 'z', '0', '9']
    
    print("\nASCII 查表:")
    for char in chars:
        print(f"  '{char}': {ascii_table[char]}")


if __name__ == "__main__":
    print("查表法 Python 範例")
    print("=" * 50)
    
    demo_lookup_table()
    demo_factorial_table()
    demo_sine_table()
    demo_memoization()
    demo_distance_table()
    demo_caching_decorator()
    demo_lookup_in_algorithms()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
