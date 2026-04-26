# 生成式 AI (Generative AI)

## 概述

生成式 AI 是能夠創造新內容（文字、圖像、音訊、影片、程式碼等）的人工智慧技術，基於深度學習模型從大量訓練資料中學習生成的模式。

## 生成式 AI 類型

| 類型 | 輸出 | 代表模型 |
|------|------|----------|
| 文字生成 | 文章、程式碼、對話 | GPT, Claude, Gemini |
| 圖像生成 | 圖片、藝術品 | DALL-E, Midjourney, Stable Diffusion |
| 音訊生成 | 音樂、語音 | AudioGen, MusicLM |
| 影片生成 | 影片、動畫 | Sora, Runway |
| 3D 生成 | 3D 模型 | Point-E, DreamFusion |

## 生成對抗網路 (GAN)

### 架構

```
┌─────────────────────────────────────────────────────────────┐
│                      GAN 架構                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   隨機噪音 ──→ ┌─────────┐                               │
│                 │ Generator │ ←── 目標：欺騙判別器          │
│                 └────┬─────┘                               │
│                      │                                      │
│                      │ 生成資料                              │
│                      ↓                                      │
│               ┌──────────┐                                  │
│  真實資料 ──→ │ Discriminator │ ←── 目標：區分真假           │
│               └──────────┘                                  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Python 實現

```python
import torch
import torch.nn as nn

class Generator(nn.Module):
    def __init__(self, latent_dim, img_shape):
        super().__init__()
        self.img_shape = img_shape
        
        def block(in_feat, out_feat, normalize=True):
            layers = [nn.Linear(in_feat, out_feat)]
            if normalize:
                layers.append(nn.BatchNorm1d(out_feat))
            layers.append(nn.LeakyReLU(0.2, inplace=True))
            return layers
        
        self.model = nn.Sequential(
            *block(latent_dim, 128, normalize=False),
            *block(128, 256),
            *block(256, 512),
            *block(512, 1024),
            nn.Linear(1024, int(torch.prod(torch.tensor(img_shape)))),
            nn.Tanh()
        )
    
    def forward(self, z):
        img = self.model(z)
        return img.view(img.size(0), *self.img_shape)

class Discriminator(nn.Module):
    def __init__(self, img_shape):
        super().__init__()
        
        self.model = nn.Sequential(
            nn.Linear(int(torch.prod(torch.tensor(img_shape)), 512),
            nn.LeakyReLU(0.2),
            nn.Linear(512, 256),
            nn.LeakyReLU(0.2),
            nn.Linear(256, 1),
            nn.Sigmoid()
        )
    
    def forward(self, img):
        img_flat = img.view(img.size(0), -1)
        return self.model(img_flat)
```

## 變分自編碼器 (VAE)

### 原理

```python
class VAE(nn.Module):
    def __init__(self, latent_dim):
        super().__init__()
        self.latent_dim = latent_dim
        
        # 編碼器
        self.encoder = nn.Sequential(
            nn.Linear(784, 512),
            nn.ReLU(),
            nn.Linear(512, 256)
        )
        self.fc_mu = nn.Linear(256, latent_dim)
        self.fc_logvar = nn.Linear(256, latent_dim)
        
        # 解碼器
        self.decoder = nn.Sequential(
            nn.Linear(latent_dim, 256),
            nn.ReLU(),
            nn.Linear(256, 512),
            nn.ReLU(),
            nn.Linear(512, 784),
            nn.Sigmoid()
        )
    
    def encode(self, x):
        h = self.encoder(x)
        mu = self.fc_mu(h)
        logvar = self.fc_logvar(h)
        return mu, logvar
    
    def reparameterize(self, mu, logvar):
        std = torch.exp(0.5 * logvar)
        eps = torch.randn_like(std)
        return mu + eps * std
    
    def decode(self, z):
        return self.decoder(z)
    
    def forward(self, x):
        mu, logvar = self.encode(x.view(-1, 784))
        z = self.reparameterize(mu, logvar)
        return self.decode(z), mu, logvar
```

## 擴散模型 (Diffusion Model)

### 前向過程（逐步加入噪音）

```python
def forward_diffusion(x0, t, device):
    """前向擴散：加入噪音"""
    noise = torch.randn_like(x0)
    alphas_cumprod = get_alphas_cumprod(device)
    
    sqrt_alphas_cumprod_t = alphas_cumprod[t] ** 0.5
    sqrt_one_minus_alphas_cumprod_t = (1 - alphas_cumprod[t]) ** 0.5
    
    return sqrt_alphas_cumprod_t * x0 + \
           sqrt_one_minus_alphas_cumprod_t * noise, noise
```

### 反向過程（去噪音）

```python
class UNet(nn.Module):
    def forward(self, x, t):
        # 時間編碼
        t_embed = time_embedding(t)
        # U-Net 架構
        x = self.encoder(x, t_embed)
        x = self.decoder(x, t_embed)
        return x

@torch.no_grad()
def reverse_diffusion(model, noisy_img, timesteps, device):
    """反向擴散：逐步去噪音"""
    for i in reversed(timesteps):
        t = torch.full((noisy_img.size(0),), i, device=device)
        predicted_noise = model(noisy_img, t)
        alphas_cumprod = get_alphas_cumprod(device)
        alpha = alphas_cumprod[i]
        noisy_img = (noisy_img - (1-alpha)**0.5 * predicted_noise) / alpha**0.5
    return noisy_img
```

## 文字生成圖像

### Stable Diffusion 架構

```
文字提示 → CLIP Text Encoder → 文字嵌入
                                    ↓
隨機噪音 ──→ Latent Diffusion Model ──→ VAE 解碼 → 圖像
                 ↑
           交叉注意力
```

### 使用方式

```python
from diffusers import StableDiffusionPipeline

pipe = StableDiffusionPipeline.from_pretrained(
    "runwayml/stable-diffusion-v1-5"
)

prompt = "a beautiful sunset over the ocean"
image = pipe(prompt).images[0]
image.save("output.png")
```

## 文字生成影片

### 模型架構

```
文字提示 → 文字編碼 → 時空 Transformer ──→ 影片
                         ↑
                      影像先驗
```

## 應用場景

| 領域 | 應用 |
|------|------|
| 藝術創作 | AI 生成藝術品、設計素材 |
| 遊戲開發 | 遊戲關卡、角色自動生成 |
| 電影特效 | 場景增強、角色動畫 |
| 醫學影像 | 合成訓練資料 |
| 產品設計 | 快速原型生成 |
| 教育 | 互動式學習內容 |

## 倫理考量

| 問題 | 說明 |
|------|------|
| 深度偽造 | 人臉生成可能被濫用 |
| 版權爭議 | 訓練資料的版權問題 |
| 幻覺問題 | 生成虛假資訊 |
| 偏見放大 | 訓練資料中的偏見 |

## 相關資源

- 相關概念：[大型語言模型](大型語言模型.md)
- 相關概念：[深度學習](深度學習.md)

## Tags

#生成式AI #GAN #VAE #Diffusion #AI
