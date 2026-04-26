#!/usr/bin/env python3
"""
排序演算法 -經典排序方法範例
"""

def bubble_sort(arr: list) -> list:
    """
    氣泡排序
    
    時間複雜度: O(n^2)
    空間複雜度: O(1)
    """
    n = len(arr)
    arr = arr[:]
    
    for i in range(n):
        swapped = False
        for j in range(0, n - i - 1):
            if arr[j] > arr[j + 1]:
                arr[j], arr[j + 1] = arr[j + 1], arr[j]
                swapped = True
        if not swapped:
            break
    
    return arr


def insertion_sort(arr: list) -> list:
    """
    插入排序
    
    時間複雜度: O(n^2) 平均/最差, O(n) 最佳
    空間複雜度: O(1)
    """
    arr = arr[:]
    
    for i in range(1, len(arr)):
        key = arr[i]
        j = i - 1
        
        while j >= 0 and arr[j] > key:
            arr[j + 1] = arr[j]
            j -= 1
        
        arr[j + 1] = key
    
    return arr


def selection_sort(arr: list) -> list:
    """
    選擇排序
    
    時間複雜度: O(n^2)
    空間複雜度: O(1)
    """
    arr = arr[:]
    n = len(arr)
    
    for i in range(n):
        min_idx = i
        for j in range(i + 1, n):
            if arr[j] < arr[min_idx]:
                min_idx = j
        arr[i], arr[min_idx] = arr[min_idx], arr[i]
    
    return arr


def merge_sort(arr: list) -> list:
    """
    合併排序
    
    時間複雜度: O(n log n)
    空間複雜度: O(n)
    """
    if len(arr) <= 1:
        return arr[:]
    
    mid = len(arr) // 2
    left = merge_sort(arr[:mid])
    right = merge_sort(arr[mid:])
    
    return merge(left, right)


def merge(left: list, right: list) -> list:
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


def quick_sort(arr: list) -> list:
    """
    快速排序
    
    時間複雜度: O(n log n) 平均, O(n^2) 最差
    空間複雜度: O(log n)
    """
    if len(arr) <= 1:
        return arr[:]
    
    pivot = arr[len(arr) // 2]
    left = [x for x in arr if x < pivot]
    middle = [x for x in arr if x == pivot]
    right = [x for x in arr if x > pivot]
    
    return quick_sort(left) + middle + quick_sort(right)


def heap_sort(arr: list) -> list:
    """
    堆排序
    
    時間複雜度: O(n log n)
    空間複雜度: O(1)
    """
    arr = arr[:]
    n = len(arr)
    
    def heapify(arr: list, n: int, i: int):
        largest = i
        left = 2 * i + 1
        right = 2 * i + 2
        
        if left < n and arr[left] > arr[largest]:
            largest = left
        if right < n and arr[right] > arr[largest]:
            largest = right
        
        if largest != i:
            arr[i], arr[largest] = arr[largest], arr[i]
            heapify(arr, n, largest)
    
    # 建立最大堆
    for i in range(n // 2 - 1, -1, -1):
        heapify(arr, n, i)
    
    # 提取元素
    for i in range(n - 1, 0, -1):
        arr[0], arr[i] = arr[i], arr[0]
        heapify(arr, i, 0)
    
    return arr


def counting_sort(arr: list) -> list:
    """
    計數排序
    
    時間複雜度: O(n + k) 其中 k 是範圍
    空間複雜度: O(k)
    穩定: 是
    """
    if not arr:
        return []
    
    max_val = max(arr)
    min_val = min(arr)
    range_size = max_val - min_val + 1
    
    count = [0] * range_size
    for num in arr:
        count[num - min_val] += 1
    
    for i in range(1, range_size):
        count[i] += count[i - 1]
    
    result = [0] * len(arr)
    for num in reversed(arr):
        result[count[num - min_val] - 1] = num
        count[num - min_val] -= 1
    
    return result


def radix_sort(arr: list) -> list:
    """
    基數排序
    
    時間複雜度: O(nk) 其中 k 是位數
    空間複雜度: O(n + k)
    """
    if not arr:
        return []
    
    max_val = max(arr)
    exponent = 1
    
    while max_val // exponent > 0:
        buckets = [[] for _ in range(10)]
        
        for num in arr:
            digit = (num // exponent) % 10
            buckets[digit].append(num)
        
        arr = [num for bucket in buckets for num in bucket]
        exponent *= 10
    
    return arr


def bucket_sort(arr: list) -> list:
    """
    桶排序
    
    時間複雜度: O(n + k) 平均
    """
    if not arr:
        return []
    
    min_val, max_val = min(arr), max(arr)
    n = len(arr)
    bucket_count = n
    
    if min_val == max_val:
        return arr[:]
    
    buckets = [[] for _ in range(bucket_count)]
    range_size = (max_val - min_val) / bucket_count
    
    for num in arr:
        idx = int((num - min_val) / range_size)
        if idx >= bucket_count:
            idx = bucket_count - 1
        buckets[idx].append(num)
    
    result = []
    for bucket in buckets:
        if bucket:
            bucket.sort()
            result.extend(bucket)
    
    return result


if __name__ == "__main__":
    import random
    
    # 測試資料
    test_arrays = [
        [64, 34, 25, 12, 22, 11, 90],
        [5, 2, 9, 1, 7, 6, 3],
        [3, 3, 3, 1, 1, 2, 2],
        list(range(100, 0, -1)),
    ]
    
    sorters = [
        ("氣泡排序", bubble_sort),
        ("插入排序", insertion_sort),
        ("選擇排序", selection_sort),
        ("合併排序", merge_sort),
        ("快速排序", quick_sort),
        ("堆排序", heap_sort),
        ("計數排序", counting_sort),
        ("基數排序", radix_sort),
        ("桶排序", bucket_sort),
    ]
    
    print("排序演算法測試:")
    for name, sorter in sorters:
        for i, arr in enumerate(test_arrays):
            result = sorter(arr[:])
            sorted_arr = sorted(arr)
            status = "✓" if result == sorted_arr else "✗"
            print(f"{name} 測試{i+1}: {status}")
    
    # 效能測試
    import time
    
    large_arr = list(range(10000))
    import random
    random.shuffle(large_arr)
    
    print("\n效能測試 (10000 元素):")
    for name, sorter in sorters[:5]:
        start = time.time()
        sorter(large_arr[:])
        elapsed = time.time() - start
        print(f"{name}: {elapsed:.4f}秒")
