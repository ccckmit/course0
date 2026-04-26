"""
時間複雜度範例
展示各種時間複雜度的計算與分析
"""

import time
import random
from typing import List, Callable, Dict
from functools import wraps


def measure_time(func: Callable) -> Callable:
    """測量函數執行時間的裝飾器"""
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.perf_counter()
        result = func(*args, **kwargs)
        end = time.perf_counter()
        return result, end - start
    return wrapper


class ComplexityAnalyzer:
    """複雜度分析器"""
    
    @staticmethod
    def o_1(arr: List) -> int:
        """O(1) - 常數時間"""
        return arr[0]
    
    @staticmethod
    def o_log_n(arr: List, target: int) -> int:
        """O(log n) - 對數時間 (二分搜尋)"""
        left, right = 0, len(arr) - 1
        while left <= right:
            mid = (left + right) // 2
            if arr[mid] == target:
                return mid
            elif arr[mid] < target:
                left = mid + 1
            else:
                right = mid - 1
        return -1
    
    @staticmethod
    def o_n(arr: List, target: int) -> int:
        """O(n) - 線性時間"""
        for i, x in enumerate(arr):
            if x == target:
                return i
        return -1
    
    @staticmethod
    def o_n_log_n(arr: List) -> List:
        """O(n log n) - 線性對數時間 (合併排序)"""
        if len(arr) <= 1:
            return arr
        mid = len(arr) // 2
        left = ComplexityAnalyzer.o_n_log_n(arr[:mid])
        right = ComplexityAnalyzer.o_n_log_n(arr[mid:])
        return ComplexityAnalyzer._merge(left, right)
    
    @staticmethod
    def _merge(left: List, right: List) -> List:
        """合併兩個有序陣列"""
        result = []
        i = j = 0
        while i < len(left) and j < len(right):
            if left[i] <= right[j]:
                result.append(left[i])
                i += 1
            else:
                result.append(right[j])
                j += 1
        result.extend(left[i:])
        result.extend(right[j:])
        return result
    
    @staticmethod
    def o_n_2(arr: List) -> List:
        """O(n²) - 平方時間 (氣泡排序)"""
        arr = arr.copy()
        n = len(arr)
        for i in range(n):
            for j in range(0, n - i - 1):
                if arr[j] > arr[j + 1]:
                    arr[j], arr[j + 1] = arr[j + 1], arr[j]
        return arr
    
    @staticmethod
    def o_2_n(arr: List) -> List:
        """O(2ⁿ) - 指數時間 (費波那契遞迴)"""
        if arr[0] <= 1:
            return arr[0]
        return ComplexityAnalyzer.o_2_n([arr[0] - 1]) + ComplexityAnalyzer.o_2_n([arr[0] - 2])
    
    @staticmethod
    def o_factorial(n: int) -> int:
        """O(n!) - 階層時間 (排列)"""
        if n <= 1:
            return 1
        result = 1
        for i in range(2, n + 1):
            result *= i
        return result


class ComplexityComparison:
    """複雜度比較"""
    
    COMPLEXITIES = {
        'O(1)': lambda n: 1,
        'O(log n)': lambda n: n.bit_length(),
        'O(n)': lambda n: n,
        'O(n log n)': lambda n: n * n.bit_length() if n > 0 else 0,
        'O(n²)': lambda n: n * n,
        'O(2ⁿ)': lambda n: 2 ** min(n, 20),
        'O(n!)': lambda n: ComplexityAnalyzer.o_factorial(min(n, 12))
    }
    
    @classmethod
    def compare(cls, n: int = 10) -> Dict[str, float]:
        """比較不同複雜度"""
        result = {}
        for name, func in cls.COMPLEXITIES.items():
            try:
                result[name] = func(n)
            except:
                result[name] = float('inf')
        return result
    
    @classmethod
    def print_comparison(cls, n: int = 10):
        """輸出複雜度比較"""
        result = cls.compare(n)
        print(f"\n當 n = {n} 時，各複雜度的運算次數：")
        print("-" * 40)
        for name, count in sorted(result.items(), key=lambda x: x[1]):
            if count < float('inf'):
                print(f"  {name:10s}: {count:>15,.0f}")
            else:
                print(f"  {name:10s}: {'無限':>15s}")


class BigOAnalysis:
    """Big-O 分析工具"""
    
    @staticmethod
    def analyze_growth():
        """分析函數成長率"""
        print("\n常見複雜度的成長率：")
        print("-" * 60)
        print(f"{'n':<6} {'O(1)':<8} {'O(log n)':<10} {'O(n)':<8} {'O(n²)':<10} {'O(2ⁿ)':<15}")
        print("-" * 60)
        
        for n in [1, 2, 4, 8, 16, 32, 64, 128]:
            o1 = 1
            ologn = n.bit_length()
            on = n
            on2 = n * n
            o2n = min(2 ** n, 999999)
            
            print(f"{n:<6} {o1:<8} {ologn:<10} {on:<8} {on2:<10} {o2n:<15}")


