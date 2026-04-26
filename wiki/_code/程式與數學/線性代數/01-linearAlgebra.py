"""
線性代數範例
展示矩陣運算、特徵值分解、奇異值分解和線性方程組求解
"""

import math
import numpy as np
from typing import List, Tuple
from dataclasses import dataclass


class Matrix:
    """矩陣類"""
    
    def __init__(self, data: List[List[float]]):
        self.data = np.array(data, dtype=float)
        self.shape = self.data.shape
    
    @staticmethod
    def zeros(rows: int, cols: int) -> 'Matrix':
        """建立零矩陣"""
        return Matrix(np.zeros((rows, cols)))
    
    @staticmethod
    def identity(n: int) -> 'Matrix':
        """建立單位矩陣"""
        return Matrix(np.eye(n))
    
    @staticmethod
    def random(rows: int, cols: int) -> 'Matrix':
        """建立隨機矩陣"""
        return Matrix(np.random.rand(rows, cols))
    
    def __add__(self, other: 'Matrix') -> 'Matrix':
        """矩陣加法"""
        return Matrix(self.data + other.data)
    
    def __sub__(self, other: 'Matrix') -> 'Matrix':
        """矩陣減法"""
        return Matrix(self.data - other.data)
    
    def __mul__(self, scalar: float) -> 'Matrix':
        """純量乘法"""
        return Matrix(self.data * scalar)
    
    def __matmul__(self, other: 'Matrix') -> 'Matrix':
        """矩陣乘法"""
        return Matrix(self.data @ other.data)
    
    def transpose(self) -> 'Matrix':
        """轉置"""
        return Matrix(self.data.T)
    
    def inverse(self) -> 'Matrix':
        """反矩陣"""
        return Matrix(np.linalg.inv(self.data))
    
    def determinant(self) -> float:
        """行列式"""
        return float(np.linalg.det(self.data))
    
    def trace(self) -> float:
        """跡"""
        return float(np.trace(self.data))
    
    def norm(self, ord: str = 'fro') -> float:
        """範數"""
        return float(np.linalg.norm(self.data, ord))
    
    def rank(self) -> int:
        """秩"""
        return int(np.linalg.matrix_rank(self.data))


class LinearAlgebra:
    """線性代數運算"""
    
    @staticmethod
    def solve(A: List[List[float]], b: List[float]) -> List[float]:
        """解線性方程組 Ax = b"""
        return np.linalg.solve(A, b).tolist()
    
    @staticmethod
    def eigenvalues(A: List[List[float]]) -> Tuple[List[complex], List[List[complex]]]:
        """特徵值分解"""
        eigenvalues, eigenvectors = np.linalg.eig(A)
        return eigenvalues.tolist(), eigenvectors.tolist()
    
    @staticmethod
    def svd(A: List[List[float]]) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        """奇異值分解 A = UΣV^T"""
        U, s, Vt = np.linalg.svd(A)
        return U, s, Vt
    
    @staticmethod
    def qr(A: List[List[float]]) -> Tuple[np.ndarray, np.ndarray]:
        """QR 分解"""
        Q, R = np.linalg.qr(A)
        return Q, R
    
    @staticmethod
    def lu(A: List[List[float]]) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
        """LU 分解"""
        from scipy.linalg import lu
        P, L, U = lu(A)
        return P, L, U
    
    @staticmethod
    def cholesky(A: List[List[float]]) -> np.ndarray:
        """Cholesky 分解 (對稱正定矩陣)"""
        return np.linalg.cholesky(A)
    
    @staticmethod
    def matrix_power(A: List[List[float]], n: int) -> np.ndarray:
        """矩陣幂"""
        return np.linalg.matrix_power(A, n)


class Vector:
    """向量類"""
    
    @staticmethod
    def dot(a: List[float], b: List[float]) -> float:
        """點積"""
        return sum(x * y for x, y in zip(a, b))
    
    @staticmethod
    def cross(a: List[float], b: List[float]) -> List[float]:
        """叉積 (3D)"""
        return [
            a[1]*b[2] - a[2]*b[1],
            a[2]*b[0] - a[0]*b[2],
            a[0]*b[1] - a[1]*b[0]
        ]
    
    @staticmethod
    def magnitude(a: List[float]) -> float:
        """向量的模"""
        return math.sqrt(sum(x**2 for x in a))
    
    @staticmethod
    def normalize(a: List[float]) -> List[float]:
        """單位化"""
        mag = Vector.magnitude(a)
        return [x / mag for x in a] if mag > 0 else a
    
    @staticmethod
    def angle(a: List[float], b: List[float]) -> float:
        """兩向量夾角 (弧度)"""
        dot = Vector.dot(a, b)
        mag = Vector.magnitude(a) * Vector.magnitude(b)
        return math.acos(dot / mag) if mag > 0 else 0
    
    @staticmethod
    def projection(a: List[float], b: List[float]) -> List[float]:
        """a 在 b 上的投影"""
        dot = Vector.dot(a, b)
        mag_sq = sum(x**2 for x in b)
        scale = dot / mag_sq if mag_sq > 0 else 0
        return [x * scale for x in b]


