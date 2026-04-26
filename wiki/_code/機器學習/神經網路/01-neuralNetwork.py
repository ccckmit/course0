#!/usr/bin/env python3
"""
神經網路 - 感知機與多層網路範例
展示神經網路的基本結構與反向傳播
"""

import math
import random


class Perceptron:
    """感知機（單層神經網路）"""
    
    def __init__(self, n_inputs: int, learning_rate: float = 0.1):
        self.weights = [random.uniform(-1, 1) for _ in range(n_inputs)]
        self.bias = random.uniform(-1, 1)
        self.lr = learning_rate
    
    def predict(self, inputs: list) -> int:
        """前向傳播"""
        activation = self.bias
        for i, w in enumerate(self.weights):
            activation += w * inputs[i]
        return 1 if activation >= 0 else 0
    
    def train(self, X: list, y: list, epochs: int = 100):
        """訓練感知機"""
        for _ in range(epochs):
            for inputs, target in zip(X, y):
                prediction = self.predict(inputs)
                error = target - prediction
                
                # 更新權重
                for i in range(len(self.weights)):
                    self.weights[i] += self.lr * error * inputs[i]
                self.bias += self.lr * error


class NeuralNetwork:
    """多層神經網路（簡化版）"""
    
    def __init__(self, layer_sizes: list, learning_rate: float = 0.1):
        """
        layer_sizes: [輸入層大小, 隱藏層大小, 輸出層大小]
        """
        self.layers = []
        self.lr = learning_rate
        
        # 初始化權重
        for i in range(len(layer_sizes) - 1):
            # Xavier 初始化
            n = layer_sizes[i]
            m = layer_sizes[i + 1]
            weights = [[random.uniform(-1/math.sqrt(n), 1/math.sqrt(n)) 
                       for _ in range(n)] for _ in range(m)]
            biases = [random.uniform(-1, 1) for _ in range(m)]
            self.layers.append({
                'weights': weights,
                'biases': biases
            })
    
    def sigmoid(self, x: float) -> float:
        """Sigmoid 啟動函數"""
        return 1 / (1 + math.exp(-x))
    
    def sigmoid_derivative(self, x: float) -> float:
        """Sigmoid 導數"""
        s = self.sigmoid(x)
        return s * (1 - s)
    
    def forward(self, inputs: list) -> list:
        """前向傳播"""
        self.activations = [inputs]
        
        for layer in self.layers:
            new_inputs = []
            for j in range(len(layer['biases'])):
                activation = layer['biases'][j]
                for i, w in enumerate(layer['weights'][j]):
                    activation += w * self.activations[-1][i]
                new_inputs.append(self.sigmoid(activation))
            
            self.activations.append(new_inputs)
        
        return self.activations[-1]
    
    def backward(self, y_true: list) -> None:
        """反向傳播"""
        # 輸出層誤差
        deltas = []
        for j in range(len(y_true)):
            error = y_true[j] - self.activations[-1][j]
            delta = error * self.sigmoid_derivative(
                sum(self.layers[-1]['weights'][j][i] * self.activations[-2][i] 
                    for i in range(len(self.activations[-2]))) + 
                self.layers[-1]['biases'][j]
            )
            deltas.append(delta)
        
        # 更新輸出層權重
        for j in range(len(self.layers[-1]['biases'])):
            self.layers[-1]['biases'][j] += self.lr * deltas[j]
            for i in range(len(self.activations[-2])):
                self.layers[-1]['weights'][j][i] += self.lr * deltas[j] * self.activations[-2][i]
        
        # 隱藏層誤差（簡化版）
        for layer_idx in range(len(self.layers) - 2, -1, -1):
            layer = self.layers[layer_idx]
            new_deltas = []
            
            for j in range(len(layer['biases'])):
                error = sum(layer['weights'][k][j] * deltas[k] 
                           for k in range(len(deltas)))
                delta = error * self.sigmoid_derivative(0)  # 簡化
                new_deltas.append(delta)
            
            deltas = new_deltas
    
    def train(self, X: list, y: list, epochs: int = 1000):
        """訓練網路"""
        for epoch in range(epochs):
            total_error = 0
            for inputs, target in zip(X, y):
                output = self.forward(inputs)
                
                # 計算誤差
                error = sum((target[i] - output[i]) ** 2 for i in range(len(target)))
                total_error += error
                
                self.backward(target)
            
            if epoch % 100 == 0:
                print(f"Epoch {epoch}, Error: {total_error:.4f}")
    
    def predict(self, X: list) -> list:
        """預測"""
        results = []
        for inputs in X:
            output = self.forward(inputs)
            results.append(output)
        return results


class AutoGrad:
    """極簡自動微分引擎（類似 micrograd）"""
    
    def __init__(self, value: float):
        self.value = value
        self.grad = 0.0
        self._backward = lambda: None
    
    def __add__(self, other):
        other = other if isinstance(other, AutoGrad) else AutoGrad(other)
        result = AutoGrad(self.value + other.value)
        
        def _backward():
            self.grad += result.grad
            other.grad += result.grad
        result._backward = _backward
        return result
    
    def __mul__(self, other):
        other = other if isinstance(other, AutoGrad) else AutoGrad(other)
        result = AutoGrad(self.value * other.value)
        
        def _backward():
            self.grad += other.value * result.grad
            other.grad += self.value * result.grad
        result._backward = _backward
        return result
    
    def backward(self) -> None:
        """反向傳播"""
        self.grad = 1.0
        # 這裡需要拓撲排序來正確計算
        self._backward()


if __name__ == "__main__":
    # 測試感知機
    print("=== 感知機 ===")
    X = [
        [0, 0],
        [0, 1],
        [1, 0],
        [1, 1]
    ]
    y = [0, 0, 0, 1]  # AND 閘
    
    p = Perceptron(2)
    p.train(X, y, epochs=100)
    
    for inputs in X:
        print(f"{inputs} -> {p.predict(inputs)}")
    
    # 測試神經網路
    print("\n=== 神經網路 (XOR) ===")
    X_xor = [[0, 0], [0, 1], [1, 0], [1, 1]]
    y_xor = [[0], [1], [1], [0]]
    
    nn = NeuralNetwork([2, 4, 1], learning_rate=0.5)
    nn.train(X_xor, y_xor, epochs=1000)
    
    for inputs in X_xor:
        output = nn.forward(inputs)[0]
        print(f"{inputs} -> {output:.4f}")
