import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

# Load your data file
data = pd.read_csv("sim_data_eps_vary.txt") 

# Function to convert raw algorithm string to LaTeX-friendly label
def format_algo_label(raw_label):
    # Example: 'LCBS (\epsilon = 0.05)' → 'LCBS ($\\epsilon=0.05$)'
    import re
    match = re.search(r'LCBS \(\\epsilon = ([\d\.]+)\)', raw_label)
    if match:
        eps_value = match.group(1)
        return rf"$\epsilon={eps_value}$"
    return raw_label

# Apply the label formatting
data['FormattedLabel'] = data['Algorithm'].apply(format_algo_label)

# Plot setup
plt.figure(figsize=(6.5, 3))
unique_labels = data['FormattedLabel'].unique()

# Marker and style settings
markers = ['o', 's', '^', 'v', 'D', 'P', '*']
linestyles = ['solid', 'dashed', 'dotted', 'dashdot']
colors = plt.cm.viridis_r(np.linspace(0, 1, len(unique_labels)))

# Plot each epsilon configuration
for idx, label in enumerate(unique_labels):
    subset = data[data['FormattedLabel'] == label]
    plt.plot(
        subset['Agents'],
        subset['SuccessRate'],
        label=label,
        marker=markers[idx % len(markers)],
        linestyle=linestyles[idx % len(linestyles)],
        color=colors[idx]
    )

# Axis and layout styling
plt.xlabel("Agents")
plt.ylabel("Success Rate")
plt.ylim(-0.05, 1.05)
plt.title("Effect of Varying $\\epsilon$ on Success Rate (LCBS)")
plt.grid(True)
# plt.legend(title="LCBS $\\epsilon$:", loc="center left", bbox_to_anchor=(1, 0.5))
plt.tight_layout()

# Save and show
plt.savefig("PLOT_LCBS_EPSILON_VARY.png", dpi=300)
plt.show()
