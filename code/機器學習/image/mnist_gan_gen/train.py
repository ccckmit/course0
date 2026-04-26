import torch
import torch.nn as nn
import torch.optim as optim
from torch.utils.data import DataLoader
from torchvision import datasets, transforms
import os

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

class Discriminator(nn.Module):
    def __init__(self):
        super().__init__()
        self.conv1 = nn.Conv2d(1, 32, 4, 2, 1)
        self.conv2 = nn.Conv2d(32, 64, 4, 2, 1)
        self.fc = nn.Linear(64 * 7 * 7, 1)
        self.act = nn.LeakyReLU(0.2, True)
        self.dropout = nn.Dropout(0.3)

    def forward(self, x):
        out = self.act(self.conv1(x))
        out = self.dropout(out)
        out = self.act(self.conv2(out))
        out = out.view(out.size(0), -1)
        out = self.fc(out)
        return out

def train(epochs=30, batch_size=256, lr=0.001):
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize((0.5,), (0.5,))
    ])

    dataset = datasets.MNIST(root="./data", train=True, download=True, transform=transform)
    dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True, num_workers=2)

    generator = Generator().to(device)
    discriminator = Discriminator().to(device)

    adversarial_loss = nn.BCEWithLogitsLoss()
    optimizer_g = optim.Adam(generator.parameters(), lr=lr, betas=(0.5, 0.999))
    optimizer_d = optim.Adam(discriminator.parameters(), lr=lr, betas=(0.5, 0.999))

    os.makedirs("weights", exist_ok=True)

    for epoch in range(epochs):
        epoch_loss_g = 0
        epoch_loss_d = 0
        for i, (imgs, _) in enumerate(dataloader):
            batch = imgs.size(0)
            real = torch.ones(batch, 1).to(device)
            fake = torch.zeros(batch, 1).to(device)

            imgs = imgs.to(device)
            optimizer_d.zero_grad()
            real_validity = discriminator(imgs)
            real_loss = adversarial_loss(real_validity, real)

            z = torch.randn(batch, latent_dim).to(device)
            fake_imgs = generator(z).detach()
            fake_validity = discriminator(fake_imgs)
            fake_loss = adversarial_loss(fake_validity, fake)

            d_loss = (real_loss + fake_loss) / 2
            d_loss.backward()
            optimizer_d.step()
            epoch_loss_d += d_loss.item()

            optimizer_g.zero_grad()
            z = torch.randn(batch, latent_dim).to(device)
            gen_imgs = generator(z)
            validity = discriminator(gen_imgs)
            g_loss = adversarial_loss(validity, real)
            g_loss.backward()
            optimizer_g.step()
            epoch_loss_g += g_loss.item()

        avg_g_loss = epoch_loss_g / len(dataloader)
        avg_d_loss = epoch_loss_d / len(dataloader)
        print(f"Epoch {epoch+1}/{epochs} - D_loss: {avg_d_loss:.4f}, G_loss: {avg_g_loss:.4f}")

        if (epoch + 1) % 10 == 0:
            torch.save({
                "generator": generator.state_dict(),
                "discriminator": discriminator.state_dict(),
            }, f"weights/gan_{epoch+1}.pth")
            print(f"Saved weights/gan_{epoch+1}.pth")

    torch.save({
        "generator": generator.state_dict(),
        "discriminator": discriminator.state_dict(),
    }, "weights/gan_final.pth")
    print("Training complete. Saved to weights/gan_final.pth")

if __name__ == "__main__":
    train()