"""
資訊理論範例
展示熵、交叉熵、KL 散度、互資訊、霍夫曼編碼和信道編碼定理
"""

import math
import heapq
from typing import List, Tuple, Dict
from collections import Counter
from dataclasses import dataclass


class Entropy:
    """熵的計算"""
    
    @staticmethod
    def entropy(probs: List[float], base: float = 2) -> float:
        """
        計算香農熵
        H(X) = -Σ p(x) log(p(x))
        """
        h = 0.0
        for p in probs:
            if p > 0:
                h -= p * math.log(p, base)
        return h
    
    @staticmethod
    def binary_entropy(p: float) -> float:
        """
        二元熵函數
        H(p) = -p log(p) - (1-p) log(1-p)
        """
        if p == 0 or p == 1:
            return 0.0
        return -p * math.log(p, 2) - (1-p) * math.log(1-p, 2)
    
    @staticmethod
    def joint_entropy(probs: Dict[Tuple, float], base: float = 2) -> float:
        """聯合熵 H(X,Y)"""
        h = 0.0
        for p in probs.values():
            if p > 0:
                h -= p * math.log(p, base)
        return h
    
    @staticmethod
    def conditional_entropy(p_xy: Dict[Tuple, float], 
                           p_x: Dict, base: float = 2) -> float:
        """條件熵 H(Y|X) = Σ p(x) H(Y|X=x)"""
        h = 0.0
        for (x, y), p_xy_val in p_xy.items():
            if p_xy_val > 0:
                p_y_given_x = p_xy_val / p_x.get(x, 1)
                if p_y_given_x > 0:
                    h -= p_xy_val * math.log(p_y_given_x, base)
        return h


class CrossEntropy:
    """交叉熵"""
    
    @staticmethod
    def cross_entropy(p: List[float], q: List[float], base: float = 2) -> float:
        """
        交叉熵 H(P, Q) = -Σ p(x) log(q(x))
        """
        h = 0.0
        for p_val, q_val in zip(p, q):
            if p_val > 0 and q_val > 0:
                h -= p_val * math.log(q_val, base)
        return h
    
    @staticmethod
    def kl_divergence(p: List[float], q: List[float], base: float = 2) -> float:
        """
        KL 散度 D(P||Q) = Σ p(x) log(p(x)/q(x))
        """
        d = 0.0
        for p_val, q_val in zip(p, q):
            if p_val > 0 and q_val > 0:
                d += p_val * math.log(p_val / q_val, base)
        return d


class MutualInformation:
    """互資訊"""
    
    @staticmethod
    def mutual_information(p_xy: Dict[Tuple, float], 
                          p_x: Dict, p_y: Dict) -> float:
        """
        互資訊 I(X;Y) = H(X) - H(X|Y) = H(Y) - H(Y|X)
        """
        i = 0.0
        for (x, y), p_xy_val in p_xy.items():
            if p_xy_val > 0:
                p_x_val = p_x.get(x, 0)
                p_y_val = p_y.get(y, 0)
                if p_x_val > 0 and p_y_val > 0:
                    i += p_xy_val * math.log(p_xy_val / (p_x_val * p_y_val), 2)
        return i


class HuffmanCoding:
    """霍夫曼編碼"""
    
    def __init__(self):
        self.codes = {}
    
    def build(self, frequencies: Dict[str, float]) -> Dict[str, str]:
        """建立霍夫曼編碼"""
        heap = [(freq, char) for char, freq in frequencies.items()]
        heapq.heapify(heap)
        
        while len(heap) > 1:
            freq1, char1 = heapq.heappop(heap)
            freq2, char2 = heapq.heappop(heap)
            heapq.heappush(heap, (freq1 + freq2, (char1, char2)))
        
        self._generate_codes(heap[0], '')
        return self.codes
    
    def _generate_codes(self, node, prefix: str):
        """遞迴生成編碼"""
        if isinstance(node[1], str):
            self.codes[node[1]] = prefix if prefix else '0'
        else:
            self._generate_codes(node[1][0], prefix + '0')
            self._generate_codes(node[1][1], prefix + '1')
    
    def encode(self, text: str) -> str:
        """編碼"""
        return ''.join(self.codes.get(c, '') for c in text)
    
    def decode(self, encoded: str) -> str:
        """解碼"""
        reverse_codes = {v: k for k, v in self.codes.items()}
        decoded = []
        code = ''
        
        for bit in encoded:
            code += bit
            if code in reverse_codes:
                decoded.append(reverse_codes[code])
                code = ''
        
        return ''.join(decoded)
    
    def average_length(self, frequencies: Dict[str, float]) -> float:
        """平均編碼長度"""
        total = sum(frequencies.values())
        avg = 0.0
        for char, freq in frequencies.items():
            avg += (freq / total) * len(self.codes.get(char, ''))
        return avg


