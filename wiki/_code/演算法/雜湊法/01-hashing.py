"""
雜湊法範例
展示各種雜湊技術和衝突處理
"""

import hashlib
from typing import List, Tuple, Dict, Any


class HashTable:
    """雜湊表"""
    
    def __init__(self, size: int = 100):
        self.size = size
        self.table: List[List[Tuple[Any, Any]]] = [[] for _ in range(size)]
    
    def _hash(self, key: Any) -> int:
        """雜湊函數"""
        if isinstance(key, str):
            return sum(ord(c) for c in key) % self.size
        return hash(key) % self.size
    
    def put(self, key: Any, value: Any):
        """插入"""
        idx = self._hash(key)
        bucket = self.table[idx]
        
        for i, (k, v) in enumerate(bucket):
            if k == key:
                bucket[i] = (key, value)
                return
        
        bucket.append((key, value))
    
    def get(self, key: Any) -> Any:
        """取得"""
        idx = self._hash(key)
        bucket = self.table[idx]
        
        for k, v in bucket:
            if k == key:
                return v
        
        return None
    
    def delete(self, key: Any):
        """刪除"""
        idx = self._hash(key)
        bucket = self.table[idx]
        
        for i, (k, v) in enumerate(bucket):
            if k == key:
                del bucket[i]
                return


class SimpleHash:
    """簡單雜湊函數"""
    
    @staticmethod
    def mod_hash(key: int, size: int) -> int:
        """除法雜湊"""
        return key % size
    
    @staticmethod
    def multiplication_hash(key: int, size: int) -> int:
        """乘法雜湊"""
        A = 0.618033988749895
        return int(size * ((key * A) % 1))
    
    @staticmethod
    def string_hash(key: str, size: int) -> int:
        """字串雜湊"""
        h = 0
        for c in key:
            h = (h * 31 + ord(c)) % size
        return h


class CollisionResolution:
    """衝突處理"""
    
    @staticmethod
    def linear_probing(hash_table: List, key: int, value: Any, size: int):
        """線性探測"""
        idx = key % size
        attempts = 0
        
        while hash_table[idx] is not None and attempts < size:
            idx = (idx + 1) % size
            attempts += 1
        
        if attempts < size:
            hash_table[idx] = value
            return True
        return False
    
    @staticmethod
    def quadratic_probing(hash_table: List, key: int, value: Any, size: int):
        """平方探測"""
        idx = key % size
        attempts = 0
        
        while hash_table[idx] is not None and attempts < size:
            idx = (key + attempts * attempts) % size
            attempts += 1
        
        if attempts < size:
            hash_table[idx] = value
            return True
        return False
    
    @staticmethod
    def double_hashing(key: int, size: int, attempt: int) -> int:
        """雙重雜湊"""
        h1 = key % size
        h2 = 1 + (key % (size - 1))
        return (h1 + attempt * h2) % size


class HashFunctions:
    """雜湊函數集合"""
    
    @staticmethod
    def md5(data: str) -> str:
        """MD5"""
        return hashlib.md5(data.encode()).hexdigest()
    
    @staticmethod
    def sha256(data: str) -> str:
        """SHA-256"""
        return hashlib.sha256(data.encode()).hexdigest()
    
    @staticmethod
    def custom_hash(data: str, mod: int = 1000) -> int:
        """自訂雜湊"""
        h = 0
        for c in data:
            h = (h * 31 + ord(c)) % mod
        return h


def demo_basic_hashtable():
    """基本雜湊表"""
    print("=" * 50)
    print("1. 基本雜湊表")
    print("=" * 50)
    
    ht = HashTable(size=10)
    
    ht.put("apple", 100)
    ht.put("banana", 200)
    ht.put("orange", 300)
    
    print(f"\n插入後:")
    print(f"  apple: {ht.get('apple')}")
    print(f"  banana: {ht.get('banana')}")
    print(f"  orange: {ht.get('orange')}")
    print(f"  grape: {ht.get('grape')}")


