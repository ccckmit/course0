"""
train.py — DDPM (Denoising Diffusion Probabilistic Model) on MNIST
使用簡單的 UNet 架構，在 MNIST 資料集上訓練散射模型。
"""

import os
import math
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.utils.data import DataLoader
from torchvision import datasets, transforms
from torchvision.utils import save_image
from tqdm import tqdm

# ─────────────────────────────────────────
# 超參數
# ─────────────────────────────────────────
# DEVICE       = "cuda" if torch.cuda.is_available() else "cpu"
DEVICE       = "cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu"
IMG_SIZE     = 28
BATCH_SIZE   = 128
EPOCHS       = 5 #30
LR           = 2e-4
T            = 1000          # 總擴散步數
BETA_START   = 1e-4
BETA_END     = 0.02
CHECKPOINT   = "ddpm_mnist.pth"
SAMPLE_DIR   = "samples_train"
os.makedirs(SAMPLE_DIR, exist_ok=True)

# ─────────────────────────────────────────
# 噪聲排程 (Linear Schedule)
# ─────────────────────────────────────────
def make_schedule(T, beta_start, beta_end, device):
    betas  = torch.linspace(beta_start, beta_end, T, device=device)
    alphas = 1.0 - betas
    alpha_bar = torch.cumprod(alphas, dim=0)                    # ᾱ_t
    alpha_bar_prev = F.pad(alpha_bar[:-1], (1, 0), value=1.0)  # ᾱ_{t-1}
    sqrt_alpha_bar      = alpha_bar.sqrt()
    sqrt_one_minus_ab   = (1 - alpha_bar).sqrt()
    posterior_variance  = betas * (1 - alpha_bar_prev) / (1 - alpha_bar)
    return {
        "betas":              betas,
        "alphas":             alphas,
        "alpha_bar":          alpha_bar,
        "alpha_bar_prev":     alpha_bar_prev,
        "sqrt_alpha_bar":     sqrt_alpha_bar,
        "sqrt_one_minus_ab":  sqrt_one_minus_ab,
        "posterior_variance": posterior_variance,
    }

# ─────────────────────────────────────────
# Sinusoidal 時間步嵌入
# ─────────────────────────────────────────
class SinusoidalPE(nn.Module):
    def __init__(self, dim):
        super().__init__()
        self.dim = dim

    def forward(self, t):
        half = self.dim // 2
        freq = torch.exp(
            -math.log(10000) * torch.arange(half, device=t.device) / (half - 1)
        )
        emb = t[:, None].float() * freq[None]
        return torch.cat([emb.sin(), emb.cos()], dim=-1)  # (B, dim)

# ─────────────────────────────────────────
# 基本殘差區塊 (含時間條件)
# ─────────────────────────────────────────
def get_groups(channels):
    """自動選擇能整除 channels 的最大 num_groups（上限 8）"""
    for g in [8, 4, 2, 1]:
        if channels % g == 0:
            return g

class ResBlock(nn.Module):
    def __init__(self, in_ch, out_ch, time_dim):
        super().__init__()
        self.norm1 = nn.GroupNorm(get_groups(in_ch), in_ch)
        self.conv1 = nn.Conv2d(in_ch, out_ch, 3, padding=1)
        self.norm2 = nn.GroupNorm(get_groups(out_ch), out_ch)
        self.conv2 = nn.Conv2d(out_ch, out_ch, 3, padding=1)
        self.time_proj = nn.Linear(time_dim, out_ch)
        self.skip = nn.Conv2d(in_ch, out_ch, 1) if in_ch != out_ch else nn.Identity()

    def forward(self, x, t_emb):
        h = self.conv1(F.silu(self.norm1(x)))
        h = h + self.time_proj(F.silu(t_emb))[:, :, None, None]
        h = self.conv2(F.silu(self.norm2(h)))
        return h + self.skip(x)

