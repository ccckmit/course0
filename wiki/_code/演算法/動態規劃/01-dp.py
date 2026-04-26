#!/usr/bin/env python3
"""
動態規劃 (Dynamic Programming) -經典問題範例
展示最優子結構和重疊子問題
"""

def fibonacci_dp(n: int) -> int:
    """
    費波那契數列 - DP 解法
    
    時間複雜度: O(n)
    空間複雜度: O(n) 或 O(1)優化
    """
    if n <= 1:
        return n
    
    dp = [0] * (n + 1)
    dp[1] = 1
    
    for i in range(2, n + 1):
        dp[i] = dp[i-1] + dp[i-2]
    
    return dp[n]


def fibonacci_optimized(n: int) -> int:
    """
    空間優化的費波那契
    """
    if n <= 1:
        return n
    
    prev, curr = 0, 1
    for _ in range(2, n + 1):
        prev, curr = curr, prev + curr
    
    return curr


def coin_change_dp(amount: int, coins: list) -> int:
    """
    硬幣找零 - 最少硬幣數
    
    完全背包問題
    """
    dp = [float('inf')] * (amount + 1)
    dp[0] = 0
    
    for coin in coins:
        for x in range(coin, amount + 1):
            dp[x] = min(dp[x], dp[x - coin] + 1)
    
    return dp[amount] if dp[amount] != float('inf') else -1


def longest_common_subsequence(s1: str, s2: str) -> int:
    """
    最長公共子序列
    
    經典 DP 問題
    """
    m, n = len(s1), len(s2)
    dp = [[0] * (n + 1) for _ in range(m + 1)]
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s1[i-1] == s2[j-1]:
                dp[i][j] = dp[i-1][j-1] + 1
            else:
                dp[i][j] = max(dp[i-1][j], dp[i][j-1])
    
    return dp[m][n]


def edit_distance(s1: str, s2: str) -> int:
    """
    編輯距離 - Levenshtein Distance
    
    計算將 s1 轉換為 s2 的最少操作數
    操作: 插入、刪除、替換
    """
    m, n = len(s1), len(s2)
    dp = [[0] * (n + 1) for _ in range(m + 1)]
    
    for i in range(m + 1):
        dp[i][0] = i
    for j in range(n + 1):
        dp[0][j] = j
    
    for i in range(1, m + 1):
        for j in range(1, n + 1):
            if s1[i-1] == s2[j-1]:
                dp[i][j] = dp[i-1][j-1]
            else:
                dp[i][j] = 1 + min(dp[i-1][j],    # 刪除
                                   dp[i][j-1],    # 插入
                                   dp[i-1][j-1]) # 替換
    
    return dp[m][n]


def knapsack_01(values: list, weights: list, capacity: int) -> int:
    """
    0/1 背包問題
    
    每個物品只能選一次
    """
    n = len(values)
    dp = [[0] * (capacity + 1) for _ in range(n + 1)]
    
    for i in range(1, n + 1):
        for w in range(capacity + 1):
            if weights[i-1] <= w:
                dp[i][w] = max(dp[i-1][w], 
                              dp[i-1][w - weights[i-1]] + values[i-1])
            else:
                dp[i][w] = dp[i-1][w]
    
    return dp[n][capacity]


def longest_increasing_subsequence(arr: list) -> int:
    """
    最長遞增子序列
    
    時間複雜度: O(n^2) 或 O(n log n) 使用二分搜尋
    """
    n = len(arr)
    dp = [1] * n
    
    for i in range(1, n):
        for j in range(i):
            if arr[j] < arr[i] and dp[j] + 1 > dp[i]:
                dp[i] = dp[j] + 1
    
    return max(dp)


def matrix_chain_order(dims: list) -> int:
    """
    矩陣鏈乘法
    
    計算最少乘法次數
    dims: [d0, d1, d2, ..., dn] 表示 n 個矩陣
    """
    n = len(dims) - 1
    dp = [[0] * n for _ in range(n)]
    
    for length in range(2, n + 1):
        for i in range(n - length + 1):
            j = i + length - 1
            dp[i][j] = float('inf')
            
            for k in range(i, j):
                cost = dp[i][k] + dp[k+1][j] + dims[i] * dims[k+1] * dims[j+1]
                dp[i][j] = min(dp[i][j], cost)
    
    return dp[0][n-1]


def triangle_min_path(triangle: list) -> int:
    """
    三角形最短路徑
    
    從頂部到底部的最小路徑和
    """
    n = len(triangle)
    dp = triangle[-1][:]
    
    for i in range(n - 2, -1, -1):
        for j in range(i + 1):
            dp[j] = triangle[i][j] + min(dp[j], dp[j+1])
    
    return dp[0]


def subset_sum(nums: list, target: int) -> bool:
    """
    子集和問題
    
    判斷是否存在子集和等於 target
    """
    n = len(nums)
    dp = [[False] * (target + 1) for _ in range(n + 1)]
    
    for i in range(n + 1):
        dp[i][0] = True
    
    for i in range(1, n + 1):
        for s in range(1, target + 1):
            if nums[i-1] > s:
                dp[i][s] = dp[i-1][s]
            else:
                dp[i][s] = dp[i-1][s] or dp[i-1][s - nums[i-1]]
    
    return dp[n][target]


if __name__ == "__main__":
    # 測試費波那契
    print(f"fib(10) = {fibonacci_dp(10)}")
    print(f"fib(10) optimized = {fibonacci_optimized(10)}")
    
    # 測試硬幣找零
    amount = 11
    coins = [1, 2, 5]
    print(f"硬幣找零 {amount}: {coin_change_dp(amount, coins)}")
    
    # 測試 LCS
    s1, s2 = "AGGTAB", "GXTXAYB"
    print(f"LCS('{s1}', '{s2}') = {longest_common_subsequence(s1, s2)}")
    
    # 測試編輯距離
    s1, s2 = "sunday", "saturday"
    print(f"編輯距離('{s1}', '{s2}') = {edit_distance(s1, s2)}")
    
    # 測試 0/1 背包
    values = [60, 100, 120]
    weights = [10, 20, 30]
    capacity = 50
    print(f"背包最大價值: {knapsack_01(values, weights, capacity)}")
    
    # 測試 LIS
    arr = [10, 9, 2, 5, 3, 7, 101, 18]
    print(f"LIS: {longest_increasing_subsequence(arr)}")
