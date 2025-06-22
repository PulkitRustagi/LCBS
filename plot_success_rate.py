import pandas as pd
import matplotlib.pyplot as plt

# Read from the file directly
data = pd.read_csv("sim_data.txt")

# Plot configuration
plt.figure(figsize=(6, 3))
markers = {
    "BBMOCBS-k": 'o',
    "BBMOCBS-eps": 's',
    "BBMOCBS-pex": 'v',
    "LCBS": 'P',
}
colors = {
    "BBMOCBS-k": 'black',
    "BBMOCBS-eps": 'cyan',
    "BBMOCBS-pex": 'blue',
    "LCBS": 'red',
}
linestyles = {
    "BBMOCBS-k": 'solid',
    "BBMOCBS-eps": 'solid',
    "BBMOCBS-pex": 'solid',
    "LCBS": 'dashed',
}

# Plot lines for each algorithm
for algo in data['Algorithm'].unique():
    subset = data[data['Algorithm'] == algo]
    plt.plot(
        subset['Agents'],
        subset['SuccessRate'],
        marker=markers.get(algo, 'o'),
        color=colors.get(algo, None),
        linestyle=linestyles.get(algo, 'solid'),
        label=algo
    )

# Axis labels and styling
plt.xlabel("Agents")
plt.ylabel("Success Rate")
plt.ylim(-0.05, 1.05)
plt.grid(True)
plt.legend(title="success rate:", loc="center left", bbox_to_anchor=(1, 0.5))
plt.tight_layout()

# Save plot
plt.savefig("success_rate_simulation_plot_test.png", dpi=300)
plt.show()
