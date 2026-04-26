"""
傅立葉變換範例
展示離散傅立葉變換 (DFT) 和快速傅立葉變換 (FFT) 的原理與應用
"""

import math
import cmath
from typing import List, Tuple
import json

class FourierTransform:
    """傅立葉變換類"""
    
    @staticmethod
    def dft(signal: List[complex]) -> List[complex]:
        """
        離散傅立葉變換 (DFT)
        O(n²) 複雜度
        
        X[k] = Σ x[n] * e^(-j*2π*k*n/N)
        """
        n = len(signal)
        result = []
        
        for k in range(n):
            xk = 0
            for n_idx in range(n):
                angle = -2 * math.pi * k * n_idx / n
                xk += signal[n_idx] * complex(math.cos(angle), math.sin(angle))
            result.append(xk)
        
        return result
    
    @staticmethod
    def idft(signal: List[complex]) -> List[complex]:
        """
        逆離散傅立葉變換 (IDFT)
        
        x[n] = (1/N) * Σ X[k] * e^(j*2π*k*n/N)
        """
        n = len(signal)
        result = []
        
        for n_idx in range(n):
            xn = 0
            for k in range(n):
                angle = 2 * math.pi * k * n_idx / n
                xn += signal[k] * complex(math.cos(angle), math.sin(angle))
            result.append(xn / n)
        
        return result
    
    @staticmethod
    def fft(signal: List[complex]) -> List[complex]:
        """
        快速傅立葉變換 (FFT) - Cooley-Tukey 演算法
        O(n log n) 複雜度
        """
        n = len(signal)
        
        if n <= 1:
            return signal
        
        if n % 2 != 0:
            raise ValueError("信號長度必須為 2 的次方")
        
        even = FourierTransform.fft(signal[0::2])
        odd = FourierTransform.fft(signal[1::2])
        
        result = [0] * n
        
        for k in range(n // 2):
            angle = -2 * math.pi * k / n
            t = complex(math.cos(angle), math.sin(angle)) * odd[k]
            result[k] = even[k] + t
            result[k + n // 2] = even[k] - t
        
        return result
    
    @staticmethod
    def ifft(signal: List[complex]) -> List[complex]:
        """逆 FFT"""
        n = len(signal)
        conjugate = [x.conjugate() for x in signal]
        result = FourierTransform.fft(conjugate)
        return [x.conjugate() / n for x in result]


class SignalGenerator:
    """信號生成器"""
    
    @staticmethod
    def sine(frequency: float, amplitude: float, duration: float, 
             sample_rate: float = 1000) -> Tuple[List[float], List[complex]]:
        """
        生成正弦波
        
        Args:
            frequency: 頻率 (Hz)
            amplitude: 振幅
            duration: 持續時間 (秒)
            sample_rate: 採樣率
        
        Returns:
            (時間軸, 信號)
        """
        num_samples = int(duration * sample_rate)
        time = [i / sample_rate for i in range(num_samples)]
        signal = [complex(amplitude * math.sin(2 * math.pi * frequency * t), 0) 
                  for t in time]
        return time, signal
    
    @staticmethod
    def square(frequency: float, amplitude: float, 
               duration: float, sample_rate: float = 1000) -> Tuple[List[float], List[complex]]:
        """生成方波"""
        num_samples = int(duration * sample_rate)
        time = [i / sample_rate for i in range(num_samples)]
        signal = []
        
        for t in time:
            value = amplitude if math.sin(2 * math.pi * frequency * t) >= 0 else -amplitude
            signal.append(complex(value, 0))
        
        return time, signal
    
    @staticmethod
    def noise(size: int, amplitude: float = 1.0) -> List[complex]:
        """生成隨機雜訊"""
        import random
        return [complex(random.uniform(-amplitude, amplitude), 0) for _ in range(size)]


class FrequencyAnalyzer:
    """頻譜分析器"""
    
    @staticmethod
    def compute_frequencies(spectrum: List[complex], 
                          sample_rate: float) -> Tuple[List[float], List[float]]:
        """
        計算頻率軸和對應的幅度
        
        Args:
            spectrum: FFT 結果
            sample_rate: 採樣率
        
        Returns:
            (頻率, 幅度)
        """
        n = len(spectrum)
        freq = [i * sample_rate / n for i in range(n // 2)]
        magnitude = [abs(spectrum[i]) * 2 / n for i in range(n // 2)]
        
        return freq, magnitude
    
    @staticmethod
    def find_peaks(freq: List[float], magnitude: List[float], 
                  top_n: int = 5) -> List[Tuple[float, float]]:
        """找出頻譜峰值"""
        peaks = list(zip(freq, magnitude))
        peaks.sort(key=lambda x: x[1], reverse=True)
        return peaks[:top_n]


def demo_dft_vs_fft():
    """比較 DFT 和 FFT"""
    print("=" * 50)
    print("1. DFT vs FFT 比較")
    print("=" * 50)
    
    signal = [complex(math.sin(2 * math.pi * 1 * i / 8), 0) for i in range(8)]
    
    print(f"\n輸入信號: {[round(abs(s), 3) for s in signal]}")
    
    dft_result = FourierTransform.dft(signal)
    fft_result = FourierTransform.fft(signal)
    
    print(f"DFT 結果: {[round(x.real, 3) + round(x.imag, 3)*1j for x in dft_result]}")
    print(f"FFT 結果: {[round(x.real, 3) + round(x.imag, 3)*1j for x in fft_result]}")
    print("\n兩者結果應該相同")


def demo_signal_decomposition():
    """信號分解範例"""
    print("\n" + "=" * 50)
    print("2. 信號分解 - 多頻率正弦波")
    print("=" * 50)
    
    sample_rate = 1024
    duration = 0.125
    
    _, signal1 = SignalGenerator.sine(frequency=10, amplitude=1, 
                                      duration=duration, sample_rate=sample_rate)
    _, signal2 = SignalGenerator.sine(frequency=50, amplitude=0.5, 
                                      duration=duration, sample_rate=sample_rate)
    
    combined = [a + b for a, b in zip(signal1, signal2)]
    print(f"\n混合信號樣本 (前10點): {[round(x.real, 3) for x in combined[:10]]}")
    print(f"信號長度: {len(combined)} (2 的次方: {len(combined) == 128})")
    
    spectrum = FourierTransform.fft(combined)
    freq, magnitude = FrequencyAnalyzer.compute_frequencies(spectrum, sample_rate)
    
    peaks = FrequencyAnalyzer.find_peaks(freq, magnitude, top_n=5)
    print(f"\n檢測到的頻率峰值:")
    for f, m in peaks:
        if m > 0.01:
            print(f"  頻率: {f:.1f} Hz, 幅度: {m:.3f}")


def demo_fft_filtering():
    """FFT 濾波範例"""
    print("\n" + "=" * 50)
    print("3. FFT 頻域濾波")
    print("=" * 50)
    
    sample_rate = 1024
    duration = 0.125
    
    _, signal = SignalGenerator.sine(frequency=10, amplitude=1, 
                                     duration=duration, sample_rate=sample_rate)
    
    noise = SignalGenerator.noise(len(signal), amplitude=0.3)
    noisy_signal = [a + b for a, b in zip(signal, noise)]
    
    spectrum = FourierTransform.fft(noisy_signal)
    n = len(spectrum)
    
    threshold = 15
    filtered_spectrum = [s if i < threshold or i > n - threshold else 0 
                        for i, s in enumerate(spectrum)]
    
    filtered = FourierTransform.ifft(filtered_spectrum)
    
    print(f"\n原始信號: {round(abs(signal[50]), 3)}")
    print(f"雜訊信號: {round(abs(noisy_signal[50]), 3)}")
    print(f"濾波後: {round(abs(filtered[50].real), 3)}")


def demo_compression():
    """壓縮範例 - 保留主要頻率"""
    print("\n" + "=" * 50)
    print("4. 基於 FFT 的信號壓縮")
    print("=" * 50)
    
    sample_rate = 1024
    duration = 0.125
    
    _, signal = SignalGenerator.sine(frequency=20, amplitude=1, 
                                     duration=duration, sample_rate=sample_rate)
    
    spectrum = FourierTransform.fft(signal)
    n = len(spectrum)
    
    compression_ratios = [0.1, 0.25, 0.5]
    
    for ratio in compression_ratios:
        keep = int(n * ratio)
        compressed = [s if i < keep or i > n - keep else 0 for i, s in enumerate(spectrum)]
        reconstructed = FourierTransform.ifft(compressed)
        
        error = sum(abs(a.real - b.real) for a, b in zip(signal, reconstructed)) / n
        print(f"\n壓縮比 {ratio:.0%}: 保留 {keep} 個頻率係數")
        print(f"  重構誤差: {error:.6f}")


def demo_square_wave():
    """方波分解範例"""
    print("\n" + "=" * 50)
    print("5. 方波的傅立葉級數近似")
    print("=" * 50)
    
    n = 128
    signal = [complex(1 if math.sin(2 * math.pi * 1 * i / n) >= 0 else -1, 0) 
              for i in range(n)]
    
    spectrum = FourierTransform.fft(signal)
    freq, magnitude = FrequencyAnalyzer.compute_frequencies(spectrum, 1)
    
    print(f"\n方波的前10個諧波幅度:")
    for i in range(1, min(11, len(magnitude))):
        if magnitude[i] > 0.01:
            print(f"  第 {i} 階諧波: {magnitude[i]:.3f}")


def demo_phase():
    """相位譜範例"""
    print("\n" + "=" * 50)
    print("6. 相位譜分析")
    print("=" * 50)
    
    sample_rate = 1024
    _, signal = SignalGenerator.sine(frequency=30, amplitude=1, 
                                      duration=0.125, sample_rate=sample_rate)
    
    spectrum = FourierTransform.fft(signal)
    
    n = len(spectrum)
    phase = [cmath.phase(spectrum[i]) for i in range(n // 2)]
    
    print(f"\n前10個頻率的相位:")
    for i in range(1, min(11, len(phase))):
        print(f"  f={i*sample_rate/n:.0f}Hz: {phase[i]:.3f} rad ({math.degrees(phase[i]):.1f}°)")


if __name__ == "__main__":
    print("傅立葉變換 Python 範例")
    print("=" * 50)
    
    demo_dft_vs_fft()
    demo_signal_decomposition()
    demo_fft_filtering()
    demo_compression()
    demo_square_wave()
    demo_phase()
    
    print("\n" + "=" * 50)
    print("所有範例執行完成！")
