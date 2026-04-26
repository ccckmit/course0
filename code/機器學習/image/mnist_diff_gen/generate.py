"""
generate.py — 從訓練好的 DDPM 模型生成 MNIST 手寫數字
支援多種採樣策略：DDPM（完整）、DDIM（快速）
"""

import os
import math
import argparse
import torch
import torch.nn as nn
import torch.nn.functional as F
from torchvision.utils import save_image, make_grid
import matplotlib.pyplot as plt
from tqdm import tqdm

# ─────────────────────────────────────────
# 與 train.py 相同的模型定義
# ─────────────────────────────────────────
DEVICE     = "cuda" if torch.cuda.is_available() else "cpu"
IMG_SIZE   = 28
T          = 1000
BETA_START = 1e-4
BETA_END   = 0.02
CHECKPOINT = "ddpm_mnist.pth"
OUTPUT_DIR = "generated"
os.makedirs(OUTPUT_DIR, exist_ok=True)


def make_schedule(T, beta_start, beta_end, device):
    betas         = torch.linspace(beta_start, beta_end, T, device=device)
    alphas        = 1.0 - betas
    alpha_bar     = torch.cumprod(alphas, dim=0)
    alpha_bar_prev = F.pad(alpha_bar[:-1], (1, 0), value=1.0)
    return {
        "betas":              betas,
        "alphas":             alphas,
        "alpha_bar":          alpha_bar,
        "alpha_bar_prev":     alpha_bar_prev,
        "sqrt_alpha_bar":     alpha_bar.sqrt(),
        "sqrt_one_minus_ab":  (1 - alpha_bar).sqrt(),
        "posterior_variance": betas * (1 - alpha_bar_prev) / (1 - alpha_bar),
    }


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
        return torch.cat([emb.sin(), emb.cos()], dim=-1)


def get_groups(channels):
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


class UNet(nn.Module):
    def __init__(self, in_ch=1, base_ch=64, time_dim=256):
        super().__init__()
        self.time_dim = time_dim
        self.pe    = SinusoidalPE(time_dim)
        self.t_mlp = nn.Sequential(
            nn.Linear(time_dim, time_dim * 4),
            nn.SiLU(),
            nn.Linear(time_dim * 4, time_dim),
        )
        self.enc1  = ResBlock(in_ch,       base_ch,     time_dim)
        self.enc2  = ResBlock(base_ch,     base_ch * 2, time_dim)
        self.enc3  = ResBlock(base_ch * 2, base_ch * 4, time_dim)
        self.down1 = nn.Conv2d(base_ch,     base_ch,     4, 2, 1)
        self.down2 = nn.Conv2d(base_ch * 2, base_ch * 2, 4, 2, 1)
        self.mid1  = ResBlock(base_ch * 4, base_ch * 4, time_dim)
        self.mid2  = ResBlock(base_ch * 4, base_ch * 4, time_dim)
        self.up2   = nn.ConvTranspose2d(base_ch * 4, base_ch * 2, 4, 2, 1)
        self.dec2  = ResBlock(base_ch * 4, base_ch * 2, time_dim)
        self.up1   = nn.ConvTranspose2d(base_ch * 2, base_ch,     4, 2, 1)
        self.dec1  = ResBlock(base_ch * 2, base_ch,     time_dim)
        self.out   = nn.Conv2d(base_ch, in_ch, 1)

    def forward(self, x, t):
        t_emb = self.t_mlp(self.pe(t))
        e1 = self.enc1(x,             t_emb)
        e2 = self.enc2(self.down1(e1), t_emb)
        e3 = self.enc3(self.down2(e2), t_emb)
        m  = self.mid2(self.mid1(e3, t_emb), t_emb)
        d2 = self.dec2(torch.cat([self.up2(m),  e2], dim=1), t_emb)
        d1 = self.dec1(torch.cat([self.up1(d2), e1], dim=1), t_emb)
        return self.out(d1)