# ─────────────────────────────────────────
# 簡易 UNet（適合 28×28）
# ─────────────────────────────────────────
class UNet(nn.Module):
    def __init__(self, in_ch=1, base_ch=64, time_dim=256):
        super().__init__()
        self.time_dim = time_dim
        self.pe  = SinusoidalPE(time_dim)
        self.t_mlp = nn.Sequential(
            nn.Linear(time_dim, time_dim * 4),
            nn.SiLU(),
            nn.Linear(time_dim * 4, time_dim),
        )

        # 編碼器
        self.enc1 = ResBlock(in_ch,       base_ch,     time_dim)   # 28×28
        self.enc2 = ResBlock(base_ch,     base_ch * 2, time_dim)   # 14×14
        self.enc3 = ResBlock(base_ch * 2, base_ch * 4, time_dim)   # 7×7

        self.down1 = nn.Conv2d(base_ch,     base_ch,     4, 2, 1)
        self.down2 = nn.Conv2d(base_ch * 2, base_ch * 2, 4, 2, 1)

        # 瓶頸
        self.mid1  = ResBlock(base_ch * 4, base_ch * 4, time_dim)
        self.mid2  = ResBlock(base_ch * 4, base_ch * 4, time_dim)

        # 解碼器
        self.up2   = nn.ConvTranspose2d(base_ch * 4, base_ch * 2, 4, 2, 1)
        self.dec2  = ResBlock(base_ch * 4, base_ch * 2, time_dim)

        self.up1   = nn.ConvTranspose2d(base_ch * 2, base_ch,     4, 2, 1)
        self.dec1  = ResBlock(base_ch * 2, base_ch,     time_dim)

        self.out   = nn.Conv2d(base_ch, in_ch, 1)

    def forward(self, x, t):
        t_emb = self.t_mlp(self.pe(t))   # (B, time_dim)

        e1 = self.enc1(x,            t_emb)   # (B,  64, 28, 28)
        e2 = self.enc2(self.down1(e1), t_emb) # (B, 128, 14, 14)
        e3 = self.enc3(self.down2(e2), t_emb) # (B, 256,  7,  7)

        m  = self.mid2(self.mid1(e3, t_emb), t_emb)

        d2 = self.dec2(torch.cat([self.up2(m),  e2], dim=1), t_emb)
        d1 = self.dec1(torch.cat([self.up1(d2), e1], dim=1), t_emb)

        return self.out(d1)

# ─────────────────────────────────────────
# 前向擴散 q(x_t | x_0)
# ─────────────────────────────────────────
def q_sample(x0, t, schedule):
    noise = torch.randn_like(x0)
    s_ab  = schedule["sqrt_alpha_bar"][t][:, None, None, None]
    s_om  = schedule["sqrt_one_minus_ab"][t][:, None, None, None]
    return s_ab * x0 + s_om * noise, noise

# ─────────────────────────────────────────
# 損失函數 (簡單 L2 噪聲預測)
# ─────────────────────────────────────────
def compute_loss(model, x0, schedule):
    B = x0.shape[0]
    t = torch.randint(0, T, (B,), device=x0.device)
    x_t, noise = q_sample(x0, t, schedule)
    predicted  = model(x_t, t)
    return F.mse_loss(predicted, noise)

# ─────────────────────────────────────────
# 逆向採樣 p(x_{t-1} | x_t)
# ─────────────────────────────────────────
@torch.no_grad()
def p_sample(model, x, t_scalar, schedule):
    B = x.shape[0]
    t_tensor = torch.full((B,), t_scalar, device=x.device, dtype=torch.long)

    beta_t    = schedule["betas"][t_scalar]
    alpha_t   = schedule["alphas"][t_scalar]
    ab_t      = schedule["alpha_bar"][t_scalar]
    ab_prev_t = schedule["alpha_bar_prev"][t_scalar]

    eps = model(x, t_tensor)
    x0_hat = (x - (1 - ab_t).sqrt() * eps) / ab_t.sqrt()
    x0_hat = x0_hat.clamp(-1, 1)

    mean = (ab_prev_t.sqrt() * beta_t / (1 - ab_t)) * x0_hat + \
           (alpha_t.sqrt() * (1 - ab_prev_t) / (1 - ab_t)) * x

    if t_scalar > 0:
        noise = torch.randn_like(x)
        var   = schedule["posterior_variance"][t_scalar]
        x = mean + var.sqrt() * noise
    else:
        x = mean
    return x

