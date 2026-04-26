#!/usr/bin/env python3
"""
幾何學 - 計算幾何範例程式
展示凸包、距離計算等幾何演算法
"""

import math


def distance(p1: tuple, p2: tuple) -> float:
    """歐氏距離"""
    return math.sqrt((p2[0] - p1[0])**2 + (p2[1] - p1[1])**2)


def cross_product(o: tuple, a: tuple, b: tuple) -> float:
    """向量 OA x OB 的有向面積（2D 叉積）"""
    return (a[0] - o[0]) * (b[1] - o[1]) - (a[1] - o[1]) * (b[0] - o[0])


def graham_scan(points: list) -> list:
    """Graham 掃描法求凸包"""
    if len(points) < 3:
        return points
    
    # 找最下方的點
    start = min(points, key=lambda p: (p[1], p[0]))
    points = [p for p in points if p != start]
    
    # 按極角排序
    points.sort(key=lambda p: math.atan2(p[1] - start[1], p[0] - start[0]))
    points = [start] + points
    
    # 構建凸包
    hull = [points[0], points[1]]
    
    for i in range(2, len(points)):
        while len(hull) >= 2 and cross_product(hull[-2], hull[-1], points[i]) <= 0:
            hull.pop()
        hull.append(points[i])
    
    return hull


def jarvis_march(points: list) -> list:
    """Jarvis  марш (禮物包裝法) 求凸包"""
    if len(points) < 3:
        return points
    
    # 找最左邊的點
    start = min(points, key=lambda p: p[0])
    
    hull = []
    point_on_hull = start
    
    while True:
        hull.append(point_on_hull)
        next_point = points[0]
        
        for p in points:
            if next_point == point_on_hull:
                next_point = p
            elif cross_product(point_on_hull, next_point, p) < 0:
                next_point = p
        
        point_on_hull = next_point
        
        if point_on_hull == start:
            break
    
    return hull


def point_in_polygon(point: tuple, polygon: list) -> bool:
    """射線法判斷點是否在多邊形內"""
    x, y = point
    n = len(polygon)
    inside = False
    
    j = n - 1
    for i in range(n):
        xi, yi = polygon[i]
        xj, yj = polygon[j]
        
        if ((yi > y) != (yj > y) and 
            x < (xj - xi) * (y - yi) / (yj - yi) + xi):
            inside = not inside
        
        j = i
    
    return inside


def point_to_line_distance(point: tuple, line: tuple, line_end: tuple) -> float:
    """點到直線的最短距離"""
    x0, y0 = point
    x1, y1 = line
    x2, y2 = line_end
    
    # 直線一般式 Ax + By + C = 0
    A = y2 - y1
    B = x1 - x2
    C = x2 * y1 - x1 * y2
    
    return abs(A * x0 + B * y0 + C) / math.sqrt(A*A + B*B)


def convex_hull_area(polygon: list) -> float:
    """計算凸多邊形面積（鞋帶公式）"""
    n = len(polygon)
    area = 0
    
    for i in range(n):
        j = (i + 1) % n
        area += polygon[i][0] * polygon[j][1]
        area -= polygon[j][0] * polygon[i][1]
    
    return abs(area) / 2


def rotate_point(point: tuple, angle: float, origin: tuple = (0, 0)) -> tuple:
    """旋轉點"""
    cos_a = math.cos(angle)
    sin_a = math.sin(angle)
    
    dx = point[0] - origin[0]
    dy = point[1] - origin[1]
    
    return (
        origin[0] + dx * cos_a - dy * sin_a,
        origin[1] + dx * sin_a + dy * cos_a
    )


def line_intersection(p1: tuple, p2: tuple, p3: tuple, p4: tuple) -> tuple:
    """求兩條直線的交點"""
    x1, y1 = p1
    x2, y2 = p2
    x3, y3 = p3
    x4, y4 = p4
    
    denom = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4)
    
    if abs(denom) < 1e-10:
        return None  # 平行
    
    t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / denom
    
    return (x1 + t * (x2 - x1), y1 + t * (y2 - y1))


if __name__ == "__main__":
    # 測試凸包
    points = [(0, 0), (1, 0), (2, 0), (1, 1), (1, 2), (0, 2)]
    print("=== 凸包測試 ===")
    print(f"輸入點: {points}")
    
    hull = graham_scan(points)
    print(f"Graham 凸包: {hull}")
    
    hull2 = jarvis_march(points)
    print(f"Jarvis 凸包: {hull2}")
    
    # 測試點在多邊形內
    polygon = [(0, 0), (4, 0), (4, 3), (0, 3)]
    test_points = [(2, 2), (5, 5), (0, 0)]
    print("\n=== 點在多邊形內測試 ===")
    for pt in test_points:
        result = point_in_polygon(pt, polygon)
        print(f"點 {pt} 在多邊形內: {result}")
    
    # 測試點到直線距離
    point = (1, 1)
    line = (0, 0)
    line_end = (2, 0)
    dist = point_to_line_distance(point, line, line_end)
    print(f"\n點到直線距離: {dist:.4f}")
    
    # 測試旋轉
    point = (1, 0)
    rotated = rotate_point(point, math.pi / 2)
    print(f"點 {point} 旋轉 90 度: {rotated}")
