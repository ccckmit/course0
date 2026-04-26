# 圖形演算法測試說明

## 執行方式

```bash
python 01-graph.py
```

## 測試項目

### 1. bfs - 廣度優先搜尋
- **輸入**: graph, start='A'
- **預期輸出**: ['A', 'B', 'C', 'D']
- **說明**: 層次遍歷

### 2. dfs - 深度優先搜尋
- **輸入**: graph, start='A'
- **預期輸出**: ['A', 'B', 'D', 'C']
- **說明**: 深度優先遍歷

### 3. dijkstra - 最短路徑
- **輸入**: graph, start='A'
- **預期輸出**: {'A': 0, 'B': 1, 'C': 3, 'D': 4}
- **說明**: 正權重最短路徑

### 4. topological_sort - 拓撲排序
- **輸入**: DAG
- **預期輸出**: 有效的節點順序
- **說明**: 用於依賴排序

### 5. bfs_shortest_path - BFS最短路徑
- **輸入**: unweighted graph, start='A', end='F'
- **預期輸出**: (路徑, 距離)
- **說明**: 未加權圖最短路徑

### 6. detect_cycle - 環檢測
- **輸入**: 圖
- **預期輸出**: True/False
- **說明**: 檢測是否有環

## 預期輸出範例

```
圖形演算法測試:
BFS: ['A', 'B', 'C', 'D']
DFS: ['A', 'B', 'D', 'C']
Dijkstra: {'A': 0, 'B': 1, 'C': 3, 'D': 4}
拓撲排序: ['A', 'C', 'B', 'D']
最短路徑 A->F: ['A', 'C', 'F'], 距離: 2
```

## 時間複雜度

| 演算法 | 時間複雜度 | 空間複雜度 |
|--------|-----------|-----------|
| BFS | O(V+E) | O(V) |
| DFS | O(V+E) | O(V) |
| Dijkstra | O((V+E)logV) | O(V) |
| Bellman-Ford | O(VE) | O(V) |
| Floyd-Warshall | O(V^3) | O(V^2) |
| 拓撲排序 | O(V+E) | O(V) |
| Prim | O((V+E)logV) | O(V) |
| Kruskal | O(ElogV) | O(V) |
