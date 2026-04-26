import torch
import torch.nn as nn
from torchvision.utils import save_image
import os
import argparse

if torch.cuda.is_available():
    device = torch.device("cuda")
elif torch.backends.mps.is_available():
    device = torch.device("mps")
else:
    device = torch.device("cpu")

latent_dim = 100
img_size = 28
channels = 1

class Generator(nn.Module):
    def __init__(self):
        super().__init__()
        self.l0 = nn.Linear(latent_dim, 7 * 7 * 64)
        self.conv1 = nn.ConvTranspose2d(64, 32, 4, 2, 1)
        self.conv2 = nn.ConvTranspose2d(32, 1, 4, 2, 1)
        self.bn1 = nn.BatchNorm2d(32)
        self.act = nn.ReLU(True)
        self.out = nn.Tanh()

    def forward(self, z):
        out = self.l0(z).view(-1, 64, 7, 7)
        out = self.act(self.bn1(self.conv1(out)))
        out = self.out(self.conv2(out))
        return out

def generate(weights_path="weights/gan_final.pth", num_images=16, output_dir="output"):
    generator = Generator().to(device)
    checkpoint = torch.load(weights_path, map_location=device, weights_only=False)
    generator.load_state_dict(checkpoint["generator"])
    generator.eval()

    os.makedirs(output_dir, exist_ok=True)

    with torch.no_grad():
        z = torch.randn(num_images, latent_dim).to(device)
        gen_imgs = generator(z)

    for i, img in enumerate(gen_imgs):
        save_image(img, f"{output_dir}/digit_{i:03d}.png")

    grid = save_image(gen_imgs, f"{output_dir}/grid.png", nrow=4, normalize=True)
    print(f"Generated {num_images} images in {output_dir}/")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--weights", default="weights/gan_final.pth")
    parser.add_argument("--num", type=int, default=16)
    parser.add_argument("--output", default="output")
    args = parser.parse_args()

    generate(args.weights, args.num, args.output)