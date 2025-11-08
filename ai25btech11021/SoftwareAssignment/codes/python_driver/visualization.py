import numpy as np
import matplotlib.pyplot as plt
from PIL import Image
import os

# taking folder name as input like in the C code
folder_name = input("Enter folder name: ").strip()

# making path to that folder
image_dir = os.path.join("..", "..", "figs", folder_name)

# fixed image name
possible_ext = ['.jpg', '.png', '.jpeg', '.pgm', '.ppm']
image_path = None

# check which file exists with name 'image'
for ext in possible_ext:
    path = os.path.join(image_dir, "image" + ext)
    if os.path.exists(path):
        image_path = path
        break

print("Reading:", image_path)

# read image and make it gray
img = Image.open(image_path).convert("L")
A = np.array(img, float)

# do svd
U, S, Vt = np.linalg.svd(A, full_matrices=False)

# create visualization folder inside figs if not exist
visu_dir = os.path.join("..", "..", "figs", "visualization")
os.makedirs(visu_dir, exist_ok=True)

# output plot path
output_path = os.path.join(visu_dir, f"{folder_name}_visu.png")

# plot and save
plt.figure(figsize=(6, 4))
plt.plot(S, 'o-', markersize=2)
plt.title("Singular Values of " + folder_name)
plt.xlabel("Rank index (k)")
plt.ylabel("Singular value")
plt.grid(True)
plt.tight_layout()
plt.savefig(output_path)
plt.close()