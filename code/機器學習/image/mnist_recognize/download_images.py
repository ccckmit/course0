import torch
from torchvision import datasets, transforms
from PIL import Image

transform = transforms.ToTensor()
testSet = datasets.MNIST(root="./data", train=False, download=True, transform=transform)

for i in range(10):
    img, label = testSet[i]
    img = img.squeeze().numpy()
    Image.fromarray((img * 255).astype("uint8")).save(f"img/{label}_{i}.png")
    print(f"Saved img/{label}_{i}.png (label={label})")