#!/usr/bin/env python3
"""
代數學 - 群論範例程式
展示群的基本操作與性質
"""

class Group:
    """群的抽象類別"""
    
    def __init__(self, elements, operation):
        self.elements = elements
        self.operation = operation
        self._verify_group()
    
    def _verify_group(self):
        """驗證四個群公理"""
        # 1. 閉合性
        for a in self.elements:
            for b in self.elements:
                result = self.operation(a, b)
                if result not in self.elements:
                    raise ValueError(f"閉合性失敗: {a} * {b} = {result}")
        
        # 2. 找單位元素
        self.identity = None
        for e in self.elements:
            if all(self.operation(e, a) == a and self.operation(a, e) == a 
                   for a in self.elements):
                self.identity = e
                break
        
        if self.identity is None:
            raise ValueError("找不到單位元素")
        
        # 3. 找反元素
        self.inverses = {}
        for a in self.elements:
            found = False
            for b in self.elements:
                if self.operation(a, b) == self.identity:
                    self.inverses[a] = b
                    found = True
                    break
            if not found:
                raise ValueError(f"找不到 {a} 的反元素")
    
    def is_abelian(self):
        """檢查是否為阿貝爾群（交換律）"""
        for a in self.elements:
            for b in self.elements:
                if self.operation(a, b) != self.operation(b, a):
                    return False
        return True
    
    def order(self):
        """群的階（元素數量）"""
        return len(self.elements)
    
    def element_order(self, a):
        """元素的階"""
        result = a
        for i in range(1, len(self.elements) + 1):
            if result == self.identity:
                return i
            result = self.operation(result, a)
        return None


class CyclicGroup(Group):
    """循環群"""
    
    @staticmethod
    def create(n):
        """建立 n 階循環群 Z_n"""
        elements = list(range(n))
        
        def add_mod(a, b):
            return (a + b) % n
        
        return CyclicGroup(elements, add_mod)


class SymmetricGroup(Group):
    """對稱群 S_n"""
    
    @staticmethod
    def create(n):
        """建立 n 次對稱群 S_n"""
        from itertools import permutations
        
        elements = list(permutations(range(n)))
        
        def compose(p, q):
            return tuple(p[q[i]] for i in range(n))
        
        return SymmetricGroup(elements, compose)


def demo_integers():
    """整數加法群 (Z, +)"""
    print("=== 整數加法群 (Z, +) ===")
    # 模擬有限子集
    Z5 = CyclicGroup.create(5)
    print(f"Z_5 是阿貝爾群: {Z5.is_abelian()}")
    print(f"Z_5 的階: {Z5.order()}")
    print(f"Z_5 中 2 的階: {Z5.element_order(2)}")
    print(f"Z_5 中每個元素的反元素: {Z5.inverses}")


def demo_modular():
    """模運算群"""
    print("\n=== 模運算乘法群 ===")
    
    # 尋找 phi(n) 值
    def phi(n):
        count = 0
        for i in range(1, n):
            from math import gcd
            if gcd(i, n) == 1:
                count += 1
        return count
    
    for n in [7, 11, 13]:
        print(f"phi({n}) = {phi(n)}")


def demo_permutations():
    """置換群"""
    print("\n=== 對稱群 S_3 ===")
    S3 = SymmetricGroup.create(3)
    print(f"S_3 是阿貝爾群: {S3.is_abelian()}")
    print(f"S_3 的階: {S3.order()}")  # 3! = 6


class Field:
    """體的範例"""
    
    @staticmethod
    def create_finite_field(p):
        """建立有限體 GF(p) - p 為質數"""
        if not all(p % i != 0 for i in range(2, int(p**0.5) + 1)):
            raise ValueError("p 必須是質數")
        
        elements = list(range(p))
        
        def add_mod(a, b):
            return (a + b) % p
        
        def mul_mod(a, b):
            return (a * b) % p
        
        return Field(elements, add_mod, mul_mod)
    
    def __init__(self, elements, add_op, mul_op):
        self.elements = elements
        self.add_op = add_op
        self.mul_op = mul_op
        
        # 驗證加法群
        self.add_group = Group(elements, add_op)
        
        # 驗證乘法群（排除 0）
        non_zero = [e for e in elements if e != 0]
        self.mul_group = Group(non_zero, mul_op)


def demo_field():
    """有限體範例"""
    print("\n=== 有限體 GF(7) ===")
    GF7 = Field.create_finite_field(7)
    print(f"GF(7) 加法單位: {GF7.add_group.identity}")
    print(f"GF(7) 乘法單位: {GF7.mul_group.identity}")


if __name__ == "__main__":
    demo_integers()
    demo_modular()
    demo_permutations()
    demo_field()
