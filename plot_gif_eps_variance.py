import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import imageio.v2 as imageio
import os
import re

# Load data
data = pd.read_csv("sim_data_eps_vary.txt")

# Format labels with LaTeX ε
def format_algo_label(raw_label):
    match = re.search(r'LCBS \(\\epsilon = ([\d\.]+)\)', raw_label)
    if match:
        return rf"$\epsilon={match.group(1)}$"
    return raw_label

data['FormattedLabel'] = data['Algorithm'].apply(format_algo_label)
unique_labels = data['FormattedLabel'].unique()

# Colors, markers, and styles
markers = ['o', 's', '^', 'v', 'D', 'P', '*']
linestyles = ['solid', 'dashed', 'dotted', 'dashdot']
colors = plt.cm.viridis_r(np.linspace(0, 1, len(unique_labels)))

# Frame storage
frame_dir = "gif_frames"
os.makedirs(frame_dir, exist_ok=True)
frame_paths = []
frame_durations = []

# Custom durations (in seconds)
custom_durations = [2.0] * len(unique_labels)

# Plot one algorithm per frame
for idx, (label, duration) in enumerate(zip(unique_labels, custom_durations)):
    subset = data[data['FormattedLabel'] == label]
    
    plt.figure(figsize=(6.5, 3))
    plt.plot(
        subset['Agents'],
        subset['SuccessRate'],
        marker=markers[idx % len(markers)],
        linestyle=linestyles[idx % len(linestyles)],
        color=colors[idx],
        label=label
    )
    
    plt.xlabel("Agents")
    plt.ylabel("Success Rate")
    plt.ylim(-0.05, 1.05)
    plt.grid(True)
    plt.legend(loc="upper right")
    plt.title("Success Rate per LCBS $\\epsilon$ Configuration")
    plt.tight_layout()
    
    frame_path = f"{frame_dir}/frame_{idx:02d}.png"
    plt.savefig(frame_path, dpi=150)
    plt.close()
    
    frame_paths.append(frame_path)
    frame_durations.append(duration)

# Create animated GIF with infinite loop
gif_path = "GIF_LCBS_SUCCESS_RATE_TRENDS.gif"
with imageio.get_writer(gif_path, mode='I', loop=0, duration=100.0) as writer:
    for path in frame_paths:
        image = imageio.imread(path)
        writer.append_data(image)
print(f"✅ GIF saved as: {gif_path}")
