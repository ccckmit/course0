#!/usr/bin/env python3
"""
數論 - 質數與模運算範例
展示質數檢驗、模運算、密碼學基礎
"""

import random
import math


def gcd(a: int, b: int) -> int:
    """歐幾里得演算法 - 最大公因數"""
    while b:
        a, b = b, a % b
    return abs(a)


def extended_gcd(a: int, b: int) -> tuple:
    """擴展歐幾里得演算法"""
    if b == 0:
        return (a, 1, 0)
    else:
        g, x1, y1 = extended_gcd(b, a % b)
        x = y1
        y = x1 - (a // b) * y1
        return (g, x, y)


def mod_inverse(a: int, m: int) -> int:
    """模反元素"""
    g, x, _ = extended_gcd(a, m)
    if g != 1:
        raise ValueError("模反元素不存在")
    return x % m


def sieve_of_eratosthenes(n: int) -> list:
    """埃拉託斯特尼篩法"""
    is_prime = [True] * (n + 1)
    is_prime[0] = is_prime[1] = False
    
    for i in range(2, int(n**0.5) + 1):
        if is_prime[i]:
            for j in range(i*i, n + 1, i):
                is_prime[j] = False
    
    return [i for i in range(n + 1) if is_prime[i]]


def miller_rabin(n: int, k: int = 10) -> bool:
    """米勒-拉賓質數檢驗"""
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    
    # 寫成 n-1 = 2^r * d
    r, d = 0, n - 1
    while d % 2 == 0:
        r += 1
        d //= 2
    
    for _ in range(k):
        a = random.randrange(2, n - 1)
        x = pow(a, d, n)
        
        if x == 1 or x == n - 1:
            continue
        
        for _ in range(r - 1):
            x = pow(x, 2, n)
            if x == n - 1:
                break
        else:
            return False
    
    return True


def is_prime(n: int) -> bool:
    """質數檢驗（簡化版）"""
    if n < 2:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    for i in range(3, int(n**0.5) + 1, 2):
        if n % i == 0:
            return False
    return True


def euler_phi(n: int) -> int:
    """歐拉函數 φ(n)"""
    result = n
    p = 2
    while p * p <= n:
        if n % p == 0:
            while n % p == 0:
                n //= p
            result -= result // p
        p += 1
    if n > 1:
        result -= result // n
    return result


def power_mod(a: int, e: int, m: int) -> int:
    """快速幂模運算"""
    return pow(a, e, m)


def chinese_remainder(remainders: list, moduli: list) -> int:
    """中國剩餘定理"""
    if len(remainders) != len(moduli):
        raise ValueError("長度必須相同")
    
    M = math.prod(moduli)
    result = 0
    
    for r, m in zip(remainders, moduli):
        M_i = M // m
        inv = mod_inverse(M_i, m)
        result += r * M_i * inv
    
    return result % M


def legendre_symbol(a: int, p: int) -> int:
    """勒讓德符號"""
    if p % 2 == 0:
        raise ValueError("p 必須是奇質數")
    a = a % p
    
    if a == 0:
        return 0
    if a == 1:
        return 1
    
    # 歐拉準則
    result = pow(a, (p - 1) // 2, p)
    return result if result == 1 else -1


if __name__ == "__main__":
    # 測試質數相關
    print("=== 質數相關 ===")
    primes = sieve_of_eratosthenes(50)
    print(f"50 以內的質數: {primes}")
    
    # 米勒-拉賓
    test_numbers = [17, 18, 19, 100, 9973]
    for n in test_numbers:
        result = miller_rabin(n)
        print(f"Miller-Rabin({n}): {'質數' if result else '合數'}")
    
    # 歐拉函數
    print("\n=== 歐拉函數 ===")
    for n in [10, 12, 15, 17]:
        print(f"φ({n}) = {euler_phi(n)}")
    
    # 模反元素
    print("\n=== 模反元素 ===")
    a, m = 3, 11
    inv = mod_inverse(a, m)
    print(f"{a}^-1 mod {m} = {inv}")
    print(f"驗證: {a} * {inv} mod {m} = {(a * inv) % m}")
    
    # 中國剩餘定理
    print("\n=== 中國剩餘定理 ===")
    remainders = [2, 3, 2]
    moduli = [3, 4, 5]
    result = chinese_remainder(remainders, moduli)
    print(f"x ≡ {remainders} (mod {moduli}) => x = {result}")
    
    # 快速幂
    print("\n=== 快速幂 ===")
    print(f"2^1000 = {power_mod(2, 1000, 1000000007)}")
