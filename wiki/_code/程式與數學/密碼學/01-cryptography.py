"""
密碼學範例
展示對稱加密、公鑰加密、雜湊函數和數位簽章的原理與應用
"""

import math
import random
import hashlib
import base64
from typing import Tuple, List
from dataclasses import dataclass


class ModularArithmetic:
    """模運算工具類"""
    
    @staticmethod
    def mod_pow(base: int, exp: int, mod: int) -> int:
        """模指數運算: base^exp mod mod"""
        return pow(base, exp, mod)
    
    @staticmethod
    def mod_inverse(a: int, mod: int) -> int:
        """模逆元: a^(-1) mod mod (使用擴展歐幾里得演算法)"""
        def extended_gcd(a, b):
            if b == 0:
                return (a, 1, 0)
            else:
                g, x, y = extended_gcd(b, a % b)
                return (g, y, x - (a // b) * y)
        
        g, x, _ = extended_gcd(a, mod)
        if g != 1:
            raise ValueError("模逆元不存在")
        return x % mod
    
    @staticmethod
    def gcd(a: int, b: int) -> int:
        """最大公因數"""
        while b:
            a, b = b, a % b
        return a
    
    @staticmethod
    def lcm(a: int, b: int) -> int:
        """最小公倍數"""
        return abs(a * b) // math.gcd(a, b)


class RSA:
    """RSA 公鑰密碼系統"""
    
    def __init__(self, key_size: int = 256):
        self.key_size = key_size
        self.n = None
        self.e = None
        self.d = None
        self._generate_keys()
    
    def _is_prime(self, n: int, k: int = 40) -> bool:
        """Miller-Rabin 質數測試"""
        if n < 2:
            return False
        if n == 2 or n == 3:
            return True
        if n % 2 == 0:
            return False
        
        r, d = 0, n - 1
        while d % 2 == 0:
            r += 1
            d //= 2
        
        for _ in range(k):
            a = random.randrange(2, n - 2)
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
    
    def _generate_prime(self, bits: int) -> int:
        """生成隨機質數"""
        while True:
            p = random.getrandbits(bits)
            if self._is_prime(p):
                return p
    
    def _generate_keys(self):
        """生成 RSA 金鑰對"""
        bits = self.key_size // 2
        
        p = self._generate_prime(bits)
        q = self._generate_prime(bits)
        
        self.n = p * q
        phi = (p - 1) * (q - 1)
        
        self.e = 65537
        while ModularArithmetic.gcd(self.e, phi) != 1:
            self.e = random.randrange(2, phi)
        
        self.d = ModularArithmetic.mod_inverse(self.e, phi)
    
    def encrypt(self, message: int) -> int:
        """加密: C = M^e mod n"""
        return pow(message, self.e, self.n)
    
    def decrypt(self, cipher: int) -> int:
        """解密: M = C^d mod n"""
        return pow(cipher, self.d, self.n)
    
    def get_public_key(self) -> Tuple[int, int]:
        """取得公鑰 (n, e)"""
        return (self.n, self.e)
    
    def get_private_key(self) -> Tuple[int, int]:
        """取得私鑰 (n, d)"""
        return (self.n, self.d)


class AES:
    """簡化版 AES 加密範例 (實際使用請用 cryptography 庫)"""
    
    SBOX = [
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
        0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
        0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
        0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
    ]
    
    @staticmethod
    def sub_bytes(state: List[int]) -> List[int]:
        """SubBytes 轉換"""
        return [AES.SBOX[b] for b in state]
    
    @staticmethod
    def add_round_key(state: List[int], key: List[int]) -> List[int]:
        """AddRoundKey 轉換 (XOR)"""
        return [s ^ k for s, k in zip(state, key)]


class HashFunction:
    """雜湊函數"""
    
    @staticmethod
    def sha256(message: str) -> str:
        """SHA-256 雜湊"""
        return hashlib.sha256(message.encode()).hexdigest()
    
    @staticmethod
    def sha512(message: str) -> str:
        """SHA-512 雜湊"""
        return hashlib.sha512(message.encode()).hexdigest()
    
    @staticmethod
    def md5(message: str) -> str:
        """MD5 雜湊 (已不安全，僅供學習)"""
        return hashlib.md5(message.encode()).hexdigest()
    
    @staticmethod
    def simple_hash(message: str, mod: int = 1000) -> int:
        """簡單雜湊函數 (僅供理解概念)"""
        h = 0
        for char in message:
            h = (h * 31 + ord(char)) % mod
        return h


class DigitalSignature:
    """數位簽章"""
    
    def __init__(self):
        self.rsa = RSA(key_size=256)
    
    def sign(self, message: str) -> int:
        """對訊息產生數位簽章"""
        message_hash = int(HashFunction.sha256(message)[:16], 16)
        return self.rsa.decrypt(message_hash)
    
    def verify(self, message: str, signature: int) -> bool:
        """驗證數位簽章"""
        message_hash = int(HashFunction.sha256(message)[:16], 16)
        decrypted = self.rsa.encrypt(signature)
        return message_hash == decrypted


class DiffieHellman:
    """Diffie-Hellman 金鑰交換"""
    
    DEFAULT_G = 2
    DEFAULT_P = 0xFFFFFFFFFFFFFFFFC90FDAA22168C234C4C6628B80DC1CD129024E088A67CC74020BBEA63B139B22514A08798E3404DD985951873AB85612C9B197F3AF6C6F4F8C2E81A8E170D465E4BF3C70B2B3E6C5F6C7E2D64FA9C3E8B9C4C8A5F4F3E8B9C4C8A5
    
    def __init__(self, p: int = None, g: int = None):
        self.p = p or DiffieHellman.DEFAULT_P
        self.g = g or DiffieHellman.DEFAULT_G
        self.private_key = random.randrange(2, self.p - 1)
        self.public_key = pow(self.g, self.private_key, self.p)
    
    def get_public_key(self) -> int:
        """取得公鑰"""
        return self.public_key
    
    def compute_shared_secret(self, other_public_key: int) -> int:
        """計算共享金鑰"""
        return pow(other_public_key, self.private_key, self.p)


def demo_modular_arithmetic():
    """模運算範例"""
    print("=" * 50)
    print("1. 模運算")
    print("=" * 50)
    
    a, b, mod = 17, 5, 13
    
    print(f"\n{a}^{b} mod {mod} = {ModularArithmetic.mod_pow(a, b, mod)}")
    print(f"{a}^(-1) mod {mod} = {ModularArithmetic.mod_inverse(a, mod)}")
    print(f"gcd({a}, {mod}) = {ModularArithmetic.gcd(a, mod)}")


def demo_rsa():
    """RSA 加密範例"""
    print("\n" + "=" * 50)
    print("2. RSA 公鑰加密")
    print("=" * 50)
    
    rsa = RSA(key_size=256)
    n, e = rsa.get_public_key()
    
    print(f"\n公鑰 (n, e):")
    print(f"  n = {n}")
    print(f"  e = {e}")
    print(f"  n 位元數: {n.bit_length()}")
    
    message = 42
    print(f"\n原始訊息: {message}")
    
    cipher = rsa.encrypt(message)
    print(f"加密後: {cipher}")
    
    decrypted = rsa.decrypt(cipher)
    print(f"解密後: {decrypted}")
    print(f"解密成功: {decrypted == message}")


def demo_aes_sbox():
    """AES S-Box 範例"""
    print("\n" + "=" * 50)
    print("3. AES SubBytes (S-Box)")
    print("=" * 50)
    
    state = [0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 
             0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF]
    
    transformed = AES.sub_bytes(state)
    
    print(f"\n原始狀態: {[hex(b) for b in state[:4]]}")
    print(f"轉換後:   {[hex(b) for b in transformed[:4]]}")
    print(f"\n狀態 XOR 金鑰:")
    key = [0x00] * 16
    xored = AES.add_round_key(state, key)
    print(f"  {[hex(b) for b in xored[:4]]}")


def demo_hash():
    """雜湊函數範例"""
    print("\n" + "=" * 50)
    print("4. 雜湊函數")
    print("=" * 50)
    
    messages = ["Hello", "World", "hello", "Hello"]
    
    for msg in messages:
        h = HashFunction.sha256(msg)
        print(f"\nSHA-256(\"{msg}\"):")
        print(f"  {h[:32]}...")
        print(f"  長度: {len(h)} 字元")


def demo_collision():
    """雜湊碰撞範例"""
    print("\n" + "=" * 50)
    print("5. 簡單雜湊碰撞")
    print("=" * 50)
    
    print("\n使用簡單雜湊函數:")
    h1 = HashFunction.simple_hash("hello")
    h2 = HashFunction.simple_hash("world")
    print(f"  \"hello\" -> {h1}")
    print(f"  \"world\" -> {h2}")
    print(f"  碰撞: {h1 == h2}")
    
    print("\n使用 SHA-256:")
    h1 = HashFunction.sha256("hello")
    h2 = HashFunction.sha256("hello")
    print(f"  \"hello\" -> {h1[:16]}...")
    print(f"  \"hello\" -> {h2[:16]}...")
    print(f"  一致性: {h1 == h2}")


def demo_digital_signature():
    """數位簽章範例"""
    print("\n" + "=" * 50)
    print("6. 數位簽章")
    print("=" * 50)
    
    ds = DigitalSignature()
    
    message = "Hello, World!"
    signature = ds.sign(message)
    
    print(f"\n訊息: \"{message}\"")
    print(f"簽章: {signature}")
    
    valid = ds.verify(message, signature)
    print(f"驗證結果: {'有效' if valid else '無效'}")
    
    invalid = ds.verify("Hello", signature)
    print(f"篡改後驗證: {'有效' if invalid else '無效'}")


def demo_diffie_hellman():
    """Diffie-Hellman 金鑰交換範例"""
    print("\n" + "=" * 50)
    print("7. Diffie-Hellman 金鑰交換")
    print("=" * 50)
    
    alice = DiffieHellman()
    bob = DiffieHellman()
    
    print(f"\nAlice 公鑰: {alice.get_public_key()}")
    print(f"Bob 公鑰: {bob.get_public_key()}")
    
    alice_shared = alice.compute_shared_secret(bob.get_public_key())
    bob_shared = bob.compute_shared_secret(alice.get_public_key())
    
    print(f"\nAlice 共享金鑰: {alice_shared}")
    print(f"Bob 共享金鑰: {bob_shared}")
    print(f"金鑰匹配: {alice_shared == bob_shared}")


if __name__ == "__main__":
    print("密碼學 Python 範例")
    print("=" * 50)
    
    demo_modular_arithmetic()
    demo_rsa()
    demo_aes_sbox()
    demo_hash()
    demo_collision()
    demo_digital_signature()
    demo_diffie_hellman()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