def demo_hash_functions():
    """雜湊函數"""
    print("\n" + "=" * 50)
    print("2. 雜湊函數")
    print("=" * 50)
    
    keys = [12, 25, 37, 48, 59]
    size = 10
    
    print(f"\n鍵: {keys}, 表大小: {size}")
    print(f"\n{'鍵':<6} {'除法':<8} {'乘法':<8}")
    print("-" * 25)
    
    for key in keys:
        mod = SimpleHash.mod_hash(key, size)
        mult = SimpleHash.multiplication_hash(key, size)
        print(f"{key:<6} {mod:<8} {mult:<8}")


def demo_string_hash():
    """字串雜湊"""
    print("\n" + "=" * 50)
    print("3. 字串雜湊")
    print("=" * 50)
    
    strings = ["hello", "world", "python", "algorithm"]
    size = 100
    
    print(f"\n字串雜湊:")
    for s in strings:
        h = SimpleHash.string_hash(s, size)
        print(f"  '{s}': {h}")


def demo_collision_linear():
    """線性探測"""
    print("\n" + "=" * 50)
    print("4. 衝突處理 - 線性探測")
    print("=" * 50)
    
    size = 10
    table = [None] * size
    
    keys = [5, 15, 25, 35, 45]
    
    print(f"\n插入鍵: {keys}, 表大小: {size}")
    
    for key in keys:
        CollisionResolution.linear_probing(table, key, key, size)
    
    print(f"表內容: {table}")


def demo_collision_quadratic():
    """平方探測"""
    print("\n" + "=" * 50)
    print("5. 衝突處理 - 平方探測")
    print("=" * 50)
    
    size = 10
    table = [None] * size
    
    keys = [5, 15, 25]
    
    print(f"\n插入鍵: {keys}, 表大小: {size}")
    
    for key in keys:
        CollisionResolution.quadratic_probing(table, key, key, size)
    
    print(f"表內容: {table}")


def demo_crypto_hash():
    """加密雜湊"""
    print("\n" + "=" * 50)
    print("6. 加密雜湊函數")
    print("=" * 50)
    
    data = "Hello, World!"
    
    print(f"\n原文: '{data}'")
    print(f"\nMD5: {HashFunctions.md5(data)[:32]}...")
    print(f"SHA-256: {HashFunctions.sha256(data)[:64]}...")


def demo_custom_hash():
    """自訂雜湊"""
    print("\n" + "=" * 50)
    print("7. 自訂雜湊函數")
    print("=" * 50)
    
    keys = ["key1", "key2", "key3", "key4", "key5"]
    
    print(f"\n自訂雜湊 (mod=100):")
    for k in keys:
        h = HashFunctions.custom_hash(k, 100)
        print(f"  '{k}': {h}")


def demo_load_factor():
    """負載因子"""
    print("\n" + "=" * 50)
    print("8. 負載因子")
    print("=" * 50)
    
    ht = HashTable(size=10)
    
    items = [("a", 1), ("b", 2), ("c", 3), ("d", 4), ("e", 5)]
    
    print(f"\n插入項目: {len(items)}")
    
    load_factor = len(items) / ht.size
    print(f"負載因子: {load_factor:.2f}")
    
    if load_factor > 0.7:
        print("建議: 需要擴展雜湊表")


def demo_bloom_filter():
    """Bloom Filter 概念"""
    print("\n" + "=" * 50)
    print("9. Bloom Filter 概念")
    print("=" * 50)
    
    size = 100
    bit_array = [False] * size
    
    def add(value):
        h = HashFunctions.custom_hash(value, size)
        bit_array[h] = True
    
    def check(value):
        h = HashFunctions.custom_hash(value, size)
        return bit_array[h]
    
    items = ["apple", "banana", "orange"]
    
    for item in items:
        add(item)
    
    print(f"\n加入: {items}")
    print(f"  'apple' 存在: {check('apple')}")
    print(f"  'grape' 存在: {check('grape')}")


if __name__ == "__main__":
    print("雜湊法 Python 範例")
    print("=" * 50)
    
    demo_basic_hashtable()
    demo_hash_functions()
    demo_string_hash()
    demo_collision_linear()
    demo_collision_quadratic()
    demo_crypto_hash()
    demo_custom_hash()
    demo_load_factor()
    demo_bloom_filter()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
