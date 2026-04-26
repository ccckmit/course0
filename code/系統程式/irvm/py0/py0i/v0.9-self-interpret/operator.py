"""
operator.py  –  手寫的 operator 模組
只實作 py0i.py 實際用到的函式，語意與標準函式庫完全相同。
完全不 import 標準函式庫的 operator 模組。
"""

# ── 算術 ─────────────────────────────────────────────────────────────────────

def add(a, b):
    """a + b"""
    return a + b

def sub(a, b):
    """a - b"""
    return a - b

def mul(a, b):
    """a * b"""
    return a * b

def truediv(a, b):
    """a / b"""
    return a / b

def floordiv(a, b):
    """a // b"""
    return a // b

def mod(a, b):
    """a % b"""
    return a % b

def pow(a, b):
    """a ** b"""
    return a ** b

def matmul(a, b):
    """a @ b"""
    return a @ b

def neg(a):
    """-a"""
    return -a

def pos(a):
    """+a"""
    return +a

def abs(a):
    """|a|"""
    return a.__abs__() if hasattr(a, '__abs__') else (a if a >= 0 else -a)

# ── 位元 ─────────────────────────────────────────────────────────────────────

def and_(a, b):
    """a & b"""
    return a & b

def or_(a, b):
    """a | b"""
    return a | b

def xor(a, b):
    """a ^ b"""
    return a ^ b

def invert(a):
    """~a"""
    return ~a

def lshift(a, b):
    """a << b"""
    return a << b

def rshift(a, b):
    """a >> b"""
    return a >> b

# ── 比較 ─────────────────────────────────────────────────────────────────────

def eq(a, b):
    """a == b"""
    return a == b

def ne(a, b):
    """a != b"""
    return a != b

def lt(a, b):
    """a < b"""
    return a < b

def le(a, b):
    """a <= b"""
    return a <= b

def gt(a, b):
    """a > b"""
    return a > b

def ge(a, b):
    """a >= b"""
    return a >= b

# ── 邏輯 ─────────────────────────────────────────────────────────────────────

def not_(a):
    """not a"""
    return not a

def truth(a):
    """bool(a)"""
    return bool(a)

# ── 容器 ─────────────────────────────────────────────────────────────────────

def getitem(a, b):
    """a[b]"""
    return a[b]

def setitem(a, b, c):
    """a[b] = c"""
    a[b] = c

def delitem(a, b):
    """del a[b]"""
    del a[b]

def contains(a, b):
    """b in a"""
    return b in a

def concat(a, b):
    """a + b  (sequences)"""
    return a + b

def length_hint(obj, default=0):
    """len(obj) with fallback"""
    try:
        return len(obj)
    except TypeError:
        return default

# ── 屬性 ─────────────────────────────────────────────────────────────────────

def attrgetter(name):
    """回傳一個函式，用來取出物件的指定屬性。"""
    def getter(obj):
        return getattr(obj, name)
    return getter

def itemgetter(*keys):
    """回傳一個函式，用來取出物件的指定 key。"""
    if len(keys) == 1:
        k = keys[0]
        def getter(obj):
            return obj[k]
    else:
        def getter(obj):
            return tuple(obj[k] for k in keys)
    return getter

def methodcaller(name, *args, **kwargs):
    """回傳一個函式，呼叫物件的指定方法。"""
    def caller(obj):
        return getattr(obj, name)(*args, **kwargs)
    return caller

# ── index / is ───────────────────────────────────────────────────────────────

def index(a):
    """a.__index__()"""
    return a.__index__()

def is_(a, b):
    """a is b"""
    return a is b

def is_not(a, b):
    """a is not b"""
    return a is not b
