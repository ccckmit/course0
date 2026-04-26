# DDPM MNIST — 散射網路手寫數字生成

使用 **DDPM (Denoising Diffusion Probabilistic Model)** 在 MNIST 資料集上訓練，
並支援 **DDIM 快速採樣**。

## 環境需求

```bash
pip install torch torchvision tqdm matplotlib
```

---

## 快速開始

### 1. 訓練模型
```bash
python train.py
```

| 參數         | 預設值  | 說明              |
|--------------|---------|-------------------|
| `EPOCHS`     | 30      | 訓練週期           |
| `BATCH_SIZE` | 128     | 批次大小           |
| `T`          | 1000    | 擴散步數           |
| `LR`         | 2e-4    | 學習率             |

訓練時每 5 個 epoch 會在 `samples_train/` 儲存一張 4×4 的樣本圖。

---

### 2. 生成圖片
```bash
# 全部模式（DDPM + DDIM + 去噪過程）
python generate.py

# 只做 DDIM 快速採樣（50 步，遠快於 DDPM 的 1000 步）
python generate.py --mode ddim --ddim_steps 50 --n 64

# DDPM 完整採樣
python generate.py --mode ddpm --n 16

# 視覺化去噪過程
python generate.py --mode denoise
```

| 參數          | 說明                          |
|---------------|-------------------------------|
| `--mode`      | `ddpm` / `ddim` / `denoise` / `all` |
| `--n`         | 生成數量（預設 64）            |
| `--ddim_steps`| DDIM 採樣步數（預設 50）       |
| `--eta`       | DDIM η 值（0=確定性，1=隨機） |
| `--ckpt`      | checkpoint 路徑               |

---

## 架構說明

```
純噪聲 x_T  ──→  逆向去噪 (UNet)  ──→  生成圖片 x_0
               ↑
          時間步嵌入 t
```

### 訓練（前向擴散）
```
x_0  →  加噪 T 步  →  x_T（純高斯噪聲）
UNet 學習預測每步加入的噪聲 ε
損失：MSE(ε_predicted, ε_true)
```

### 生成（逆向去噪）
```
x_T（隨機噪聲）→  UNet 逐步去噪  →  x_0（清晰數字）
```

### UNet 結構
- **編碼器**：3 層 ResBlock + Downsample（28→14→7）
- **瓶頸**：2 個 ResBlock
- **解碼器**：Upsample + Skip Connection（7→14→28）
- **時間條件**：Sinusoidal PE → MLP → 注入每個 ResBlock

### DDIM vs DDPM
| 方法  | 步數  | 速度   | 確定性 |
|-------|-------|--------|--------|
| DDPM  | 1000  | 慢     | 隨機   |
| DDIM  | 50    | ~20×快 | 可控   |

---

## 輸出目錄

```
samples_train/   ← 訓練中的 epoch 樣本
generated/
  ├── ddpm_64imgs.png              ← DDPM 生成
  ├── ddim_50steps_64imgs.png      ← DDIM 生成
  └── denoising_process.png        ← 去噪過程視覺化
ddpm_mnist.pth                    ← 模型權重
```
