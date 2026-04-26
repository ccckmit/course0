"""
BipedalWalker-v3 Training Script
Algorithm: Soft Actor-Critic (SAC)
Framework: PyTorch + Gymnasium
Device:    CUDA / MPS (Apple Silicon iMac M3) / CPU  ← auto-detected
"""

import os
import random
from dataclasses import dataclass

import gymnasium as gym
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim


# ─────────────────────────────────────────────────────────────
#  Config
# ─────────────────────────────────────────────────────────────
@dataclass
class Config:
    env_id: str = "BipedalWalker-v3"
    seed: int = 42

    # SAC
    gamma: float = 0.99          # discount factor
    tau: float   = 0.005         # soft-update rate for target networks
    tune_alpha: bool = True      # auto-tune entropy temperature

    # Network
    hidden_dim: int = 256

    # Training
    # total_steps: int    = 1_000_000
    total_steps: int    = 100000
    batch_size: int     = 256
    buffer_size: int    = 1_000_000
    learning_starts: int = 10_000
    lr: float           = 3e-4
    updates_per_step: int = 1

    # Logging / saving
    log_interval: int  = 5_000
    save_interval: int = 50_000
    save_dir: str      = "checkpoints"


# ─────────────────────────────────────────────────────────────
#  Device  (CUDA → MPS → CPU)
# ─────────────────────────────────────────────────────────────
def get_device() -> torch.device:
    if torch.cuda.is_available():
        device = torch.device("cuda")
    elif torch.backends.mps.is_available():
        device = torch.device("mps")
    else:
        device = torch.device("cpu")
    print(f"[Device] {device}")
    return device


# ─────────────────────────────────────────────────────────────
#  Replay Buffer  (numpy on CPU, moved to device on sample)
# ─────────────────────────────────────────────────────────────
class ReplayBuffer:
    def __init__(self, capacity: int, obs_dim: int, act_dim: int, device: torch.device):
        self.capacity = capacity
        self.device   = device
        self.ptr      = 0
        self.size     = 0

        self.obs      = np.zeros((capacity, obs_dim), dtype=np.float32)
        self.actions  = np.zeros((capacity, act_dim), dtype=np.float32)
        self.rewards  = np.zeros((capacity, 1),       dtype=np.float32)
        self.next_obs = np.zeros((capacity, obs_dim), dtype=np.float32)
        self.dones    = np.zeros((capacity, 1),       dtype=np.float32)

    def add(self, obs, action, reward, next_obs, done: float):
        self.obs[self.ptr]      = obs
        self.actions[self.ptr]  = action
        self.rewards[self.ptr]  = reward
        self.next_obs[self.ptr] = next_obs
        self.dones[self.ptr]    = done
        self.ptr  = (self.ptr + 1) % self.capacity
        self.size = min(self.size + 1, self.capacity)

    def sample(self, batch_size: int):
        idx = np.random.randint(0, self.size, size=batch_size)
        to  = lambda arr: torch.from_numpy(arr[idx]).to(self.device)
        return to(self.obs), to(self.actions), to(self.rewards), \
               to(self.next_obs), to(self.dones)

    def __len__(self):
        return self.size


# ─────────────────────────────────────────────────────────────
#  Networks
# ─────────────────────────────────────────────────────────────
def _mlp(in_dim: int, out_dim: int, hidden: int) -> nn.Sequential:
    return nn.Sequential(
        nn.Linear(in_dim, hidden), nn.ReLU(),
        nn.Linear(hidden, hidden), nn.ReLU(),
        nn.Linear(hidden, out_dim),
    )


class TwinCritic(nn.Module):
    """Two independent Q-networks to reduce overestimation bias."""
    def __init__(self, obs_dim: int, act_dim: int, hidden: int):
        super().__init__()
        self.q1 = _mlp(obs_dim + act_dim, 1, hidden)
        self.q2 = _mlp(obs_dim + act_dim, 1, hidden)

    def forward(self, obs, action):
        x = torch.cat([obs, action], dim=-1)
        return self.q1(x), self.q2(x)


LOG_STD_MIN, LOG_STD_MAX = -5, 2


