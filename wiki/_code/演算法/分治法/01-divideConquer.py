#!/usr/bin/env python3
"""
分割擊破法 (Divide and Conquer) -經典問題範例
展示分治策略
"""

def merge_sort(arr: list) -> list:
    """
    合併排序
    
    時間複雜度: O(n log n)
    空間複雜度: O(n)
    """
    if len(arr) <= 1:
        return arr
    
    mid = len(arr) // 2
    left = merge_sort(arr[:mid])
    right = merge_sort(arr[mid:])
    
    return merge(left, right)


def merge(left: list, right: list) -> list:
    """合併兩個已排序陣列"""
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


def quick_sort(arr: list, low: int = None, high: int = None) -> list:
    """
    快速排序
    
    時間複雜度: 平均 O(n log n)，最差 O(n^2)
    空間複雜度: O(log n)
    """
    if low is None:
        low = 0
    if high is None:
        high = len(arr) - 1
    
    if low < high:
        pivot_idx = partition(arr, low, high)
        quick_sort(arr, low, pivot_idx - 1)
        quick_sort(arr, pivot_idx + 1, high)
    
    return arr


def partition(arr: list, low: int, high: int) -> int:
    """分區操作"""
    pivot = arr[high]
    i = low - 1
    
    for j in range(low, high):
        if arr[j] <= pivot:
            i += 1
            arr[i], arr[j] = arr[j], arr[i]
    
    arr[i + 1], arr[high] = arr[high], arr[i + 1]
    return i + 1


def binary_search(arr: list, target: int) -> int:
    """
    二分搜尋 - 分治經典
    
    時間複雜度: O(log n)
    """
    return binary_search_recursive(arr, target, 0, len(arr) - 1)


def binary_search_recursive(arr: list, target: int, low: int, high: int) -> int:
    if low > high:
        return -1
    
    mid = (low + high) // 2
    
    if arr[mid] == target:
        return mid
    elif arr[mid] < target:
        return binary_search_recursive(arr, target, mid + 1, high)
    else:
        return binary_search_recursive(arr, target, low, mid - 1)