class ShannonCapacity:
    """夏農容量"""
    
    @staticmethod
    def channel_capacity(bandwidth: float, snr: float) -> float:
        """
        夏農-哈特利定理
        C = B * log2(1 + S/N)
        """
        return bandwidth * math.log2(1 + snr)
    
    @staticmethod
    def required_bandwidth(data_rate: float, snr: float) -> float:
        """計算所需頻寬"""
        return data_rate / math.log2(1 + snr)
    
    @staticmethod
    def coding_gain(target_ber: float, snr: float) -> float:
        """編碼增益 (近似)"""
        return snr / (-10 * math.log10(target_ber))


class SourceCoding:
    """信源編碼"""
    
    @staticmethod
    def min_bits_per_symbol(entropy: float) -> float:
        """理論最小平均碼長"""
        return entropy
    
    @staticmethod
    def coding_efficiency(avg_length: float, entropy: float) -> float:
        """編碼效率 η = H / L"""
        return entropy / avg_length if avg_length > 0 else 0


class ChannelCoding:
    """信道編碼"""
    
    @staticmethod
    def hamming_distance(a: str, b: str) -> int:
        """漢明距離"""
        return sum(c1 != c2 for c1, c2 in zip(a, b))
    
    @staticmethod
    def parity_check_matrix(n: int, m: int) -> List[List[int]]:
        """生成奇偶校驗矩陣"""
        matrix = []
        for i in range(m):
            row = []
            for j in range(n):
                bit = (i >> j) & 1
                row.append(bit if i < n else 0)
            matrix.append(row)
        return matrix
    
    @staticmethod
    def simple_error_detection(data: str) -> str:
        """簡單奇偶校驗"""
        ones = data.count('1')
        parity = '1' if ones % 2 == 1 else '0'
        return data + parity
    
    @staticmethod
    def simple_error_correction(data: str, parity_bits: List[int]) -> str:
        """簡單漢明碼錯誤修正"""
        error_pos = 0
        for i, p in enumerate(parity_bits):
            if p == 1:
                error_pos += 2 ** i
        
        if error_pos > 0:
            data = list(data)
            data[error_pos - 1] = str(1 - int(data[error_pos - 1]))
            data = ''.join(data)
        
        return data


def demo_entropy():
    """熵範例"""
    print("=" * 50)
    print("1. 香農熵")
    print("=" * 50)
    
    distributions = [
        [0.5, 0.5],
        [0.75, 0.25],
        [1.0],
        [0.25, 0.25, 0.25, 0.25],
    ]
    
    names = [
        "公平硬幣",
        "偏斜硬幣",
        "確定事件",
        "均勻分佈 (4 符號)"
    ]
    
    for name, probs in zip(names, distributions):
        h = Entropy.entropy(probs)
        print(f"\n{name}:")
        print(f"  P = {probs}")
        print(f"  H = {h:.4f} bits")
    
    print(f"\n二元熵函數 H(p):")
    for p in [0.1, 0.3, 0.5, 0.7, 0.9]:
        h = Entropy.binary_entropy(p)
        print(f"  p = {p}: H = {h:.4f} bits")


def demo_cross_entropy_kl():
    """交叉熵和 KL 散度範例"""
    print("\n" + "=" * 50)
    print("2. 交叉熵與 KL 散度")
    print("=" * 50)
    
    p = [0.3, 0.3, 0.4]
    q = [0.4, 0.3, 0.3]
    
    h_p = Entropy.entropy(p)
    h_q = Entropy.entropy(q)
    ce = CrossEntropy.cross_entropy(p, q)
    kl = CrossEntropy.kl_divergence(p, q)
    
    print(f"\n真實分佈 P = {p}")
    print(f"近似分佈 Q = {q}")
    print(f"\nH(P) = {h_p:.4f} bits")
    print(f"H(Q) = {h_q:.4f} bits")
    print(f"H(P, Q) = {ce:.4f} bits")
    print(f"D(P||Q) = {kl:.4f} bits")
    print(f"\n驗證: H(P,Q) = H(P) + D(P||Q) = {h_p + kl:.4f}")


