#!/usr/bin/env python3
"""
貪婪法 (Greedy) - 活動選擇問題範例
展示貪心選擇策略
"""

def activity_selection(starts: list, finishes: list) -> list:
    """
    活動選擇問題
    選擇最大數量的不相交活動
    
    貪心策略: 總是選擇下一個結束最早的活動
    
    時間複雜度: O(n log n) 由於排序
    """
    # 按結束時間排序
    activities = sorted(zip(starts, finishes), key=lambda x: x[1])
    
    selected = []
    current_end = 0
    
    for start, finish in activities:
        if start >= current_end:
            selected.append((start, finish))
            current_end = finish
    
    return selected


def coin_change_greedy(amount: int, coins: list) -> tuple:
    """
    貪心硬幣找零
    總是選擇面額最大的硬幣
    
    注意: 貪心法不一定最優
    """
    coins = sorted(coins, reverse=True)
    result = []
    remaining = amount
    
    for coin in coins:
        count = remaining // coin
        if count > 0:
            result.append((coin, count))
            remaining -= count * coin
    
    if remaining == 0:
        return result, amount
    else:
        return None, remaining  # 無法找零


def fractional_knapsack(values: list, weights: list, capacity: int) -> tuple:
    """
    分數背包問題
    可以取部分物品
    
    貪心策略: 總是選擇價值/重量比最高的物品
    """
    # 計算價值/重量比並排序
    items = sorted(zip(values, weights), 
                  key=lambda x: x[0]/x[1], 
                  reverse=True)
    
    total_value = 0
    taken = []
    remaining = capacity
    
    for value, weight in items:
        if remaining == 0:
            break
        
        if weight <= remaining:
            taken.append((value, weight, 1.0))
            total_value += value
            remaining -= weight
        else:
            fraction = remaining / weight
            taken.append((value, weight, fraction))
            total_value += value * fraction
            remaining = 0
    
    return total_value, taken


def huffman_coding(freq: dict) -> dict:
    """
    霍夫曼編碼 - 貪心壓縮
    構建最優前綴碼
    
    時間複雜度: O(n log n)
    """
    import heapq
    
    # 建立優先隊列
    heap = [(freq[char], char) for char in freq]
    heapq.heapify(heap)
    
    if len(heap) == 1:
        return {heap[0][1]: '0'}
    
    codes = {}
    
    while len(heap) > 1:
        # 取最小的兩個節點
        f1, c1 = heapq.heappop(heap)
        f2, c2 = heapq.heappop(heap)
        
        # 對現有節點添加前綴
        if isinstance(c1, tuple):
            for char in c1:
                codes[char] = '0' + codes[char]
        else:
            codes[c1] = '0'
            
        if isinstance(c2, tuple):
            for char in c2:
                codes[char] = '1' + codes[char]
        else:
            codes[c2] = '1'
        
        # 合併節點放回隊列
        new_node = (f1 + f2, (c1, c2))
        heapq.heappush(heap, new_node)
    
    return codes


def minimum_spanning_tree_prim(adj: dict) -> tuple:
    """
    Prim 演算法 - 最小生成樹
    貪心選擇最小的邊
    
    時間複雜度: O(V^2) 或 O(E log V)
    """
    if not adj:
        return [], 0
    
    vertices = list(adj.keys())
    in_mst = {vertices[0]}
    mst_edges = []
    total_weight = 0
    
    while len(in_mst) < len(vertices):
        min_edge = None
        min_weight = float('inf')
        
        for v in in_mst:
            for neighbor, weight in adj[v]:
                if neighbor not in in_mst and weight < min_weight:
                    min_edge = (v, neighbor)
                    min_weight = weight
        
        if min_edge is None:
            break
            
        mst_edges.append(min_edge)
        total_weight += min_weight
        in_mst.add(min_edge[1])
    
    return mst_edges, total_weight


def dijkstra_shortest_path(graph: dict, start: str) -> dict:
    """
    Dijkstra 最短路徑
    貪心選擇最近的未訪問節點
    
    時間複雜度: O(V^2) 或 O(E log V)
    """
    import heapq
    
    dist = {v: float('inf') for v in graph}
    dist[start] = 0
    visited = set()
    pq = [(0, start)]
    
    while pq:
        d, u = heapq.heappop(pq)
        
        if u in visited:
            continue
        visited.add(u)
        
        for v, weight in graph[u]:
            if v not in visited and dist[u] + weight < dist[v]:
                dist[v] = dist[u] + weight
                heapq.heappush(pq, (dist[v], v))
    
    return dist


if __name__ == "__main__":
    # 測試活動選擇
    starts = [1, 3, 0, 5, 3, 5, 6, 8, 8, 2]
    finishes = [4, 5, 6, 7, 9, 9, 10, 11, 12, 14]
    result = activity_selection(starts, finishes)
    print(f"活動選擇: {result}")
    
    # 測試硬幣找零
    amount = 63
    coins = [25, 10, 5, 1]
    result, remaining = coin_change_greedy(amount, coins)
    print(f"硬幣找零 {amount}: {result}, 剩餘: {remaining}")
    
    # 測試分數背包
    values = [60, 100, 120]
    weights = [10, 20, 30]
    capacity = 50
    total, taken = fractional_knapsack(values, weights, capacity)
    print(f"分數背包總價值: {total}")
    print(f"取用: {taken}")
    
    # 測試霍夫曼
    freq = {'a': 45, 'b': 13, 'c': 12, 'd': 16, 'e': 9, 'f': 5}
    codes = huffman_coding(freq)
    print(f"霍夫曼編碼: {codes}")
