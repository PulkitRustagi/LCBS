import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import re

# Load the data from a file
data = pd.read_csv("sim_data_eps_vary_test.txt")  # <-- Update this filename as needed

# Format labels: LCBS (ε=...) in LaTeX
def format_label(algo):
    match = re.search(r'LCBS \(\\epsilon = ([\d\.]+)\)', algo)
    if match:
        return rf"LCBS ($\epsilon={match.group(1)}$)"
    return algo  # BBMOCBS variants stay unchanged

data['FormattedLabel'] = data['Algorithm'].apply(format_label)
unique_algos = data['FormattedLabel'].unique()

# Styles
colors = plt.cm.tab10(np.linspace(0, 1, len(unique_algos)))
markers = ['o', 's', '^', 'v', 'D', 'P', '*', 'x', '+']
bbmocbs_styles = ['dotted']

# Initialize plot
plt.figure(figsize=(7.5, 4))
bbmocbs_style_idx = 0

for idx, label in enumerate(unique_algos):
    subset = data[data['FormattedLabel'] == label]

    if "LCBS" in label:
        linestyle = 'solid'
    else:
        linestyle = bbmocbs_styles[bbmocbs_style_idx % len(bbmocbs_styles)]
        bbmocbs_style_idx += 1

    plt.plot(
        subset['Agents'],
        subset['SuccessRate'],
        label=label,
        color=colors[idx % len(colors)],
        marker=markers[idx % len(markers)],
        linestyle=linestyle,
        linewidth=1.8
    )

# Axis and style
plt.xlabel("Agents")
plt.ylabel("Success Rate")
plt.ylim(-0.05, 1.05)
plt.grid(True, linestyle='--', alpha=0.5)
plt.legend(title="Algorithm", loc="center left", bbox_to_anchor=(1, 0.5), fontsize="small")
plt.title("BASELINE DOMINATION PLOT")
plt.tight_layout()

# Save and show
plt.savefig("LCBS_VARIANT_DOMINATION.png", dpi=300)
plt.show()