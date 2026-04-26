#!/usr/bin/env python3
"""
搜尋演算法 -經典搜尋方法範例
"""

def linear_search(arr: list, target) -> int:
    """
    線性搜尋
    
    時間複雜度: O(n)
    """
    for i, val in enumerate(arr):
        if val == target:
            return i
    return -1


def binary_search(arr: list, target) -> int:
    """
    二分搜尋
    
    時間複雜度: O(log n)
    前提: 已排序陣列
    """
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


def binary_search_left(arr: list, target) -> int:
    """
    二分搜尋 - 左邊界
    
    找出第一個 >= target 的位置
    """
    left, right = 0, len(arr)
    
    while left < right:
        mid = (left + right) // 2
        if arr[mid] < target:
            left = mid + 1
        else:
            right = mid
    
    return left


def binary_search_right(arr: list, target) -> int:
    """
    二分搜尋 - 右邊界
    
    找出第一個 > target 的位置
    """
    left, right = 0, len(arr)
    
    while left < right:
        mid = (left + right) // 2
        if arr[mid] <= target:
            left = mid + 1
        else:
            right = mid
    
    return left


def interpolation_search(arr: list, target) -> int:
    """
    插值搜尋
    
    時間複雜度: O(log log n) 平均
    前提: 已排序均勻分布陣列
    """
    left, right = 0, len(arr) - 1
    
    while left <= right and target >= arr[left] and target <= arr[right]:
        if left == right:
            if arr[left] == target:
                return left
            return -1
        
        pos = left + ((target - arr[left]) * (right - left) // 
                     (arr[right] - arr[left]))
        
        if arr[pos] == target:
            return pos
        elif arr[pos] < target:
            left = pos + 1
        else:
            right = pos - 1
    
    return -1


def jump_search(arr: list, target) -> int:
    """
    跳躍搜尋
    
    時間複雜度: O(√n)
    前提: 已排序陣列
    """
    import math
    n = len(arr)
    step = int(math.sqrt(n))
    
    prev = 0
    while arr[min(step, n) - 1] < target:
        prev = step
        step += int(math.sqrt(n))
        if prev >= n:
            return -1
    
    while arr[prev] < target:
        prev += 1
        if prev == min(step, n):
            return -1
    
    if arr[prev] == target:
        return prev
    
    return -1


def exponential_search(arr: list, target) -> int:
    """
    指數搜尋
    
    時間複雜度: O(log n)
    前提: 已排序陣列
    """
    import math
    
    if arr[0] == target:
        return 0
    
    n = len(arr)
    bound = 1
    
    while bound < n and arr[bound] < target:
        bound *= 2
    
    left, right = bound // 2, min(bound, n - 1)
    
    while left <= right:
        mid = (left + right) // 2
        
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            left = mid + 1
        else:
            right = mid - 1
    
    return -1


def ternary_search(arr: list, target) -> int:
    """
    三元搜尋
    
    時間複雜度: O(log n)
    前提: 已排序單峰陣列
    """
    left, right = 0, len(arr) - 1
    
    while left <= right:
        mid1 = left + (right - left) // 3
        mid2 = right - (right - left) // 3
        
        if arr[mid1] == target:
            return mid1
        if arr[mid2] == target:
            return mid2
        
        if arr[mid1] > target:
            right = mid1 - 1
        elif arr[mid2] < target:
            left = mid2 + 1
        else:
            left = mid1 + 1
            right = mid2 - 1
    
    return -1


def fibonacci_search(arr: list, target) -> int:
    """
    費波那契搜尋
    
    時間複雜度: O(log n)
    前提: 已排序陣列
    """
    n = len(arr)
    
    fib_m2 = 0
    fib_m1 = 1
    fib = fib_m1 + fib_m2
    
    while fib < n:
        fib_m2 = fib_m1
        fib_m1 = fib
        fib = fib_m1 + fib_m2
    
    offset = -1
    
    while fib > 1:
        i = min(offset + fib_m2, n - 1)
        
        if arr[i] < target:
            fib = fib_m1
            fib_m1 = fib_m2
            fib_m2 = fib - fib_m1
            offset = i
        elif arr[i] > target:
            fib = fib_m2
            fib_m1 = fib_m1 - fib_m2
            fib_m2 = fib - fib_m1
        else:
            return i
    
    if fib_m1 and arr[offset + 1] == target:
        return offset + 1
    
    return -1


class HashTable:
    """
    雜湊表實現
    """
    
    def __init__(self, size: int = 100):
        self.size = size
        self.table = [[] for _ in range(size)]
    
    def _hash(self, key) -> int:
        """簡單雜湊函數"""
        if isinstance(key, str):
            key = sum(ord(c) for c in key)
        return key % self.size
    
    def insert(self, key, value):
        """插入鍵值對"""
        idx = self._hash(key)
        bucket = self.table[idx]
        
        for i, (k, v) in enumerate(bucket):
            if k == key:
                bucket[i] = (key, value)
                return
        
        bucket.append((key, value))
    
    def search(self, key):
        """搜尋"""
        idx = self._hash(key)
        bucket = self.table[idx]
        
        for k, v in bucket:
            if k == key:
                return v
        
        return None
    
    def delete(self, key):
        """刪除"""
        idx = self._hash(key)
        bucket = self.table[idx]
        
        for i, (k, v) in enumerate(bucket):
            if k == key:
                del bucket[i]
                return True
        
        return False


if __name__ == "__main__":
    # 測試資料
    arr = [1, 3, 5, 7, 9, 11, 13, 15, 17, 19]
    
    print("搜尋演算法測試:")
    print(f"陣列: {arr}")
    print(f"線性搜尋 9: {linear_search(arr, 9)}")
    print(f"二分搜尋 9: {binary_search(arr, 9)}")
    print(f"插值搜尋 9: {interpolation_search(arr, 9)}")
    print(f"跳躍搜尋 9: {jump_search(arr, 9)}")
    print(f"指數搜尋 9: {exponential_search(arr, 9)}")
    print(f"費波那契搜尋 9: {fibonacci_search(arr, 9)}")
    
    # 測試雜湊表
    print("\n雜湊表測試:")
    ht = HashTable()
    ht.insert("name", "Alice")
    ht.insert("age", 25)
    ht.insert("city", "Taipei")
    
    print(f"搜尋 name: {ht.search('name')}")
    print(f"搜尋 age: {ht.search('age')}")
    print(f"搜尋 city: {ht.search('city')}")
    print(f"搜尋 unknown: {ht.search('unknown')}")
