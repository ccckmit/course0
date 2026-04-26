#!/usr/bin/env python3
"""
圖形演算法 -經典圖演算法範例
"""

from collections import deque, defaultdict
import heapq


def bfs(graph: dict, start: str) -> list:
    """
    廣度優先搜尋 (BFS)
    
    時間複雜度: O(V + E)
    空間複雜度: O(V)
    """
    visited = set([start])
    queue = deque([start])
    result = []
    
    while queue:
        vertex = queue.popleft()
        result.append(vertex)
        
        for neighbor in graph.get(vertex, []):
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append(neighbor)
    
    return result


def dfs(graph: dict, start: str) -> list:
    """
    深度優先搜尋 (DFS)
    
    時間複雜度: O(V + E)
    """
    visited = set()
    result = []
    
    def dfs_helper(vertex: str):
        visited.add(vertex)
        result.append(vertex)
        
        for neighbor in graph.get(vertex, []):
            if neighbor not in visited:
                dfs_helper(neighbor)
    
    dfs_helper(start)
    return result


def dfs_iterative(graph: dict, start: str) -> list:
    """DFS 迭代版本"""
    visited = set()
    stack = [start]
    result = []
    
    while stack:
        vertex = stack.pop()
        if vertex not in visited:
            visited.add(vertex)
            result.append(vertex)
            
            for neighbor in graph.get(vertex, []):
                if neighbor not in visited:
                    stack.append(neighbor)
    
    return result


def dijkstra(graph: dict, start: str) -> dict:
    """
    Dijkstra 最短路徑
    
    時間複雜度: O((V + E) log V)
    適用: 正權重圖
    """
    dist = {v: float('inf') for v in graph}
    dist[start] = 0
    pq = [(0, start)]
    
    while pq:
        d, u = heapq.heappop(pq)
        
        if d > dist[u]:
            continue
        
        for v, weight in graph[u]:
            new_dist = dist[u] + weight
            if new_dist < dist[v]:
                dist[v] = new_dist
                heapq.heappush(pq, (new_dist, v))
    
    return dist


def bellman_ford(graph: list, start: str, n: int) -> dict:
    """
    Bellman-Ford 最短路徑
    
    時間複雜度: O(VE)
    適用: 可有負權重
    """
    dist = {i: float('inf') for i in range(n)}
    dist[start] = 0
    
    for _ in range(n - 1):
        for u, v, w in graph:
            if dist[u] != float('inf') and dist[u] + w < dist[v]:
                dist[v] = dist[u] + w
    
    # 檢測負權重環
    for u, v, w in graph:
        if dist[u] != float('inf') and dist[u] + w < dist[v]:
            return None  # 有負權重環
    
    return dist


def floyd_warshall(matrix: list) -> list:
    """
    Floyd-Warshall 全點對最短路徑
    
    時間複雜度: O(V^3)
    """
    n = len(matrix)
    dist = [row[:] for row in matrix]
    
    for k in range(n):
        for i in range(n):
            for j in range(n):
                if dist[i][k] + dist[k][j] < dist[i][j]:
                    dist[i][j] = dist[i][k] + dist[k][j]
    
    return dist


def topological_sort(graph: dict) -> list:
    """
    拓撲排序
    
    時間複雜度: O(V + E)
    """
    in_degree = defaultdict(int)
    
    for u in graph:
        for v in graph[u]:
            in_degree[v] += 1
    
    queue = deque([v for v in graph if in_degree[v] == 0])
    result = []
    
    while queue:
        u = queue.popleft()
        result.append(u)
        
        for v in graph[u]:
            in_degree[v] -= 1
            if in_degree[v] == 0:
                queue.append(v)
    
    return result if len(result) == len(graph) else []


def prim_mst(graph: dict) -> tuple:
    """
    Prim 最小生成樹
    
    時間複雜度: O((V + E) log V)
    """
    if not graph:
        return [], 0
    
    vertices = list(graph.keys())
    in_mst = set([vertices[0]])
    mst_edges = []
    total_weight = 0
    
    while len(in_mst) < len(vertices):
        min_edge = None
        min_weight = float('inf')
        
        for u in in_mst:
            for v, w in graph[u]:
                if v not in in_mst and w < min_weight:
                    min_edge = (u, v)
                    min_weight = w
        
        if min_edge is None:
            break
        
        mst_edges.append(min_edge)
        total_weight += min_weight
        in_mst.add(min_edge[1])
    
    return mst_edges, total_weight


