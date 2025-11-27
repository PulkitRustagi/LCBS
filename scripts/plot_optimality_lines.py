# plot_optimality_lines.py

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# ---------------- Configuration ----------------

BASE_DIR = "../data/OPTIMALITY_CHECK"  # folder containing LCBS.csv, BBMOCBS_k1.csv, ...

# Time limits you want on the x-axis (in decreasing order)
TIME_LIMITS = [120, 60, 30, 10, 5, 1]

# Algorithm IDs and pretty labels for the legend
ALGORITHMS = [
    ("LCBS",         "LCBS"),
    ("BBMOCBS_k1",   "BB-MO-CBS-k (k=1)"),
    ("BBMOCBS_k5",   "BB-MO-CBS-k (k=5)"),
    ("BBMOCBS_k10",  "BB-MO-CBS-k (k=10)"),
    ("BBMOCBS_pex",  "BB-MO-CBS-pex"),
    ("BBMOCBS_eps",  "BB-MO-CBS-ε"),
]

# Optional: choose colors so LCBS stands out
COLOR_MAP = {
    "LCBS":         "red",   # LCBS (ours)
    "BBMOCBS_k1":   "gold",  # BBMOCBS-k (k=1)
    "BBMOCBS_k5":   "black", # BBMOCBS-k (k=5)
    "BBMOCBS_k10":  "gray",  # BBMOCBS-k (k=10)
    "BBMOCBS_pex":  "blue",  # BBMOCBS-pex
    "BBMOCBS_eps":  "lightblue",  # BBMOCBS-eps
}

# ---------------- Helper ----------------

def load_stats_for_algorithm(alg_id):
    """
    Load <alg_id>.csv and return:
      times (np.array of TIME_LIMITS in decreasing order),
      mean_opt (np.array),
      std_opt (np.array)
    where the statistics are over all maps for each time.
    """
    path = os.path.join(BASE_DIR, f"{alg_id}.csv")
    df = pd.read_csv(path)

    # Group by time, average across maps
    grouped = df.groupby("time_limit_sec")["optimality_percentage"]
    mean = grouped.mean()
    std  = grouped.std(ddof=0)  # population std (ddof=0); use ddof=1 if you prefer sample std

    # Align to TIME_LIMITS in the specified order
    means = []
    stds  = []
    for t in TIME_LIMITS:
        if t in mean.index:
            means.append(mean.loc[t])
            stds.append(std.loc[t])
        else:
            # If some time is missing, fall back to NaN
            means.append(np.nan)
            stds.append(np.nan)

    return np.array(TIME_LIMITS), np.array(means), np.array(stds)

# ---------------- Plotting ----------------

def main():
    # x-coordinates as equally spaced indices; labels will be the actual times
    x_idx = np.arange(len(TIME_LIMITS))

    fig, ax = plt.subplots(figsize=(5, 3))

    for alg_id, label in ALGORITHMS:
        times, mean_opt, std_opt = load_stats_for_algorithm(alg_id)

        color = COLOR_MAP.get(alg_id, None)

        # Line
        ax.plot(
            x_idx,
            mean_opt,
            label=label,
            color=color,
        )

        # Shaded ±1 std band
        lower = mean_opt - std_opt
        upper = mean_opt + std_opt
        ax.fill_between(
            x_idx,
            lower,
            upper,
            color=color,
            alpha=0.2,
            linewidth=0,
        )

    # Axes labels and ticks
    ax.set_xlabel("Time limit (s)", fontsize=10)
    ax.set_ylabel("Optimality (%)", fontsize=10)

    ax.set_xticks(x_idx)
    ax.set_xticklabels([str(t) for t in TIME_LIMITS], fontsize=9)

    ax.set_ylim(0, 105)
    ax.set_yticks(np.arange(0, 101, 20))

    # Optional: invert x so that time decreases left->right or right->left
    # Currently TIME_LIMITS is [120, 60, 30, 10, 5, 1],
    # so it already decreases left -> right.
    # If you ever reorder TIME_LIMITS ascending, call ax.invert_xaxis().

    ax.grid(True, linestyle="--", alpha=0.3)
    ax.legend(fontsize=8, frameon=False, loc="lower left")

    fig.tight_layout()
    fig.savefig("../figures/optimality_lines_all_algs.png", dpi=300, bbox_inches="tight")
    plt.close(fig)

    print("Saved optimality_lines_all_algs.[pdf|png]")

if __name__ == "__main__":
    main()
