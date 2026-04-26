"""
化簡法範例
展示問題簡化、降維和近似算法
"""

import math
import random
from typing import List, Tuple, Callable


class Reduction:
    """化簡方法"""
    
    @staticmethod
    def reduce_dimension_2d(points: List[Tuple[float, float]], 
                           target_dim: int = 1) -> List[float]:
        """維度化簡 (2D -> 1D)"""
        if target_dim == 1:
            return [math.sqrt(x**2 + y**2) for x, y in points]
        return points
    
    @staticmethod
    def simplify_curve(points: List[Tuple[float, float]], 
                      tolerance: float = 0.1) -> List[Tuple[float, float]]:
        """曲線簡化 (Douglas-Peucker)"""
        if len(points) <= 2:
            return points
        
        def perpendicular_distance(point: Tuple[float, float],
                                  line_start: Tuple[float, float],
                                  line_end: Tuple[float, float]) -> float:
            x, y = point
            x1, y1 = line_start
            x2, y2 = line_end
            
            dx = x2 - x1
            dy = y2 - y1
            
            if dx == 0 and dy == 0:
                return math.sqrt((x - x1)**2 + (y - y1)**2)
            
            t = max(0, min(1, ((x - x1) * dx + (y - y1) * dy) / (dx*dx + dy*dy)))
            
            proj_x = x1 + t * dx
            proj_y = y1 + t * dy
            
            return math.sqrt((x - proj_x)**2 + (y - proj_y)**2)
        
        max_dist = 0
        max_idx = 0
        
        for i in range(1, len(points) - 1):
            dist = perpendicular_distance(points[i], points[0], points[-1])
            if dist > max_dist:
                max_dist = dist
                max_idx = i
        
        if max_dist > tolerance:
            left = Reduction.simplify_curve(points[:max_idx+1], tolerance)
            right = Reduction.simplify_curve(points[max_idx:], tolerance)
            return left[:-1] + right
        else:
            return [points[0], points[-1]]
    
    @staticmethod
    def approximate_function(f: Callable, start: float, end: float, 
                            n_points: int = 100) -> List[Tuple[float, float]]:
        """函數近似"""
        step = (end - start) / (n_points - 1)
        return [(start + i * step, f(start + i * step)) for i in range(n_points)]


class Approximation:
    """近似方法"""
    
    @staticmethod
    def linear_approximation(points: List[Tuple[float, float]]) -> Tuple[float, float]:
        """線性近似 (最小二乘法)"""
        n = len(points)
        sum_x = sum(x for x, _ in points)
        sum_y = sum(y for _, y in points)
        sum_xy = sum(x * y for x, y in points)
        sum_x2 = sum(x * x for x, _ in points)
        
        slope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x * sum_x)
        intercept = (sum_y - slope * sum_x) / n
        
        return slope, intercept
    
    @staticmethod
    def piecewise_constant(values: List[float], n_segments: int) -> List[float]:
        """分段常數近似"""
        if not values:
            return []
        
        segment_size = len(values) // n_segments
        result = []
        
        for i in range(n_segments):
            start = i * segment_size
            end = start + segment_size if i < n_segments - 1 else len(values)
            avg = sum(values[start:end]) / (end - start)
            result.extend([avg] * (end - start))
        
        return result
    
    @staticmethod
    def newton_interpolation(x_points: List[float], y_points: List[float], 
                            x: float) -> float:
        """牛頓插值"""
        n = len(x_points)
        divided_diff = y_points.copy()
        
        for i in range(1, n):
            for j in range(n - 1, i - 1, -1):
                divided_diff[j] = (divided_diff[j] - divided_diff[j-1]) / (x_points[j] - x_points[j-i])
        
        result = divided_diff[0]
        product = 1
        
        for i in range(1, n):
            product *= (x - x_points[i-1])
            result += divided_diff[i] * product
        
        return result


