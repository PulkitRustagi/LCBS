import pandas as pd
import matplotlib.pyplot as plt

time_limit = "1" + "sec"  # seconds

# Read from the file directly
data = pd.read_csv("../data/sim_data_algos_"+time_limit+".txt")

# Plot configuration
plt.figure(figsize=(6, 3))
markers = {
    "BBMOCBS-k (k=5)": 'o',
    "BBMOCBS-k (k=10)": 'x',
    "BBMOCBS-eps": 's',
    "BBMOCBS-pex": 'v',
    "LCBS": 'P',
}   
colors = {
    "BBMOCBS-k (k=5)": 'black',
    "BBMOCBS-k (k=10)": 'gray',
    "BBMOCBS-eps": 'cyan',
    "BBMOCBS-pex": 'blue',
    "LCBS": 'red',
}
linestyles = {
    "BBMOCBS-k (k=5)": 'dotted',
    "BBMOCBS-k (k=10)": 'dotted',
    "BBMOCBS-eps": 'dotted',
    "BBMOCBS-pex": 'dotted',
    "LCBS": 'solid',
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
plt.title("Success Rate of Algorithms (T = "+time_limit+")")
plt.tight_layout()

# Save plot
plt.savefig("../figures/SUCCESS_RATE_PLOT_BASELINES_"+time_limit+".png", dpi=150)
plt.show()