# ─────────────────────────────────────────
# DDPM 完整逆向採樣（1000 步）
# ─────────────────────────────────────────
@torch.no_grad()
def ddpm_sample(model, schedule, n, device, verbose=True):
    x = torch.randn(n, 1, IMG_SIZE, IMG_SIZE, device=device)
    steps = reversed(range(T))
    if verbose:
        steps = tqdm(steps, total=T, desc="DDPM sampling")

    for step in steps:
        t_tensor = torch.full((n,), step, device=device, dtype=torch.long)
        beta_t    = schedule["betas"][step]
        alpha_t   = schedule["alphas"][step]
        ab_t      = schedule["alpha_bar"][step]
        ab_prev_t = schedule["alpha_bar_prev"][step]

        eps    = model(x, t_tensor)
        x0_hat = ((x - (1 - ab_t).sqrt() * eps) / ab_t.sqrt()).clamp(-1, 1)
        mean   = (ab_prev_t.sqrt() * beta_t / (1 - ab_t)) * x0_hat + \
                 (alpha_t.sqrt() * (1 - ab_prev_t) / (1 - ab_t)) * x

        if step > 0:
            var = schedule["posterior_variance"][step]
            x   = mean + var.sqrt() * torch.randn_like(x)
        else:
            x = mean

    return x


# ─────────────────────────────────────────
# DDIM 快速採樣（可自訂步數，不需隨機噪聲）
# ─────────────────────────────────────────
@torch.no_grad()
def ddim_sample(model, schedule, n, device, ddim_steps=50, eta=0.0, verbose=True):
    """
    eta=0   → 完全確定性（最快）
    eta=1   → 等同 DDPM（帶隨機噪聲）
    """
    # 均勻選取子步驟
    step_indices = torch.linspace(0, T - 1, ddim_steps, dtype=torch.long)
    steps        = list(reversed(step_indices.tolist()))

    x = torch.randn(n, 1, IMG_SIZE, IMG_SIZE, device=device)
    it = tqdm(range(len(steps)), desc=f"DDIM sampling ({ddim_steps} steps)") if verbose else range(len(steps))

    for i in it:
        t_curr = int(steps[i])
        t_prev = int(steps[i + 1]) if i + 1 < len(steps) else 0

        t_tensor = torch.full((n,), t_curr, device=device, dtype=torch.long)
        ab_t     = schedule["alpha_bar"][t_curr]
        ab_prev  = schedule["alpha_bar"][t_prev] if t_prev > 0 else torch.tensor(1.0, device=device)

        eps    = model(x, t_tensor)
        x0_hat = ((x - (1 - ab_t).sqrt() * eps) / ab_t.sqrt()).clamp(-1, 1)

        # DDIM update
        sigma  = eta * ((1 - ab_prev) / (1 - ab_t) * (1 - ab_t / ab_prev)).sqrt()
        dir_xt = (1 - ab_prev - sigma ** 2).sqrt() * eps
        noise  = sigma * torch.randn_like(x) if eta > 0 else 0.0
        x      = ab_prev.sqrt() * x0_hat + dir_xt + noise

    return x


# ─────────────────────────────────────────
# 視覺化：反擴散過程（中間步驟）
# ─────────────────────────────────────────
@torch.no_grad()
def visualize_denoising(model, schedule, device, n_steps_show=8):
    """顯示從純噪聲逐漸還原的過程"""
    x = torch.randn(1, 1, IMG_SIZE, IMG_SIZE, device=device)
    snapshots = []

    # 均勻挑選要截圖的步驟
    show_at = set(
        int(T - 1 - i * (T / n_steps_show))
        for i in range(n_steps_show)
    )
    show_at.add(0)

    for step in tqdm(reversed(range(T)), total=T, desc="Denoising visualization"):
        t_tensor = torch.full((1,), step, device=device, dtype=torch.long)
        beta_t    = schedule["betas"][step]
        alpha_t   = schedule["alphas"][step]
        ab_t      = schedule["alpha_bar"][step]
        ab_prev_t = schedule["alpha_bar_prev"][step]

        eps    = model(x, t_tensor)
        x0_hat = ((x - (1 - ab_t).sqrt() * eps) / ab_t.sqrt()).clamp(-1, 1)
        mean   = (ab_prev_t.sqrt() * beta_t / (1 - ab_t)) * x0_hat + \
                 (alpha_t.sqrt() * (1 - ab_prev_t) / (1 - ab_t)) * x

        if step > 0:
            var = schedule["posterior_variance"][step]
            x   = mean + var.sqrt() * torch.randn_like(x)
        else:
            x = mean

        if step in show_at:
            img = (x * 0.5 + 0.5).clamp(0, 1).cpu()
            snapshots.append((T - 1 - step, img))

    # 繪圖
    snapshots.sort(key=lambda kv: kv[0])
    fig, axes = plt.subplots(1, len(snapshots), figsize=(len(snapshots) * 2, 2.5))
    for ax, (removed_noise, img) in zip(axes, snapshots):
        ax.imshow(img[0, 0].numpy(), cmap="gray")
        pct = int(removed_noise / (T - 1) * 100)
        ax.set_title(f"t={T-1-removed_noise}\n({pct}% done)", fontsize=8)
        ax.axis("off")

    plt.suptitle("Denoising Process (noise → digit)", fontsize=12, y=1.02)
    plt.tight_layout()
    out = f"{OUTPUT_DIR}/denoising_process.png"
    plt.savefig(out, dpi=150, bbox_inches="tight")
    plt.close()
    print(f"  ↳ Saved denoising process to {out}")