class Simplification:
    """簡化方法"""
    
    @staticmethod
    def reduce_precision(values: List[float], decimals: int = 2) -> List[float]:
        """降低精度"""
        return [round(v, decimals) for v in values]
    
    @staticmethod
    def sampling(values: List, sample_size: int) -> List:
        """採樣簡化"""
        if sample_size >= len(values):
            return values
        
        step = len(values) / sample_size
        return [values[int(i * step)] for i in range(sample_size)]
    
    @staticmethod
    def clustering_simplify(values: List[float], n_clusters: int) -> List[float]:
        """分群簡化"""
        if n_clusters >= len(values):
            return values
        
        points = [(v, i) for i, v in enumerate(values)]
        points.sort(key=lambda x: x[0])
        
        cluster_size = len(points) // n_clusters
        result = []
        
        for i in range(n_clusters):
            start = i * cluster_size
            end = start + cluster_size if i < n_clusters - 1 else len(points)
            cluster = points[start:end]
            
            avg = sum(v for v, _ in cluster) / len(cluster)
            result.extend([avg] * len(cluster))
        
        return result


def demo_dimension_reduction():
    """維度化簡範例"""
    print("=" * 50)
    print("1. 維度化簡")
    print("=" * 50)
    
    points = [(1, 2), (3, 4), (5, 6), (7, 8)]
    
    print(f"\n2D 點: {points}")
    print(f"化簡至 1D: {Reduction.reduce_dimension_2d(points)}")


def demo_curve_simplification():
    """曲線簡化範例"""
    print("\n" + "=" * 50)
    print("2. 曲線簡化 (Douglas-Peucker)")
    print("=" * 50)
    
    points = [(0, 0), (1, 0.1), (2, -0.1), (3, 5), (4, 0), (5, 0.1), (6, 0)]
    
    simplified = Reduction.simplify_curve(points, tolerance=0.5)
    
    print(f"\n原始點數: {len(points)}")
    print(f"簡化後點數: {len(simplified)}")
    print(f"簡化結果: {simplified}")


def demo_function_approximation():
    """函數近似範例"""
    print("\n" + "=" * 50)
    print("3. 函數近似")
    print("=" * 50)
    
    f = lambda x: math.sin(x)
    points = Reduction.approximate_function(f, 0, math.pi, 5)
    
    print(f"\n近似 sin(x) 在 [0, π]:")
    for x, y in points:
        print(f"  x = {x:.2f}, sin(x) = {y:.4f}")


def demo_linear_approx():
    """線性近似範例"""
    print("\n" + "=" * 50)
    print("4. 線性近似")
    print("=" * 50)
    
    points = [(1, 2), (2, 3.5), (3, 5), (4, 6.5), (5, 8)]
    slope, intercept = Approximation.linear_approximation(points)
    
    print(f"\n資料點: {points}")
    print(f"線性近似: y = {slope:.2f}x + {intercept:.2f}")
    
    for x, y in points:
        predicted = slope * x + intercept
        print(f"  x={x}: 實際={y}, 預測={predicted:.2f}")


def demo_piecewise_constant():
    """分段常數近似範例"""
    print("\n" + "=" * 50)
    print("5. 分段常數近似")
    print("=" * 50)
    
    values = list(range(10))
    
    print(f"\n原始: {values}")
    
    for n_seg in [2, 5]:
        result = Approximation.piecewise_constant(values, n_seg)
        print(f"分段 {n_seg}: {result[:10]}...")


def demo_interpolation():
    """插值範例"""
    print("\n" + "=" * 50)
    print("6. 牛頓插值")
    print("=" * 50)
    
    x_points = [0, 1, 2, 3]
    y_points = [0, 1, 4, 9]
    
    print(f"\n已知點: {list(zip(x_points, y_points))}")
    
    for x in [0.5, 1.5, 2.5]:
        y = Approximation.newton_interpolation(x_points, y_points, x)
        print(f"x = {x}: 插值 = {y:.4f}")


def demo_simplification():
    """簡化方法範例"""
    print("\n" + "=" * 50)
    print("7. 資料簡化")
    print("=" * 50)
    
    values = [1.23456, 2.34567, 3.45678, 4.56789, 5.67890]
    
    print(f"\n原始: {values}")
    print(f"降低精度 (2位): {Simplification.reduce_precision(values, 2)}")
    
    sampled = Simplification.sampling(values, 2)
    print(f"採樣 (2個): {sampled}")
    
    clustered = Simplification.clustering_simplify(values, 2)
    print(f"分群 (2群): {clustered}")


if __name__ == "__main__":
    print("化簡法 Python 範例")
    print("=" * 50)
    
    demo_dimension_reduction()
    demo_curve_simplification()
    demo_function_approximation()
    demo_linear_approx()
    demo_piecewise_constant()
    demo_interpolation()
    demo_simplification()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
