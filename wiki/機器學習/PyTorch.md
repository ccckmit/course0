# PyTorch

## 概述

PyTorch 是 Facebook（Meta）的人工智慧研究團隊於 2016 年開發的開源深度學習框架，迅速成為學術研究和產業開發的首選工具之一。PyTorch 的設計理念強調動態計算圖（Dynamic Computational Graph）和 Python 原生風格，讓研究者能夠以直覺的方式快速原型化、實驗和部署深度學習模型。

PyTorch 的核心優勢在於其靈活性和易用性。與 TensorFlow 1.x 的靜態圖不同，PyTorch 使用動態圖，允許在執行時動態構建網路結構，這使得調試更加直觀，程式碼更加 Pythonic。PyTorch 還提供了 TorchScript 和 ONNX 導出功能，支持模型部署到生產環境。2020 年發布的 PyTorch 2.0 引入了 torch.compile，大幅提升了效能。

## 張量操作

### 創建張量

```python
import torch
import numpy as np

# 從 Python 列表創建
t1 = torch.tensor([1, 2, 3])
t2 = torch.tensor([[1, 2, 3], [4, 5, 6]])

# 從 NumPy 創建
arr = np.array([[1, 2], [3, 4]])
t3 = torch.from_numpy(arr)

# 指定類型
t4 = torch.tensor([1, 2, 3], dtype=torch.float32)
t5 = torch.tensor([1, 2, 3], dtype=torch.long)

# 常用創建函數
t6 = torch.zeros(2, 3)           # 全零
t7 = torch.ones(2, 3)            # 全一
t8 = torch.full((2, 3), 7)      # 常數填充
t9 = torch.eye(3)               # 單位矩陣
t10 = torch.rand(2, 3)           # 均勻分佈 [0, 1)
t11 = torch.randn(2, 3)          # 標準常態分佈
t12 = torch.randint(0, 10, (2, 3))  # 整數隨機

# 從範圍
t13 = torch.arange(0, 10, 2)    # [0, 2, 4, 6, 8]
t14 = torch.linspace(0, 1, 5)    # [0, 0.25, 0.5, 0.75, 1]
```

### 張量操作

```python
# 基本運算
a = torch.tensor([[1, 2], [3, 4]])
b = torch.tensor([[5, 6], [7, 8]])

c = a + b           # 加法
c = a - b           # 減法
c = a * b           # 元素乘法
c = a / b           # 元素除法
c = a @ b           # 矩陣乘法
c = torch.matmul(a, b)

# 常用函數
c = torch.sum(a)           # 求和
c = torch.mean(a)          # 平均值
c = torch.max(a)           # 最大值
c = torch.min(a)           # 最小值
c = torch.argmax(a)        # 最大值索引
c = torch.argmin(a)        # 最小值索引

# 維度操作
reshaped = a.view(4, 1)    # 重塑 (共享記憶體)
flattened = a.flatten()    # 展平
transposed = a.T           # 轉置
squeezed = a.squeeze()    # 移除維度為 1 的軸
unsqueezed = a.unsqueeze(0)  # 添加維度為 1 的軸

# 切片
row = a[0]                 # 第一行
col = a[:, 0]              # 第一列
sub = a[0:2, 1:3]          # 子矩陣
```

### 張量屬性

```python
t = torch.randn(3, 4, 5)

print(f"形狀: {t.shape}")
print(f"維度數: {t.dim()}")
print(f"元素數: {t.numel()}")
print(f"資料類型: {t.dtype}")
print(f"設備: {t.device}")
print(f"是否梯度追蹤: {t.requires_grad}")

# 設備移動
t_cuda = t.cuda()          # 移到 GPU
t_cpu = t_cuda.cpu()       # 移回 CPU
t_mps = t.to('mps')        # Apple Silicon GPU
```

## 自動微分系統

### 基本用法

```python
# 創建需要梯度的張量
x = torch.tensor([1., 2., 3.], requires_grad=True)
y = torch.tensor([4., 5., 6.], requires_grad=True)

# 前向計算
z = x + y
w = z * 2
out = w.sum()

# 反向傳播
out.backward()

# 查看梯度
print(x.grad)  # dz/dx = 2
print(y.grad)  # dz/dy = 2

# 梯度累加：每次 backward 前需清零
x.grad.zero_()
```