class GaussianActor(nn.Module):
    """Squashed Gaussian policy with reparameterisation."""
    def __init__(self, obs_dim: int, act_dim: int, hidden: int, act_scale: torch.Tensor):
        super().__init__()
        self.shared       = nn.Sequential(
            nn.Linear(obs_dim, hidden), nn.ReLU(),
            nn.Linear(hidden, hidden),  nn.ReLU(),
        )
        self.mu_head      = nn.Linear(hidden, act_dim)
        self.log_std_head = nn.Linear(hidden, act_dim)
        # register as buffer so it moves with .to(device) automatically
        self.register_buffer("act_scale", act_scale)

    def forward(self, obs):
        h       = self.shared(obs)
        mu      = self.mu_head(h)
        log_std = self.log_std_head(h).clamp(LOG_STD_MIN, LOG_STD_MAX)
        return mu, log_std

    def get_action(self, obs):
        mu, log_std = self.forward(obs)
        std  = log_std.exp()
        dist = torch.distributions.Normal(mu, std)
        x_t  = dist.rsample()                          # reparameterised
        y_t  = torch.tanh(x_t)
        action = y_t * self.act_scale

        # log-prob with tanh squashing correction
        log_prob = (dist.log_prob(x_t)
                    - torch.log(self.act_scale * (1 - y_t.pow(2)) + 1e-6)
                   ).sum(dim=-1, keepdim=True)

        mean_action = torch.tanh(mu) * self.act_scale
        return action, log_prob, mean_action


# ─────────────────────────────────────────────────────────────
#  SAC Agent
# ─────────────────────────────────────────────────────────────
class SAC:
    def __init__(self, obs_dim: int, act_dim: int,
                 act_scale: np.ndarray, cfg: Config, device: torch.device):
        self.cfg    = cfg
        self.device = device

        scale_t = torch.FloatTensor(act_scale).to(device)

        self.actor         = GaussianActor(obs_dim, act_dim, cfg.hidden_dim, scale_t).to(device)
        self.critic        = TwinCritic(obs_dim, act_dim, cfg.hidden_dim).to(device)
        self.critic_target = TwinCritic(obs_dim, act_dim, cfg.hidden_dim).to(device)
        self.critic_target.load_state_dict(self.critic.state_dict())
        for p in self.critic_target.parameters():
            p.requires_grad = False

        self.actor_opt  = optim.Adam(self.actor.parameters(),  lr=cfg.lr)
        self.critic_opt = optim.Adam(self.critic.parameters(), lr=cfg.lr)

        # Entropy temperature α
        self.target_entropy = float(-act_dim)          # heuristic
        if cfg.tune_alpha:
            self.log_alpha = torch.zeros(1, requires_grad=True, device=device)
            self.alpha     = self.log_alpha.exp().item()
            self.alpha_opt = optim.Adam([self.log_alpha], lr=cfg.lr)
        else:
            self.log_alpha = None
            self.alpha     = 0.2

    # ── Soft-update target networks ──────────
    @torch.no_grad()
    def _soft_update(self):
        for p, tp in zip(self.critic.parameters(),
                         self.critic_target.parameters()):
            tp.data.mul_(1 - self.cfg.tau).add_(p.data * self.cfg.tau)

    # ── Action selection ─────────────────────
    @torch.no_grad()
    def select_action(self, obs: np.ndarray, deterministic: bool = False) -> np.ndarray:
        obs_t = torch.FloatTensor(obs).unsqueeze(0).to(self.device)
        if deterministic:
            _, _, action = self.actor.get_action(obs_t)
        else:
            action, _, _ = self.actor.get_action(obs_t)
        return action.squeeze(0).cpu().numpy()

    # ── One gradient step ────────────────────
    def update(self, buffer: ReplayBuffer) -> dict:
        obs, actions, rewards, next_obs, dones = buffer.sample(self.cfg.batch_size)

        # ── Critic ──
        with torch.no_grad():
            next_a, next_log_pi, _ = self.actor.get_action(next_obs)
            q1_next, q2_next = self.critic_target(next_obs, next_a)
            q_next    = torch.min(q1_next, q2_next) - self.alpha * next_log_pi
            q_target  = rewards + self.cfg.gamma * (1 - dones) * q_next

        q1, q2      = self.critic(obs, actions)
        critic_loss = F.mse_loss(q1, q_target) + F.mse_loss(q2, q_target)
        self.critic_opt.zero_grad()
        critic_loss.backward()
        self.critic_opt.step()

        # ── Actor ──
        new_a, log_pi, _ = self.actor.get_action(obs)
        q1_pi, q2_pi     = self.critic(obs, new_a)
        actor_loss        = (self.alpha * log_pi - torch.min(q1_pi, q2_pi)).mean()
        self.actor_opt.zero_grad()
        actor_loss.backward()
        self.actor_opt.step()

        # ── Alpha ──
        alpha_loss_val = 0.0
        if self.cfg.tune_alpha:
            alpha_loss = (-self.log_alpha.exp() *
                          (log_pi.detach() + self.target_entropy)).mean()
            self.alpha_opt.zero_grad()
            alpha_loss.backward()
            self.alpha_opt.step()
            self.alpha     = self.log_alpha.exp().item()
            alpha_loss_val = alpha_loss.item()

        self._soft_update()

        return dict(
            critic_loss = critic_loss.item(),
            actor_loss  = actor_loss.item(),
            alpha       = self.alpha,
            alpha_loss  = alpha_loss_val,
        )

    # ── Checkpoint ───────────────────────────
    def save(self, path: str):
        torch.save(dict(
            actor      = self.actor.state_dict(),
            critic     = self.critic.state_dict(),
            alpha      = self.alpha,
            log_alpha  = self.log_alpha.detach().cpu() if self.log_alpha is not None else None,
        ), path)
        print(f"[Save] {path}")

    def load(self, path: str):
        ckpt = torch.load(path, map_location=self.device)
        self.actor.load_state_dict(ckpt["actor"])
        self.critic.load_state_dict(ckpt["critic"])
        self.critic_target.load_state_dict(ckpt["critic"])
        self.alpha = ckpt["alpha"]
        if self.log_alpha is not None and ckpt["log_alpha"] is not None:
            self.log_alpha.data.copy_(ckpt["log_alpha"].to(self.device))
        print(f"[Load] {path}")


