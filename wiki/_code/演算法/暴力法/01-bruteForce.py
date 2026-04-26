#!/usr/bin/env python3
"""
暴力法 (Brute Force) - 演算法範例
展示最基本的暴力搜尋方法
"""

def brute_force_search(text: str, pattern: str) -> int:
    """
    暴力字串搜尋
    在 text 中尋找 pattern 第一次出現的位置
    
    時間複雜度: O(n * m)
    """
    n, m = len(text), len(pattern)
    for i in range(n - m + 1):
        if text[i:i+m] == pattern:
            return i
    return -1


def brute_force_max(arr: list) -> tuple:
    """
    暴力找最大值
    簡單遍歷所有元素
    """
    if not arr:
        return None, None
    max_val = arr[0]
    max_idx = 0
    for i, val in enumerate(arr):
        if val > max_val:
            max_val = val
            max_idx = i
    return max_val, max_idx


def brute_force_two_sum(nums: list, target: int) -> list:
    """
    暴力兩數之和
    找出所有兩數之和等於 target 的組合
    
    時間複雜度: O(n^2)
    """
    result = []
    n = len(nums)
    for i in range(n):
        for j in range(i + 1, n):
            if nums[i] + nums[j] == target:
                result.append((i, j))
    return result


def brute_force_substring(text: str) -> list:
    """
    列舉所有子字串
    """
    n = len(text)
    substrings = []
    for i in range(n):
        for j in range(i + 1, n + 1):
            substrings.append(text[i:j])
    return substrings


def brute_force_subset(items: list) -> list:
    """
    列舉所有子集
    使用位元遮罩
    
    時間複雜度: O(2^n * n)
    """
    n = len(items)
    subsets = []
    for mask in range(1 << n):
        subset = []
        for i in range(n):
            if mask & (1 << i):
                subset.append(items[i])
        subsets.append(subset)
    return subsets


def brute_force_permutation(items: list) -> list:
    """
    列舉所有排列
    
    時間複雜度: O(n!)
    """
    if len(items) == 1:
        return [items]
    
    result = []
    for i, item in enumerate(items):
        rest = items[:i] + items[i+1:]
        for perm in brute_force_permutation(rest):
            result.append([item] + perm)
    return result


def brute_force_all_subsets_sum(nums: list) -> dict:
    """
    列舉所有子集和
    """
    n = len(nums)
    sums = {0: [[]]}  # key: sum, value: list of subsets
    
    for num in nums:
        new_sums = {}
        for s, subsets in sums.items():
            # 不選 num
            if s not in new_sums:
                new_sums[s] = []
            new_sums[s].extend(subsets)
            
            # 選 num
            new_s = s + num
            new_subsets = [subset + [num] for subset in subsets]
            if new_s not in new_sums:
                new_sums[new_s] = []
            new_sums[new_s].extend(new_subsets)
        sums = new_sums
    
    return sums


if __name__ == "__main__":
    # 測試暴力搜尋
    text = "Hello, World!"
    pattern = "World"
    result = brute_force_search(text, pattern)
    print(f"搜尋 '{pattern}' 在 '{text}': 位置 = {result}")
    
    # 測試找最大值
    arr = [3, 1, 4, 1, 5, 9, 2, 6]
    max_val, max_idx = brute_force_max(arr)
    print(f"最大值: {max_val} at index {max_idx}")
    
    # 測試兩數之和
    nums = [2, 7, 11, 15]
    target = 9
    result = brute_force_two_sum(nums, target)
    print(f"兩數之和 = {target}: {result}")
    
    # 測試子集
    items = ['a', 'b', 'c']
    result = brute_force_subset(items)
    print(f"子集數量: {len(result)}")
    print(f"子集: {result}")
    
    # 測試排列
    items = [1, 2, 3]
    result = brute_force_permutation(items)
    print(f"排列: {result}")