### 自定義 autograd 函數

```python
class MyFunction(torch.autograd.Function):
    """自定義可微分函數"""
    
    @staticmethod
    def forward(ctx, input):
        # 保存反向傳播需要的資料
        ctx.save_for_backward(input)
        return input * 2
    
    @staticmethod
    def backward(ctx, grad_output):
        input, = ctx.saved_tensors
        # 反向傳播：grad_input = grad_output * 2
        return grad_output * 2

# 使用
result = MyFunction.apply(x)
```

### 計算圖

```python
# 計算圖示例
x = torch.tensor(1.0, requires_grad=True)
y = torch.tensor(2.0, requires_grad=True)

a = x + y          # a = 3
b = a * 2          # b = 6
c = b + y          # c = 8
c.backward()

# 梯度: dc/dx = 2, dc/dy = 3
print(x.grad)  # 2
print(y.grad)  # 3
```

## 神經網路模組

### 基類 nn.Module

```python
import torch.nn as nn

class MyNetwork(nn.Module):
    """自定義神經網路"""
    
    def __init__(self):
        super().__init__()
        
        # 定義網路層
        self.layer1 = nn.Linear(10, 20)
        self.layer2 = nn.Linear(20, 5)
        
        # 啟動函數
        self.relu = nn.ReLU()
        
        # Dropout
        self.dropout = nn.Dropout(0.2)
    
    def forward(self, x):
        x = self.layer1(x)
        x = self.relu(x)
        x = self.dropout(x)
        x = self.layer2(x)
        return x

# 使用
model = MyNetwork()
print(model)
print(list(model.parameters()))
```

### 常見層

```python
# 全連接層
linear = nn.Linear(in_features=512, out_features=256)

# 卷積層
conv1d = nn.Conv1d(in_channels=1, out_channels=16, kernel_size=3, padding=1)
conv2d = nn.Conv2d(in_channels=3, out_channels=64, kernel_size=3, padding=1)
conv3d = nn.Conv3d(in_channels=3, out_channels=64, kernel_size=3, padding=1)

# 反卷積
deconv = nn.ConvTranspose2d(3, 64, kernel_size=4, stride=2, padding=1)

# 池化層
maxpool = nn.MaxPool2d(kernel_size=2, stride=2)
avgpool = nn.AvgPool2d(kernel_size=2, stride=2)
adaptive_pool = nn.AdaptiveAvgPool2d((1, 1))

# 循環層
lstm = nn.LSTM(input_size=256, hidden_size=512, num_layers=2, batch_first=True)
gru = nn.GRU(input_size=256, hidden_size=512, num_layers=2, batch_first=True)

# 正規化層
bn = nn.BatchNorm2d(num_features=64)
ln = nn.LayerNorm(normalized_shape=256)
gn = nn.GroupNorm(num_groups=4, num_channels=64)

# 損失函數
mse_loss = nn.MSELoss()
bce_loss = nn.BCELoss()
ce_loss = nn.CrossEntropyLoss()
smooth_l1 = nn.SmoothL1Loss()
```

### 預訓練模型

```python
import torchvision.models as models

# 加載預訓練模型
resnet18 = models.resnet18(pretrained=True)
resnet50 = models.resnet50(weights='DEFAULT')
vgg16 = models.vgg16(pretrained=True)

# 修改最後一層（遷移學習）
resnet18.fc = nn.Linear(512, 10)  # 改為 10 類分類

# 凍結部分參數
for param in resnet18.parameters():
    param.requires_grad = False

# 只訓練最後一層
for param in resnet18.fc.parameters():
    param.requires_grad = True
```

## 資料處理

### Dataset 與 DataLoader