@torch.no_grad()
def generate_samples(model, schedule, n=16, device=DEVICE):
    model.eval()
    x = torch.randn(n, 1, IMG_SIZE, IMG_SIZE, device=device)
    for step in tqdm(reversed(range(T)), desc="Sampling", total=T, leave=False):
        x = p_sample(model, x, step, schedule)
    model.train()
    return x

# ─────────────────────────────────────────
# 主訓練迴圈
# ─────────────────────────────────────────
def main():
    print(f"Device: {DEVICE}")

    # 資料集
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize([0.5], [0.5]),   # [-1, 1]
    ])
    dataset    = datasets.MNIST("./data", train=True, download=True, transform=transform)
    dataloader = DataLoader(dataset, batch_size=BATCH_SIZE, shuffle=True,
                            num_workers=2, pin_memory=True)

    # 模型與優化器
    model     = UNet().to(DEVICE)
    optimizer = torch.optim.AdamW(model.parameters(), lr=LR)
    scheduler_lr = torch.optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=EPOCHS)
    schedule  = make_schedule(T, BETA_START, BETA_END, DEVICE)

    # 載入已有的 checkpoint
    start_epoch = 0
    if os.path.exists(CHECKPOINT):
        ckpt = torch.load(CHECKPOINT, map_location=DEVICE)
        model.load_state_dict(ckpt["model"])
        optimizer.load_state_dict(ckpt["optimizer"])
        start_epoch = ckpt["epoch"] + 1
        print(f"Resumed from epoch {start_epoch}")

    total_params = sum(p.numel() for p in model.parameters()) / 1e6
    print(f"Model parameters: {total_params:.2f}M")

    for epoch in range(start_epoch, EPOCHS):
        model.train()
        total_loss = 0.0
        pbar = tqdm(dataloader, desc=f"Epoch {epoch+1}/{EPOCHS}")

        for x0, _ in pbar:
            x0 = x0.to(DEVICE)
            optimizer.zero_grad()
            loss = compute_loss(model, x0, schedule)
            loss.backward()
            nn.utils.clip_grad_norm_(model.parameters(), 1.0)
            optimizer.step()

            total_loss += loss.item()
            pbar.set_postfix(loss=f"{loss.item():.4f}")

        avg_loss = total_loss / len(dataloader)
        scheduler_lr.step()
        print(f"Epoch {epoch+1:3d} | avg loss: {avg_loss:.4f} | lr: {scheduler_lr.get_last_lr()[0]:.6f}")

        # 每 5 個 epoch 儲存樣本
        if (epoch + 1) % 5 == 0 or epoch == 0:
            samples = generate_samples(model, schedule, n=16)
            samples = (samples * 0.5 + 0.5).clamp(0, 1)
            save_image(samples, f"{SAMPLE_DIR}/epoch_{epoch+1:03d}.png", nrow=4)
            print(f"  ↳ Saved sample grid to {SAMPLE_DIR}/epoch_{epoch+1:03d}.png")

        # 儲存 checkpoint
        torch.save({
            "epoch":     epoch,
            "model":     model.state_dict(),
            "optimizer": optimizer.state_dict(),
            "loss":      avg_loss,
        }, CHECKPOINT)

    print(f"\nTraining complete! Checkpoint saved to '{CHECKPOINT}'")

if __name__ == "__main__":
    main()
