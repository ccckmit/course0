"""
BipedalWalker-v3 Run / Evaluation Script
Loads a trained SAC checkpoint and renders the agent.

Usage:
    python run.py                               # loads checkpoints/sac_best.pt
    python run.py --ckpt checkpoints/sac_final.pt --episodes 5
    python run.py --record                      # save an MP4 to recordings/
"""

import argparse
import os

import gymnasium as gym
import numpy as np
import torch
import torch.nn as nn


# ─────────────────────────────────────────────────────────────
#  Device  (must match train.py logic)
# ─────────────────────────────────────────────────────────────
def get_device() -> torch.device:
    if torch.cuda.is_available():
        return torch.device("cuda")
    elif torch.backends.mps.is_available():
        return torch.device("mps")
    return torch.device("cpu")


# ─────────────────────────────────────────────────────────────
#  Minimal Actor  (copy of train.py – no training dependencies)
# ─────────────────────────────────────────────────────────────
LOG_STD_MIN, LOG_STD_MAX = -5, 2


class GaussianActor(nn.Module):
    def __init__(self, obs_dim: int, act_dim: int, hidden: int, act_scale: torch.Tensor):
        super().__init__()
        self.shared = nn.Sequential(
            nn.Linear(obs_dim, hidden), nn.ReLU(),
            nn.Linear(hidden, hidden),  nn.ReLU(),
        )
        self.mu_head      = nn.Linear(hidden, act_dim)
        self.log_std_head = nn.Linear(hidden, act_dim)
        self.register_buffer("act_scale", act_scale)

    def get_action(self, obs, deterministic: bool = True):
        h       = self.shared(obs)
        mu      = self.mu_head(h)
        log_std = self.log_std_head(h).clamp(LOG_STD_MIN, LOG_STD_MAX)
        if deterministic:
            return torch.tanh(mu) * self.act_scale
        std  = log_std.exp()
        x_t  = torch.distributions.Normal(mu, std).rsample()
        return torch.tanh(x_t) * self.act_scale


# ─────────────────────────────────────────────────────────────
#  Load actor from checkpoint
# ─────────────────────────────────────────────────────────────
def load_actor(ckpt_path: str, obs_dim: int, act_dim: int,
               act_scale: np.ndarray, hidden: int,
               device: torch.device) -> GaussianActor:
    scale_t = torch.FloatTensor(act_scale).to(device)
    actor   = GaussianActor(obs_dim, act_dim, hidden, scale_t).to(device)
    ckpt    = torch.load(ckpt_path, map_location=device)
    actor.load_state_dict(ckpt["actor"])
    actor.eval()
    print(f"[Load] {ckpt_path}")
    return actor


# ─────────────────────────────────────────────────────────────
#  Run one episode
# ─────────────────────────────────────────────────────────────
@torch.no_grad()
def run_episode(env: gym.Env, actor: GaussianActor,
                device: torch.device, deterministic: bool = True) -> float:
    obs, _ = env.reset()
    total_reward = 0.0

    while True:
        obs_t  = torch.FloatTensor(obs).unsqueeze(0).to(device)
        action = actor.get_action(obs_t, deterministic=deterministic)
        action = action.squeeze(0).cpu().numpy()

        obs, reward, terminated, truncated, _ = env.step(action)
        total_reward += reward
        if terminated or truncated:
            break

    return total_reward


# ─────────────────────────────────────────────────────────────
#  Main
# ─────────────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description="Run a trained SAC BipedalWalker agent")
    parser.add_argument("--ckpt",        default="checkpoints/sac_best.pt",
                        help="Path to checkpoint (.pt)")
    parser.add_argument("--episodes",    type=int, default=3,
                        help="Number of episodes to run")
    parser.add_argument("--hidden",      type=int, default=256,
                        help="Hidden dim (must match training)")
    parser.add_argument("--stochastic",  action="store_true",
                        help="Use stochastic (sampled) policy instead of deterministic mean")
    parser.add_argument("--record",      action="store_true",
                        help="Record episodes to MP4 (saved in recordings/)")
    parser.add_argument("--record-dir",  default="recordings",
                        help="Output directory for recordings")
    parser.add_argument("--no-render",   action="store_true",
                        help="Disable rendering window (useful with --record)")
    args = parser.parse_args()

    device = get_device()
    print(f"[Device] {device}")

    if not os.path.exists(args.ckpt):
        print(f"[Error] Checkpoint not found: {args.ckpt}")
        print("  → Run train.py first, or pass --ckpt <path>")
        return

    # Build env
    if args.record:
        os.makedirs(args.record_dir, exist_ok=True)
        base_env  = gym.make("BipedalWalker-v3", render_mode="rgb_array")
        env       = gym.wrappers.RecordVideo(
            base_env,
            video_folder=args.record_dir,
            episode_trigger=lambda _: True,   # record every episode
            name_prefix="bipedal",
        )
    elif args.no_render:
        env = gym.make("BipedalWalker-v3")
    else:
        env = gym.make("BipedalWalker-v3", render_mode="human")

    obs_dim   = env.observation_space.shape[0]
    act_dim   = env.action_space.shape[0]
    act_scale = env.action_space.high

    actor = load_actor(args.ckpt, obs_dim, act_dim, act_scale, args.hidden, device)

    deterministic = not args.stochastic
    mode_str      = "deterministic" if deterministic else "stochastic"
    print(f"\nRunning {args.episodes} episode(s) in {mode_str} mode …\n")

    returns = []
    for ep in range(1, args.episodes + 1):
        ret = run_episode(env, actor, device, deterministic=deterministic)
        returns.append(ret)
        status = "✓ SOLVED" if ret >= 300 else ""
        print(f"  Episode {ep:>2}: return = {ret:>8.2f}  {status}")

    env.close()

    print(f"\n{'─'*40}")
    print(f"  Mean return : {np.mean(returns):>8.2f}")
    print(f"  Std  return : {np.std(returns):>8.2f}")
    print(f"  Min  return : {np.min(returns):>8.2f}")
    print(f"  Max  return : {np.max(returns):>8.2f}")
    if args.record:
        print(f"\n  Videos saved → {os.path.abspath(args.record_dir)}/")


if __name__ == "__main__":
    main()