```python
from torch.utils.data import Dataset, DataLoader
import pandas as pd

class CustomDataset(Dataset):
    """自定義資料集"""
    
    def __init__(self, csv_file):
        self.data = pd.read_csv(csv_file)
    
    def __len__(self):
        return len(self.data)
    
    def __getitem__(self, idx):
        features = self.data.iloc[idx, :-1].values.astype(np.float32)
        label = self.data.iloc[idx, -1]
        
        # 轉換為張量
        features = torch.tensor(features)
        label = torch.tensor(label, dtype=torch.long)
        
        return features, label

# 使用 DataLoader
dataset = CustomDataset('data.csv')
dataloader = DataLoader(
    dataset,
    batch_size=32,
    shuffle=True,
    num_workers=4,
    pin_memory=True
)

for batch_features, batch_labels in dataloader:
    # 訓練代碼
    pass
```

### 影像資料集

```python
from torchvision import datasets, transforms

# 影像增強
transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.RandomHorizontalFlip(),
    transforms.RandomRotation(15),
    transforms.ColorJitter(brightness=0.2, contrast=0.2),
    transforms.ToTensor(),
    transforms.Normalize(mean=[0.485, 0.456, 0.406], 
                        std=[0.229, 0.224, 0.225])
])

# 載入資料集
train_dataset = datasets.CIFAR10(
    root='./data',
    train=True,
    download=True,
    transform=transform
)

test_dataset = datasets.CIFAR10(
    root='./data',
    train=False,
    download=True,
    transform=transform
)

train_loader = DataLoader(train_dataset, batch_size=64, shuffle=True)
test_loader = DataLoader(test_dataset, batch_size=64, shuffle=False)
```

## 訓練流程

### 基本訓練迴圈

```python
import torch.optim as optim

# 模型
model = MyNetwork().to('cuda')

# 損失函數
criterion = nn.CrossEntropyLoss()

# 優化器
optimizer = optim.Adam(model.parameters(), lr=0.001)

# 學習率排程
scheduler = optim.lr_scheduler.StepLR(optimizer, step_size=10, gamma=0.1)

# 訓練
num_epochs = 50

for epoch in range(num_epochs):
    model.train()
    train_loss = 0.0
    
    for batch_idx, (data, target) in enumerate(train_loader):
        # 移到 GPU
        data, target = data.to('cuda'), target.to('cuda')
        
        # 清零梯度
        optimizer.zero_grad()
        
        # 前向傳播
        output = model(data)
        
        # 計算損失
        loss = criterion(output, target)
        
        # 反向傳播
        loss.backward()
        
        # 更新參數
        optimizer.step()
        
        train_loss += loss.item()
    
    # 驗證
    model.eval()
    correct = 0
    total = 0
    
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to('cuda'), target.to('cuda')
            output = model(data)
            _, predicted = torch.max(output, 1)
            total += target.size(0)
            correct += (predicted == target).sum().item()
    
    accuracy = 100 * correct / total
    print(f'Epoch {epoch+1}: Loss={train_loss/len(train_loader):.4f}, Accuracy={accuracy:.2f}%')
    
    # 更新學習率
    scheduler.step()
```

### GPU 加速

```python
# 檢查 GPU 可用性
if torch.cuda.is_available():
    print(f"GPU: {torch.cuda.get_device_name(0)}")
    print(f"數量: {torch.cuda.device_count()}")

# 移到 GPU
device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
model = model.to(device)

# 混合精度訓練
from torch.cuda.amp import autocast, GradScaler

scaler = GradScaler()

for data, target in dataloader:
    data, target = data.to(device), target.to(device)
    
    optimizer.zero_grad()
    
    # 自動混合精度
    with autocast():
        output = model(data)
        loss = criterion(output, target)
    
    scaler.scale(loss).backward()
    scaler.step(optimizer)
    scaler.update()
```

### 模型保存與載入

```python
# 保存完整模型
torch.save(model, 'model.pth')

# 只保存參數（推薦）
torch.save(model.state_dict(), 'model_weights.pth')

# 保存優化器狀態
checkpoint = {
    'epoch': epoch,
    'model_state_dict': model.state_dict(),
    'optimizer_state_dict': optimizer.state_dict(),
    'loss': loss,
}
torch.save(checkpoint, 'checkpoint.pth')

# 載入
model = MyNetwork()
model.load_state_dict(torch.load('model_weights.pth'))
model.eval()

# 從 checkpoint 恢復
checkpoint = torch.load('checkpoint.pth')
model.load_state_dict(checkpoint['model_state_dict'])
optimizer.load_state_dict(checkpoint['optimizer_state_dict'])
start_epoch = checkpoint['epoch']
```