def kruskal_mst(edges: list, n: int) -> tuple:
    """
    Kruskal 最小生成樹
    
    時間複雜度: O(E log V)
    """
    def find(parent: list, x: int) -> int:
        if parent[x] != x:
            parent[x] = find(parent, parent[x])
        return parent[x]
    
    def union(parent: list, rank: list, x: int, y: int):
        px, py = find(parent, x), find(parent, y)
        if px == py:
            return False
        if rank[px] < rank[py]:
            px, py = py, px
        parent[py] = px
        if rank[px] == rank[py]:
            rank[px] += 1
        return True
    
    edges.sort(key=lambda x: x[2])
    parent = list(range(n))
    rank = [0] * n
    
    mst = []
    total_weight = 0
    
    for u, v, w in edges:
        if union(parent, rank, u, v):
            mst.append((u, v, w))
            total_weight += w
            if len(mst) == n - 1:
                break
    
    return mst, total_weight


def bfs_shortest_path(graph: dict, start: str, end: str) -> tuple:
    """
    BFS 最短路徑
    
    適用: 未加權圖
    """
    if start == end:
        return [start], 0
    
    visited = {start: (None, 0)}
    queue = deque([start])
    
    while queue:
        u = queue.popleft()
        
        for v in graph.get(u, []):
            if v not in visited:
                visited[v] = (u, visited[u][1] + 1)
                queue.append(v)
                
                if v == end:
                    # 回溯路徑
                    path = [v]
                    while visited[v][0] is not None:
                        v = visited[v][0]
                        path.append(v)
                    return path[::-1], visited[end][1]
    
    return None, float('inf')


def detect_cycle_undirected(graph: dict) -> bool:
    """檢測無向圖環"""
    visited = set()
    
    def dfs(u: str, parent: str) -> bool:
        visited.add(u)
        
        for v in graph.get(u, []):
            if v == parent:
                continue
            if v in visited:
                return True
            if dfs(v, u):
                return True
        
        return False
    
    for vertex in graph:
        if vertex not in visited:
            if dfs(vertex, None):
                return True
    
    return False


def detect_cycle_directed(graph: dict) -> bool:
    """檢測有向圖環"""
    WHITE, GRAY, BLACK = 0, 1, 2
    color = defaultdict(int)
    
    def dfs(u: str) -> bool:
        color[u] = GRAY
        
        for v in graph.get(u, []):
            if color[v] == GRAY:
                return True
            if color[v] == WHITE and dfs(v):
                return True
        
        color[u] = BLACK
        return False
    
    for vertex in graph:
        if color[vertex] == WHITE:
            if dfs(vertex):
                return True
    
    return False


if __name__ == "__main__":
    # 測試圖
    graph = {
        'A': [('B', 1), ('C', 4)],
        'B': [('A', 1), ('C', 2), ('D', 5)],
        'C': [('A', 4), ('B', 2), ('D', 1)],
        'D': [('B', 5), ('C', 1)]
    }
    
    print("圖形演算法測試:")
    print(f"BFS: {bfs(graph, 'A')}")
    print(f"DFS: {dfs(graph, 'A')}")
    print(f"Dijkstra: {dijkstra(graph, 'A')}")
    
    # 拓撲排序測試
    dag = {
        'A': ['B', 'C'],
        'B': ['D'],
        'C': ['D'],
        'D': []
    }
    print(f"拓撲排序: {topological_sort(dag)}")
    
    # BFS 最短路徑
    unweighted = {
        'A': ['B', 'C'],
        'B': ['A', 'D', 'E'],
        'C': ['A', 'F'],
        'D': ['B'],
        'E': ['B', 'F'],
        'F': ['C', 'E']
    }
    path, dist = bfs_shortest_path(unweighted, 'A', 'F')
    print(f"最短路徑 A->F: {path}, 距離: {dist}")