class LinearTransformation:
    """線性變換"""
    
    @staticmethod
    def rotation_2d(angle: float) -> np.ndarray:
        """2D 旋轉矩陣"""
        c, s = math.cos(angle), math.sin(angle)
        return np.array([[c, -s], [s, c]])
    
    @staticmethod
    def scaling_2d(sx: float, sy: float) -> np.ndarray:
        """2D 縮放矩陣"""
        return np.array([[sx, 0], [0, sy]])
    
    @staticmethod
    def shearing_2d(shx: float, shy: float) -> np.ndarray:
        """2D 剪切矩陣"""
        return np.array([[1, shx], [shy, 1]])
    
    @staticmethod
    def reflection_2d(axis: str = 'x') -> np.ndarray:
        """2D 反射矩陣"""
        if axis == 'x':
            return np.array([[1, 0], [0, -1]])
        return np.array([[-1, 0], [0, 1]])
    
    @staticmethod
    def rotation_3d(axis: str, angle: float) -> np.ndarray:
        """3D 旋轉矩陣"""
        c, s = math.cos(angle), math.sin(angle)
        
        if axis == 'x':
            return np.array([[1, 0, 0], [0, c, -s], [0, s, c]])
        elif axis == 'y':
            return np.array([[c, 0, s], [0, 1, 0], [-s, 0, c]])
        else:
            return np.array([[c, -s, 0], [s, c, 0], [0, 0, 1]])


class LeastSquares:
    """最小二乘法"""
    
    @staticmethod
    def fit(x: List[float], y: List[float], degree: int = 1) -> List[float]:
        """多項式擬合"""
        return np.polyfit(x, y, degree).tolist()
    
    @staticmethod
    def predict(coeffs: List[float], x: float) -> float:
        """預測"""
        return np.polyval(coeffs, x)
    
    @staticmethod
    def linear_regression(X: List[List[float]], y: List[float]) -> Tuple[np.ndarray, float]:
        """線性回歸"""
        X = np.array(X)
        y = np.array(y)
        
        X_b = np.c_[np.ones((len(X), 1)), X]
        
        weights = np.linalg.lstsq(X_b, y, rcond=None)[0]
        
        return weights[1:], weights[0]


class Eigendecomposition:
    """特徵值分解"""
    
    @staticmethod
    def power_iteration(A: List[List[float]], num_iter: int = 100) -> Tuple[float, List[float]]:
        """幂迭代法 (最大特徵值)"""
        n = len(A)
        v = np.random.rand(n)
        v = v / np.linalg.norm(v)
        
        for _ in range(num_iter):
            Av = A @ v
            v = Av / np.linalg.norm(Av)
        
        eigenvalue = float(v @ (A @ v))
        return eigenvalue, v.tolist()
    
    @staticmethod
    def inverse_power_iteration(A: List[List[float]], shift: float = 0, 
                               num_iter: int = 100) -> Tuple[float, List[float]]:
        """逆幂迭代法 (最近於 shift 的特徵值)"""
        n = len(A)
        A_shifted = np.array(A) - shift * np.eye(n)
        
        v = np.random.rand(n)
        v = v / np.linalg.norm(v)
        
        for _ in range(num_iter):
            Av = np.linalg.solve(A_shifted, v)
            v = Av / np.linalg.norm(v)
        
        eigenvalue = float(v @ (A @ v))
        return eigenvalue, v.tolist()


def demo_matrix_operations():
    """矩陣運算範例"""
    print("=" * 50)
    print("1. 矩陣基本運算")
    print("=" * 50)
    
    A = Matrix([[1, 2], [3, 4]])
    B = Matrix([[5, 6], [7, 8]])
    
    print(f"\nA = \n{A.data}")
    print(f"B = \n{B.data}")
    
    print(f"\nA + B =\n{(A + B).data}")
    print(f"A - B =\n{(A - B).data}")
    print(f"A * 2 =\n{(A * 2).data}")
    print(f"A @ B =\n{(A @ B).data}")
    print(f"A^T =\n{A.transpose().data}")
    print(f"det(A) = {A.determinant()}")
    print(f"rank(A) = {A.rank()}")
    print(f"norm(A) = {A.norm()}")