def demo_mutual_information():
    """互資訊範例"""
    print("\n" + "=" * 50)
    print("3. 互資訊")
    print("=" * 50)
    
    p_xy = {
        ('0', '0'): 0.1,
        ('0', '1'): 0.1,
        ('1', '0'): 0.2,
        ('1', '1'): 0.6,
    }
    
    p_x = {'0': 0.2, '1': 0.8}
    p_y = {'0': 0.3, '1': 0.7}
    
    i = MutualInformation.mutual_information(p_xy, p_x, p_y)
    
    h_x = Entropy.entropy(list(p_x.values()))
    h_y = Entropy.entropy(list(p_y.values()))
    
    print(f"\n聯合分佈 P(X,Y):")
    for (x, y), p in p_xy.items():
        print(f"  P({x},{y}) = {p}")
    
    print(f"\nH(X) = {h_x:.4f} bits")
    print(f"H(Y) = {h_y:.4f} bits")
    print(f"I(X;Y) = {i:.4f} bits")


def demo_huffman():
    """霍夫曼編碼範例"""
    print("\n" + "=" * 50)
    print("4. 霍夫曼編碼")
    print("=" * 50)
    
    frequencies = {
        'A': 0.45,
        'B': 0.35,
        'C': 0.15,
        'D': 0.05,
    }
    
    hf = HuffmanCoding()
    codes = hf.build(frequencies)
    
    print(f"\n符號頻率:")
    for char, freq in frequencies.items():
        print(f"  {char}: {freq}")
    
    print(f"\n霍夫曼編碼:")
    for char, code in codes.items():
        print(f"  {char}: {code}")
    
    text = "ABABCA"
    encoded = hf.encode(text)
    decoded = hf.decode(encoded)
    avg_len = hf.average_length(frequencies)
    entropy = Entropy.entropy(list(frequencies.values()))
    efficiency = SourceCoding.coding_efficiency(avg_len, entropy)
    
    print(f"\n原文: {text}")
    print(f"編碼: {encoded}")
    print(f"解碼: {decoded}")
    print(f"平均碼長: {avg_len:.4f} bits/symbol")
    print(f"熵: {entropy:.4f} bits/symbol")
    print(f"編碼效率: {efficiency:.4%}")


def demo_shannon_capacity():
    """夏農容量範例"""
    print("\n" + "=" * 50)
    print("5. 夏農-哈特利定理")
    print("=" * 50)
    
    bw = 3000
    snr_db = 30
    snr = 10 ** (snr_db / 10)
    
    capacity = ShannonCapacity.channel_capacity(bw, snr)
    rate_k = capacity / 1000
    rate_m = capacity / 1e6
    
    print(f"\n頻寬 B = {bw} Hz")
    print(f"訊號雜訊比 S/N = {snr_db} dB ({snr:.0f})")
    print(f"\n信道容量 C = {capacity:.2f} bps")
    print(f"         = {rate_k:.2f} Kbps")
    print(f"         = {rate_m:.2f} Mbps")


def demo_coding_efficiency():
    """編碼效率範例"""
    print("\n" + "=" * 50)
    print("6. 信源編碼效率")
    print("=" * 50)
    
    entropy = 2.5
    
    for avg_len in [2.5, 2.7, 3.0, 4.0]:
        eff = SourceCoding.coding_efficiency(avg_len, entropy)
        print(f"\n平均碼長 L = {avg_len}")
        print(f"熵 H = {entropy}")
        print(f"效率 η = {eff:.2%}")


def demo_hamming():
    """漢明距離範例"""
    print("\n" + "=" * 50)
    print("7. 漢明距離與錯誤檢測")
    print("=" * 50)
    
    code1 = "10101"
    code2 = "11100"
    
    distance = ChannelCoding.hamming_distance(code1, code2)
    
    print(f"\n{code1}")
    print(f"{code2}")
    print(f"漢明距離: {distance}")
    print(f"可檢測錯誤數: {distance - 1}")
    print(f"可修正錯誤數: {distance // 2}")
    
    data = "1011"
    with_parity = ChannelCoding.simple_error_detection(data)
    print(f"\n原數據: {data}")
    print(f"加奇偶校驗: {with_parity}")


if __name__ == "__main__":
    print("資訊理論 Python 範例")
    print("=" * 50)
    
    demo_entropy()
    demo_cross_entropy_kl()
    demo_mutual_information()
    demo_huffman()
    demo_shannon_capacity()
    demo_coding_efficiency()
    demo_hamming()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