def power(base: float, exp: int) -> float:
    """
    快速幂 - 分治計算 base^exp
    
    時間複雜度: O(log n)
    """
    if exp == 0:
        return 1
    if exp < 0:
        return 1 / power(base, -exp)
    
    half = power(base, exp // 2)
    
    if exp % 2 == 0:
        return half * half
    else:
        return base * half * half


def closest_pair(points: list) -> tuple:
    """
    最近點對問題
    
    時間複雜度: O(n log n)
    
    返回: (點1, 點2, 最小距離)
    """
    points = sorted(points, key=lambda p: p[0])
    return closest_pair_recursive(points)


def closest_pair_recursive(points: list) -> tuple:
    n = len(points)
    
    if n <= 3:
        return brute_force_closest(points)
    
    mid = n // 2
    mid_x = points[mid][0]
    
    left = points[:mid]
    right = points[mid:]
    
    d_left = closest_pair_recursive(left)
    d_right = closest_pair_recursive(right)
    d = min(d_left, d_right)
    
    strip = [p for p in points if abs(p[0] - mid_x) < d[2]]
    
    return min(d, closest_pair_strip(strip, d[2]))


def brute_force_closest(points: list) -> tuple:
    """暴力法找最近點對"""
    n = len(points)
    if n < 2:
        return (None, None, float('inf'))
    
    min_dist = float('inf')
    closest = (None, None)
    
    for i in range(n):
        for j in range(i + 1, n):
            d = ((points[i][0] - points[j][0])**2 + 
                 (points[i][1] - points[j][1])**2)**0.5
            if d < min_dist:
                min_dist = d
                closest = (points[i], points[j])
    
    return (*closest, min_dist)


def closest_pair_strip(strip: list, d: float) -> tuple:
    """在條帶中找最近點對"""
    strip = sorted(strip, key=lambda p: p[1])
    n = len(strip)
    min_dist = d
    closest = (None, None)
    
    for i in range(n):
        for j in range(i + 1, min(i + 8, n)):
            d = ((strip[i][0] - strip[j][0])**2 + 
                 (strip[i][1] - strip[j][1])**2)**0.5
            if d < min_dist:
                min_dist = d
                closest = (strip[i], strip[j])
    
    return (*closest, min_dist)


def find_kth_smallest(arr: list, k: int) -> int:
    """
    尋找第 k 小的元素
    
    使用快速選擇演算法
    平均時間複雜度: O(n)
    """
    return quick_select(arr, 0, len(arr) - 1, k - 1)


def quick_select(arr: list, low: int, high: int, k: int) -> int:
    if low == high:
        return arr[low]
    
    pivot_idx = partition(arr, low, high)
    
    if k == pivot_idx:
        return arr[k]
    elif k < pivot_idx:
        return quick_select(arr, low, pivot_idx - 1, k)
    else:
        return quick_select(arr, pivot_idx + 1, high, k)


def matrix_multiply_strassen(A: list, B: list) -> list:
    """
    Strassen 矩陣乘法
    
    時間複雜度: O(n^2.807)
    """
    n = len(A)
    
    if n == 1:
        return [[A[0][0] * B[0][0]]]
    
    mid = n // 2
    
    A11 = [[A[i][j] for j in range(mid)] for i in range(mid)]
    A12 = [[A[i][j] for j in range(mid, n)] for i in range(mid)]
    A21 = [[A[i][j] for j in range(mid)] for i in range(mid, n)]
    A22 = [[A[i][j] for j in range(mid, n)] for i in range(mid, n)]
    
    B11 = [[B[i][j] for j in range(mid)] for i in range(mid)]
    B12 = [[B[i][j] for j in range(mid, n)] for i in range(mid)]
    B21 = [[B[i][j] for j in range(mid)] for i in range(mid, n)]
    B22 = [[B[i][j] for j in range(mid, n)] for i in range(mid, n)]
    
    M1 = matrix_multiply_strassen(
        matrix_add(A11, A22), matrix_add(B11, B22))
    M2 = matrix_multiply_strassen(
        matrix_add(A21, A22), B11)
    M3 = matrix_multiply_strassen(
        A11, matrix_subtract(B12, B22))
    M4 = matrix_multiply_strassen(
        A22, matrix_subtract(B21, B11))
    M5 = matrix_multiply_strassen(
        matrix_add(A11, A12), B22)
    M6 = matrix_multiply_strassen(
        matrix_subtract(A21, A11), matrix_add(B11, B12))
    M7 = matrix_multiply_strassen(
        matrix_subtract(A12, A22), matrix_add(B21, B22))
    
    C11 = matrix_add(matrix_subtract(matrix_add(M1, M4), M5), M7)
    C12 = matrix_add(M3, M5)
    C21 = matrix_add(M2, M4)
    C22 = matrix_add(matrix_subtract(matrix_add(M1, M3), M2), M6)
    
    return matrix_combine(C11, C12, C21, C22)


def matrix_add(A: list, B: list) -> list:
    return [[A[i][j] + B[i][j] for j in range(len(A))] for i in range(len(A))]


def matrix_subtract(A: list, B: list) -> list:
    return [[A[i][j] - B[i][j] for j in range(len(A))] for i in range(len(A))]


def matrix_combine(A11, A12, A21, A22) -> list:
    n = len(A11)
    result = [[0] * (2*n) for _ in range(2*n)]
    for i in range(n):
        for j in range(n):
            result[i][j] = A11[i][j]
            result[i][j+n] = A12[i][j]
            result[i+n][j] = A21[i][j]
            result[i+n][j+n] = A22[i][j]
    return result


if __name__ == "__main__":
    # 測試合併排序
    arr = [64, 34, 25, 12, 22, 11, 90]
    print(f"合併排序: {merge_sort(arr)}")
    
    # 測試快速排序
    arr = [64, 34, 25, 12, 22, 11, 90]
    print(f"快速排序: {quick_sort(arr[:])}")
    
    # 測試二分搜尋
    arr = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    print(f"二分搜尋 5: {binary_search(arr, 5)}")
    
    # 測試快速冪
    print(f"2^10 = {power(2, 10)}")
    
    # 測試最近點對
    points = [(2, 3), (12, 30), (40, 50), (5, 1), (12, 10), (3, 4)]
    result = closest_pair(points)
    print(f"最近點對: {result[0]} 和 {result[1]}, 距離: {result[2]:.2f}")
    
    # 測試找第 k 小
    arr = [7, 10, 4, 3, 20, 15]
    print(f"第3小: {find_kth_smallest(arr, 3)}")