def demo_linear_equations():
    """線性方程組求解"""
    print("\n" + "=" * 50)
    print("2. 線性方程組 Ax = b")
    print("=" * 50)
    
    A = [[2, 1, -1], [3, 2, 1], [1, -1, 2]]
    b = [8, 18, 9]
    
    x = LinearAlgebra.solve(A, b)
    
    print(f"\nA = {A}")
    print(f"b = {b}")
    print(f"\n解 x = {x}")
    
    Ax = np.array(A) @ np.array(x)
    print(f"\n驗證 Ax = {Ax.tolist()}")
    print(f"與 b 相等: {np.allclose(Ax, b)}")


def demo_eigenvalues():
    """特徵值分解"""
    print("\n" + "=" * 50)
    print("3. 特徵值分解")
    print("=" * 50)
    
    A = [[4, 2], [1, 3]]
    
    eigenvalues, eigenvectors = LinearAlgebra.eigenvalues(A)
    
    print(f"\nA = {A}")
    print(f"特徵值: {eigenvalues}")
    print(f"特徵向量:")
    for i, v in enumerate(eigenvectors):
        print(f"  λ = {eigenvalues[i]:.4f}, v = {v}")


def demo_svd():
    """奇異值分解"""
    print("\n" + "=" * 50)
    print("4. 奇異值分解 (SVD)")
    print("=" * 50)
    
    A = [[1, 2], [3, 4], [5, 6]]
    
    U, s, Vt = LinearAlgebra.svd(A)
    
    print(f"\nA = {A}")
    print(f"\nU (左奇異向量):\n{U}")
    print(f"\nΣ (奇異值): {s}")
    print(f"\nV^T (右奇異向量):\n{Vt}")
    
    reconstructed = U @ np.diag(s) @ Vt
    print(f"\n重建: UΣV^T =\n{reconstructed}")
    print(f"與原矩陣相等: {np.allclose(reconstructed, A)}")


def demo_transformations():
    """線性變換"""
    print("\n" + "=" * 50)
    print("5. 2D 線性變換")
    print("=" * 50)
    
    v = np.array([1, 0])
    
    R = LinearTransformation.rotation_2d(math.pi / 4)
    print(f"\n旋轉 45°: {R @ v}")
    
    S = LinearTransformation.scaling_2d(2, 0.5)
    print(f"縮放 (2, 0.5): {S @ v}")
    
    H = LinearTransformation.shearing_2d(0.5, 0)
    print(f"剪切 (0.5, 0): {H @ v}")
    
    F = LinearTransformation.reflection_2d('x')
    print(f"關於 x 軸反射: {F @ v}")


def demo_least_squares():
    """最小二乘法"""
    print("\n" + "=" * 50)
    print("6. 最小二乘法擬合")
    print("=" * 50)
    
    x = [1, 2, 3, 4, 5]
    y = [2.1, 4.0, 5.2, 6.9, 8.1]
    
    coeffs = LeastSquares.fit(x, y, degree=1)
    
    print(f"\n資料點: {list(zip(x, y))}")
    print(f"擬合直線: y = {coeffs[0]:.4f}x + {coeffs[1]:.4f}")
    
    print(f"\n預測:")
    for xi in [1.5, 2.5, 3.5, 4.5]:
        yi = LeastSquares.predict(coeffs, xi)
        print(f"  x = {xi}, y = {yi:.4f}")


def demo_vector_operations():
    """向量運算"""
    print("\n" + "=" * 50)
    print("7. 向量運算")
    print("=" * 50)
    
    a = [1, 2, 3]
    b = [4, 5, 6]
    
    print(f"\na = {a}")
    print(f"b = {b}")
    print(f"\n點積: {Vector.dot(a, b)}")
    print(f"叉積: {Vector.cross(a, b)}")
    print(f"|a| = {Vector.magnitude(a)}")
    print(f"單位化 a: {Vector.normalize(a)}")
    print(f"夾角: {math.degrees(Vector.angle(a, b)):.2f}°")


def demo_qr_decomposition():
    """QR 分解"""
    print("\n" + "=" * 50)
    print("8. QR 分解")
    print("=" * 50)
    
    A = [[12, -51, 4], [6, 167, -68], [-4, 24, -41]]
    
    Q, R = LinearAlgebra.qr(A)
    
    print(f"\nA = {A}")
    print(f"\nQ (正交矩陣):\n{Q}")
    print(f"\nR (上三角):\n{R}")
    
    print(f"\n驗證: Q @ R = \n{Q @ R}")
    print(f"與 A 相等: {np.allclose(Q @ R, A)}")


if __name__ == "__main__":
    print("線性代數 Python 範例")
    print("=" * 50)
    
    demo_matrix_operations()
    demo_linear_equations()
    demo_eigenvalues()
    demo_svd()
    demo_transformations()
    demo_least_squares()
    demo_vector_operations()
    demo_qr_decomposition()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