# ─────────────────────────────────────────────────────────────
#  Main training loop
# ─────────────────────────────────────────────────────────────
def train():
    cfg    = Config()
    device = get_device()

    random.seed(cfg.seed)
    np.random.seed(cfg.seed)
    torch.manual_seed(cfg.seed)

    env     = gym.make(cfg.env_id)
    env.action_space.seed(cfg.seed)
    obs_dim   = env.observation_space.shape[0]   # 24
    act_dim   = env.action_space.shape[0]         # 4
    act_scale = env.action_space.high             # [1, 1, 1, 1]

    agent  = SAC(obs_dim, act_dim, act_scale, cfg, device)
    buffer = ReplayBuffer(cfg.buffer_size, obs_dim, act_dim, device)

    os.makedirs(cfg.save_dir, exist_ok=True)

    obs, _       = env.reset(seed=cfg.seed)
    ep_ret       = 0.0
    ep_len       = 0
    ep_count     = 0
    recent_rets: list[float] = []
    best_mean    = -float("inf")

    print(f"\n{'═'*60}")
    print(f"  BipedalWalker-v3  ·  SAC  ·  {device}")
    print(f"  obs={obs_dim}  act={act_dim}  steps={cfg.total_steps:,}")
    print(f"{'═'*60}\n")

    for step in range(1, cfg.total_steps + 1):

        # Collect
        if step < cfg.learning_starts:
            action = env.action_space.sample()
        else:
            action = agent.select_action(obs)

        next_obs, reward, terminated, truncated, _ = env.step(action)
        done = terminated or truncated

        # Store  (use terminated, NOT truncated, to avoid value-bootstrap error)
        buffer.add(obs, action, reward, next_obs, float(terminated))
        obs    = next_obs
        ep_ret += reward
        ep_len += 1

        if done:
            recent_rets.append(ep_ret)
            if len(recent_rets) > 20:
                recent_rets.pop(0)
            ep_count += 1
            obs, _ = env.reset()
            ep_ret = ep_len = 0

        # Update
        if step >= cfg.learning_starts and len(buffer) >= cfg.batch_size:
            for _ in range(cfg.updates_per_step):
                agent.update(buffer)

        # Log
        if step % cfg.log_interval == 0:
            mean_ret = np.mean(recent_rets) if recent_rets else float("nan")
            print(f"step {step:>8,} | ep {ep_count:>5} | "
                  f"return(avg20) {mean_ret:>8.2f} | "
                  f"α {agent.alpha:.4f} | buf {len(buffer):,}")

        # Save
        if step % cfg.save_interval == 0:
            agent.save(os.path.join(cfg.save_dir, f"sac_step{step}.pt"))
            mean_ret = np.mean(recent_rets) if recent_rets else -9999
            if mean_ret > best_mean:
                best_mean = mean_ret
                agent.save(os.path.join(cfg.save_dir, "sac_best.pt"))
                print(f"  ★ New best mean return: {best_mean:.2f}")

    env.close()
    agent.save(os.path.join(cfg.save_dir, "sac_final.pt"))
    print("\n[Done] Training finished.")


if __name__ == "__main__":
    train()