def demo_constant_time():
    """O(1) 常數時間範例"""
    print("=" * 50)
    print("1. O(1) - 常數時間")
    print("=" * 50)
    
    arr = [random.randint(1, 1000) for _ in range(100)]
    
    result, t = measure_time(ComplexityAnalyzer.o_1)(arr)
    print(f"\n取第一個元素: {result}")
    print(f"執行時間: {t:.10f} 秒")


def demo_logarithmic_time():
    """O(log n) 對數時間範例"""
    print("\n" + "=" * 50)
    print("2. O(log n) - 對數時間 (二分搜尋)")
    print("=" * 50)
    
    arr = list(range(1, 1001))
    target = 753
    
    idx, t = measure_time(ComplexityAnalyzer.o_log_n)(arr, target)
    print(f"\n在 1-1000 中搜尋 {target}")
    print(f"找到位置: {idx}")
    print(f"執行時間: {t:.10f} 秒")
    print(f"理論最大比較次數: {arr[len(arr)-1].bit_length()}")


def demo_linear_time():
    """O(n) 線性時間範例"""
    print("\n" + "=" * 50)
    print("3. O(n) - 線性時間")
    print("=" * 50)
    
    arr = list(range(1, 101))
    target = 75
    
    idx, t = measure_time(ComplexityAnalyzer.o_n)(arr, target)
    print(f"\n在 1-100 中搜尋 {target}")
    print(f"找到位置: {idx}")
    print(f"執行時間: {t:.10f} 秒")


def demo_nlogn_time():
    """O(n log n) 範例"""
    print("\n" + "=" * 50)
    print("4. O(n log n) - 線性對數時間 (合併排序)")
    print("=" * 50)
    
    arr = [random.randint(1, 1000) for _ in range(100)]
    
    sorted_arr, t = measure_time(ComplexityAnalyzer.o_n_log_n)(arr)
    print(f"\n排序 {len(arr)} 個隨機數")
    print(f"前10個排序結果: {sorted_arr[:10]}")
    print(f"執行時間: {t:.10f} 秒")


def demo_quadratic_time():
    """O(n²) 範例"""
    print("\n" + "=" * 50)
    print("5. O(n²) - 平方時間 (氣泡排序)")
    print("=" * 50)
    
    arr = [random.randint(1, 1000) for _ in range(100)]
    
    sorted_arr, t = measure_time(ComplexityAnalyzer.o_n_2)(arr)
    print(f"\n排序 {len(arr)} 個隨機數")
    print(f"前10個排序結果: {sorted_arr[:10]}")
    print(f"執行時間: {t:.10f} 秒")


def demo_exponential_time():
    """O(2ⁿ) 範例"""
    print("\n" + "=" * 50)
    print("6. O(2ⁿ) - 指數時間 (費波那契)")
    print("=" * 50)
    
    for n in [5, 10, 15, 20]:
        start = time.perf_counter()
        result = ComplexityAnalyzer.o_2_n([n])
        t = time.perf_counter() - start
        print(f"F({n}) = {result}, 時間: {t:.6f} 秒")


def demo_complexity_comparison():
    """複雜度比較"""
    print("\n" + "=" * 50)
    print("7. 複雜度比較")
    print("=" * 50)
    
    BigOAnalysis.analyze_growth()
    ComplexityComparison.print_comparison(10)


def demo_practical_comparison():
    """實際執行時間比較"""
    print("\n" + "=" * 50)
    print("8. 實際執行時間比較")
    print("=" * 50)
    
    sizes = [100, 1000, 10000]
    
    print(f"\n{'n':<10} {'O(n)':<15} {'O(n log n)':<15} {'O(n²)':<15}")
    print("-" * 55)
    
    for n in sizes:
        arr = [random.randint(1, n) for _ in range(n)]
        
        _, t_linear = measure_time(ComplexityAnalyzer.o_n)(arr, -1)
        _, t_nlogn = measure_time(ComplexityAnalyzer.o_n_log_n)(arr)
        
        if n <= 1000:
            _, t_quad = measure_time(ComplexityAnalyzer.o_n_2)(arr)
        else:
            t_quad = float('nan')
        
        print(f"{n:<10} {t_linear:<15.8f} {t_nlogn:<15.8f} {t_quad:<15.8f}")


if __name__ == "__main__":
    print("時間複雜度 Python 範例")
    print("=" * 50)
    
    demo_constant_time()
    demo_logarithmic_time()
    demo_linear_time()
    demo_nlogn_time()
    demo_quadratic_time()
    demo_exponential_time()
    demo_complexity_comparison()
    demo_practical_comparison()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