# ─────────────────────────────────────────
# 主程式
# ─────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description="DDPM/DDIM MNIST Generator")
    parser.add_argument("--mode",       choices=["ddpm", "ddim", "denoise", "all"],
                        default="all",  help="採樣模式")
    parser.add_argument("--n",          type=int, default=64,   help="生成圖片數量")
    parser.add_argument("--ddim_steps", type=int, default=50,   help="DDIM 步數")
    parser.add_argument("--eta",        type=float, default=0.0, help="DDIM eta (0=確定性, 1=隨機)")
    parser.add_argument("--ckpt",       default=CHECKPOINT,     help="Checkpoint 路徑")
    args = parser.parse_args()

    # 載入模型
    if not os.path.exists(args.ckpt):
        print(f"[ERROR] Checkpoint '{args.ckpt}' not found.")
        print("        Please run train.py first.")
        return

    print(f"Device   : {DEVICE}")
    print(f"Checkpoint: {args.ckpt}")

    model = UNet().to(DEVICE)
    ckpt  = torch.load(args.ckpt, map_location=DEVICE)
    model.load_state_dict(ckpt["model"])
    model.eval()
    print(f"Loaded model (trained epoch: {ckpt.get('epoch', '?')+1}, loss: {ckpt.get('loss', '?'):.4f})")

    schedule = make_schedule(T, BETA_START, BETA_END, DEVICE)

    # ── DDPM 完整採樣 ──────────────────────
    if args.mode in ("ddpm", "all"):
        print(f"\n[1/3] DDPM sampling ({T} steps) × {args.n} images …")
        imgs = ddpm_sample(model, schedule, args.n, DEVICE)
        imgs = (imgs * 0.5 + 0.5).clamp(0, 1)
        nrow = int(args.n ** 0.5)
        out  = f"{OUTPUT_DIR}/ddpm_{args.n}imgs.png"
        save_image(imgs, out, nrow=nrow)
        print(f"  ↳ Saved to {out}")

    # ── DDIM 快速採樣 ──────────────────────
    if args.mode in ("ddim", "all"):
        print(f"\n[2/3] DDIM sampling ({args.ddim_steps} steps, η={args.eta}) × {args.n} images …")
        imgs = ddim_sample(model, schedule, args.n, DEVICE,
                           ddim_steps=args.ddim_steps, eta=args.eta)
        imgs = (imgs * 0.5 + 0.5).clamp(0, 1)
        nrow = int(args.n ** 0.5)
        out  = f"{OUTPUT_DIR}/ddim_{args.ddim_steps}steps_{args.n}imgs.png"
        save_image(imgs, out, nrow=nrow)
        print(f"  ↳ Saved to {out}")

    # ── 去噪過程視覺化 ────────────────────
    if args.mode in ("denoise", "all"):
        print(f"\n[3/3] Visualizing denoising process …")
        visualize_denoising(model, schedule, DEVICE)

    print(f"\nAll outputs saved to './{OUTPUT_DIR}/'")


if __name__ == "__main__":
    main()
