import numpy as np

def unbroadcast(grad, shape):
    """處理 NumPy 廣播機制在反向傳遞時的梯度還原"""
    if grad.shape == shape:
        return grad
    ndim_diff = grad.ndim - len(shape)
    if ndim_diff > 0:
        grad = grad.sum(axis=tuple(range(ndim_diff)))
    for i, dim in enumerate(shape):
        if dim == 1 and grad.shape[i] > 1:
            grad = grad.sum(axis=i, keepdims=True)
    return grad

class Tensor:
    def __init__(self, data, _children=(), requires_grad=False):
        self.data = np.array(data, dtype=np.float32)
        self.grad = np.zeros_like(self.data)
        self._backward = lambda: None
        self._prev = set(_children)
        self.requires_grad = requires_grad

    def zero_grad(self):
        self.grad = np.zeros_like(self.data)

    def backward(self):
        topo = []
        visited = set()
        def build_topo(v):
            if v not in visited:
                visited.add(v)
                for child in v._prev:
                    build_topo(child)
                topo.append(v)
        build_topo(self)
        
        self.grad = np.ones_like(self.data)
        for v in reversed(topo):
            v._backward()

    # --- 基礎運算 ---
    def __add__(self, other):
        other = other if isinstance(other, Tensor) else Tensor(other)
        out = Tensor(self.data + other.data, (self, other), self.requires_grad or other.requires_grad)
        def _backward():
            self.grad += unbroadcast(out.grad, self.data.shape)
            other.grad += unbroadcast(out.grad, other.data.shape)
        out._backward = _backward
        return out

    def __mul__(self, other):
        other = other if isinstance(other, Tensor) else Tensor(other)
        out = Tensor(self.data * other.data, (self, other), self.requires_grad or other.requires_grad)
        def _backward():
            self.grad += unbroadcast(out.grad * other.data, self.data.shape)
            other.grad += unbroadcast(out.grad * self.data, other.data.shape)
        out._backward = _backward
        return out

    def __matmul__(self, other):
        other = other if isinstance(other, Tensor) else Tensor(other)
        out = Tensor(self.data @ other.data, (self, other), self.requires_grad or other.requires_grad)
        def _backward():
            self.grad += unbroadcast(out.grad @ np.swapaxes(other.data, -1, -2), self.data.shape)
            other.grad += unbroadcast(np.swapaxes(self.data, -1, -2) @ out.grad, other.data.shape)
        out._backward = _backward
        return out

    # --- 張量形狀操作 ---
    def transpose(self, ax1, ax2):
        out = Tensor(np.swapaxes(self.data, ax1, ax2), (self,), self.requires_grad)
        def _backward():
            self.grad += np.swapaxes(out.grad, ax1, ax2)
        out._backward = _backward
        return out

    def reshape(self, *shape):
        out = Tensor(self.data.reshape(*shape), (self,), self.requires_grad)
        def _backward():
            self.grad += out.grad.reshape(self.data.shape)
        out._backward = _backward
        return out

    # --- 激勵與非線性函數 ---
    def relu(self):
        out = Tensor(np.maximum(0, self.data), (self,), self.requires_grad)
        def _backward():
            self.grad += out.grad * (self.data > 0)
        out._backward = _backward
        return out

    def masked_fill(self, mask, value):
        # 使用 np.where 完美支援 Broadcasting，解決 boolean index 的形狀衝突
        out_data = np.where(mask, value, self.data)
        out = Tensor(out_data, (self,), self.requires_grad)
        def _backward():
            # 反向傳遞時，被遮蔽（填入 -inf）的地方梯度為 0
            self.grad += np.where(mask, 0, out.grad)
        out._backward = _backward
        return out

    def softmax(self, axis=-1):
        # 為了穩定性減去最大值
        max_val = np.max(self.data, axis=axis, keepdims=True)
        exps = np.exp(self.data - max_val)
        probs = exps / np.sum(exps, axis=axis, keepdims=True)
        out = Tensor(probs, (self,), self.requires_grad)
        def _backward():
            # Softmax 的 Jacobian-vector product
            s = out.data
            grad_s = out.grad
            self.grad += s * (grad_s - np.sum(grad_s * s, axis=axis, keepdims=True))
        out._backward = _backward
        return out

    def cross_entropy(self, targets):
        """融合 Softmax 與 Cross Entropy 以提高數值穩定性"""
        logits = self.data
        max_logits = np.max(logits, axis=-1, keepdims=True)
        exps = np.exp(logits - max_logits)
        probs = exps / np.sum(exps, axis=-1, keepdims=True)
        
        # 取得目標類別的機率
        batch_size, seq_len = targets.shape
        loss = 0.0
        for b in range(batch_size):
            for t in range(seq_len):
                loss -= np.log(probs[b, t, targets[b, t]] + 1e-10)
        loss = loss / (batch_size * seq_len)
        
        out = Tensor(loss, (self,), self.requires_grad)
        def _backward():
            d_logits = probs.copy()
            for b in range(batch_size):
                for t in range(seq_len):
                    d_logits[b, t, targets[b, t]] -= 1
            d_logits = d_logits / (batch_size * seq_len)
            self.grad += out.grad * d_logits
        out._backward = _backward
        return out

    # Python Magic Methods
    def __sub__(self, other): return self + (other * -1)
    def __truediv__(self, other): return self * (other ** -1)
    def __radd__(self, other): return self + other
    def __rmul__(self, other): return self * other
    def __pow__(self, power):
        out = Tensor(self.data ** power, (self,), self.requires_grad)
        def _backward():
            self.grad += out.grad * (power * self.data ** (power - 1))
        out._backward = _backward
        return out

# 請加入到 engine.py 的最下方
def cat(tensors, axis=0):
    """將多個 Tensor 沿著指定維度拼接起來"""
    data = np.concatenate([t.data for t in tensors], axis=axis)
    requires_grad = any(t.requires_grad for t in tensors)
    out = Tensor(data, tuple(tensors), requires_grad)
    
    def _backward():
        if not out.requires_grad: return
        # 沿著拼接的維度切分梯度，並分配給原本的張量
        split_sizes = [t.data.shape[axis] for t in tensors]
        grads = np.split(out.grad, np.cumsum(split_sizes)[:-1], axis=axis)
        for t, g in zip(tensors, grads):
            if t.requires_grad:
                t.grad += g
    out._backward = _backward
    return out