## 模型評估與推理

### 推理模式

```python
model.eval()

# 單一輸入
input_tensor = torch.randn(1, 10).to(device)

# 關閉梯度
with torch.no_grad():
    output = model(input_tensor)
    probabilities = torch.softmax(output, dim=1)
    predicted_class = torch.argmax(probabilities, dim=1)
```

### 評估指標

```python
from sklearn.metrics import classification_report, confusion_matrix

def evaluate_model(model, test_loader, device):
    model.eval()
    all_preds = []
    all_labels = []
    
    with torch.no_grad():
        for data, target in test_loader:
            data, target = data.to(device), target.to(device)
            output = model(data)
            _, predicted = torch.max(output, 1)
            
            all_preds.extend(predicted.cpu().numpy())
            all_labels.extend(target.cpu().numpy())
    
    # 分類報告
    print(classification_report(all_labels, all_preds))
    
    # 混淆矩陣
    print(confusion_matrix(all_labels, all_preds))
```

## 進階功能

### 分布式訓練

```python
import torch.distributed as dist
from torch.nn.parallel import DistributedDataParallel as DDP

# 初始化
dist.init_process_group(backend='nccl')

# 包裝模型
model = model.to(device)
model = DDP(model, device_ids=[local_rank])

# 數據分發
train_sampler = torch.utils.data.distributed.DistributedSampler(dataset)
train_loader = DataLoader(dataset, sampler=train_sampler)
```

### 梯度裁剪

```python
# 防止梯度爆炸
torch.nn.utils.clip_grad_norm_(model.parameters(), max_norm=1.0)
```

### 自定義損失函數

```python
class FocalLoss(nn.Module):
    """Focal Loss - 處理類別不平衡"""
    
    def __init__(self, alpha=1, gamma=2):
        super().__init__()
        self.alpha = alpha
        self.gamma = gamma
    
    def forward(self, inputs, targets):
        ce_loss = nn.functional.cross_entropy(inputs, targets, reduction='none')
        pt = torch.exp(-ce_loss)
        focal_loss = self.alpha * (1 - pt) ** self.gamma * ce_loss
        return focal_loss.mean()
```

### 訓練回調

```python
from torch.utils.callbacks import EarlyStopping, ModelCheckpoint

early_stopping = EarlyStopping(patience=10, restore_best_weights=True)
checkpoint = ModelCheckpoint('best_model.pth', monitor='val_loss')

# 使用
for epoch in range(num_epochs):
    # 訓練...
    early_stopping(val_loss, model)
    if early_stopping.early_stop:
        break
```

## 部署與導出

### TorchScript

```python
# 追蹤
model.eval()
traced_model = torch.jit.trace(model, example_input)

# 腳本化
scripted_model = torch.jit.script(model)

# 保存
traced_model.save('model_traced.pt')
scripted_model.save('model_scripted.pt')
```

### ONNX 導出

```python
# 導出為 ONNX
torch.onnx.export(
    model,
    example_input,
    'model.onnx',
    export_params=True,
    opset_version=11,
    do_constant_folding=True,
    input_names=['input'],
    output_names=['output'],
    dynamic_axes={'input': {0: 'batch_size'}, 'output': {0: 'batch_size'}}
)
```

## 生態系統

| 領域 | 庫/工具 |
|------|---------|
| 電腦視覺 | torchvision, timm, OpenMMLab |
| 自然語言處理 | transformers, torchtext |
| 深度學習框架 | PyTorch Lightning, Ignite |
| 模型部署 | TorchServe, ONNX Runtime |
| 量化壓縮 | PyTorch Quantization, ONNX |

## 相關概念

- [深度學習](深度學習.md) - 深度學習理論
- [TensorFlow](TensorFlow.md) - 另一個深度學習框架
- [神經網路](神經網路.md) - 神經網路基礎

## Tags

#PyTorch #深度學習 #機器學習 #Tensor #類神經網路