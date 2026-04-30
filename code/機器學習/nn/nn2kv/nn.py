import numpy as np
from engine import Tensor

class Module:
    def parameters(self):
        params = []
        for v in self.__dict__.values():
            if isinstance(v, Tensor) and v.requires_grad:
                params.append(v)
            elif isinstance(v, Module):
                params.extend(v.parameters())
            elif isinstance(v, list):
                for item in v:
                    if isinstance(item, Module):
                        params.extend(item.parameters())
        return params

class Linear(Module):
    def __init__(self, in_features, out_features, bias=False):
        std = 0.08
        self.weight = Tensor(np.random.normal(0, std, (in_features, out_features)), requires_grad=True)
        self.bias = Tensor(np.zeros((out_features,)), requires_grad=True) if bias else None

    def __call__(self, x):
        out = x @ self.weight
        if self.bias is not None:
            out = out + self.bias
        return out

class Embedding(Module):
    def __init__(self, num_embeddings, embedding_dim):
        self.weight = Tensor(np.random.normal(0, 0.08, (num_embeddings, embedding_dim)), requires_grad=True)

    def __call__(self, indices): # indices 為 NumPy 陣列
        out_data = self.weight.data[indices]
        out = Tensor(out_data, (self.weight,), requires_grad=True)
        def _backward():
            np.add.at(self.weight.grad, indices, out.grad)
        out._backward = _backward
        return out

class RMSNorm(Module):
    def __init__(self, dim, eps=1e-5):
        self.eps = eps
        # 由於原始程式無參數，這裡我們也省略 gamma 參數或將其固定
        self.scale = Tensor(np.ones(dim), requires_grad=False)

    def __call__(self, x):
        # 變異數計算 x / sqrt(mean(x^2))
        ms = (x ** 2)
        # 用手動方式寫出 sum 避免複雜 graph，但在這裡為了簡化，直接用 numpy 前向/後向
        out_data = x.data * (np.mean(x.data**2, axis=-1, keepdims=True) + self.eps) ** -0.5
        out = Tensor(out_data, (x,), requires_grad=True)
        
        def _backward():
            N = x.data.shape[-1]
            ms = np.mean(x.data**2, axis=-1, keepdims=True) + self.eps
            inv_std = ms ** -0.5
            dx = (out.grad * inv_std) - (x.data * inv_std**3 * np.sum(out.grad * x.data, axis=-1, keepdims=True) / N)
            x.grad += dx
            
        out._backward = _backward
        return out

class Adam:
    def __init__(self, params, lr=0.01, betas=(0.85, 0.99), eps=1e-8):
        self.params = params
        self.lr = lr
        self.beta1, self.beta2 = betas
        self.eps = eps
        self.m = [np.zeros_like(p.data) for p in params]
        self.v = [np.zeros_like(p.data) for p in params]
        self.t = 0

    def step(self):
        self.t += 1
        for i, p in enumerate(self.params):
            self.m[i] = self.beta1 * self.m[i] + (1 - self.beta1) * p.grad
            self.v[i] = self.beta2 * self.v[i] + (1 - self.beta2) * (p.grad ** 2)
            m_hat = self.m[i] / (1 - self.beta1 ** self.t)
            v_hat = self.v[i] / (1 - self.beta2 ** self.t)
            p.data -= self.lr * m_hat / (np.sqrt(v_hat) + self.eps)

    def zero_grad(self):
        for p in self.params:
            p.zero_grad()