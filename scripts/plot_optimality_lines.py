# plot_optimality_lines.py
#
# Plots optimality vs time limit, averaged over all benchmarks,
# for all algorithms on a single 2D plot with mean ± std shading.
#
# Expects CSV files:
#   LCBS.csv
#   BBMOCBS_k1.csv
#   BBMOCBS_k5.csv
#   BBMOCBS_k10.csv
#   BBMOCBS_pex.csv
#   BBMOCBS_eps.csv
#
# Each with columns:
#   map,time_limit_sec,optimality_percentage

import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# ---------------- Configuration ----------------

# Folder containing LCBS.csv, BBMOCBS_k1.csv, etc.
BASE_DIR = "../data/OPTIMALITY_CHECK"

# Time limits on the x-axis (in decreasing order)
TIME_LIMITS = [120, 60, 30, 10, 5]

# Algorithm IDs (matching CSV file stems) and legend labels
ALGORITHMS = [
    ("LCBS",         "LCBS (ours)"),
    ("BBMOCBS_k1",   "BB-MO-CBS-k (k=1)"),
    ("BBMOCBS_k5",   "BB-MO-CBS-k (k=5)"),
    ("BBMOCBS_k10",  "BB-MO-CBS-k (k=10)"),
    ("BBMOCBS_eps",  "BB-MO-CBS-" + r"$\varepsilon$"),
    ("BBMOCBS_pex",  "BB-MO-CBS-pex"),
]

# Color scheme (matched to success-rate plots)
COLOR_MAP = {
    "LCBS":         "red",   # LCBS (ours)
    "BBMOCBS_k1":   "gold",  # BBMOCBS-k (k=1)
    "BBMOCBS_k5":   "black", # BBMOCBS-k (k=5)
    "BBMOCBS_k10":  "gray",  # BBMOCBS-k (k=10)
    "BBMOCBS_pex":  "blue",  # BBMOCBS-pex
    "BBMOCBS_eps":  "cyan",  # BBMOCBS-eps
}

# Marker scheme (matched to success-rate plots)
MARKER_MAP = {
    "LCBS":         "P",  # filled plus
    "BBMOCBS_k1":   "D",  # diamond
    "BBMOCBS_k5":   "o",  # circle
    "BBMOCBS_k10":  "x",  # x
    "BBMOCBS_pex":  "v",  # triangle down
    "BBMOCBS_eps":  "s",  # square
}

# Linestyles (solid for LCBS, dotted for baselines)
LINESTYLE_MAP = {
    "LCBS":         "solid",
    "BBMOCBS_k1":   "dotted",
    "BBMOCBS_k5":   "dotted",
    "BBMOCBS_k10":  "dotted",
    "BBMOCBS_pex":  "dotted",
    "BBMOCBS_eps":  "dotted",
}

# ---------------- Helper ----------------

def load_stats_for_algorithm(alg_id):
    """
    Load <alg_id>.csv and return:
      times (np.array of TIME_LIMITS in decreasing order),
      mean_opt (np.array),
      std_opt (np.array)
    where statistics are over all maps for each time.
    """
    path = os.path.join(BASE_DIR, f"{alg_id}.csv")
    df = pd.read_csv(path)

    # Group by time, average across maps
    grouped = df.groupby("time_limit_sec")["optimality_percentage"]
    mean = grouped.mean()
    std  = grouped.std(ddof=0)  # population std; use ddof=1 for sample std if preferred

    means = []
    stds  = []
    for t in TIME_LIMITS:
        if t in mean.index:
            means.append(mean.loc[t])
            stds.append(std.loc[t])
        else:
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

        color     = COLOR_MAP.get(alg_id, None)
        marker    = MARKER_MAP.get(alg_id, "o")
        linestyle = LINESTYLE_MAP.get(alg_id, "solid")

        # Main line
        ax.plot(
            x_idx,
            mean_opt,
            label=label,
            color=color,
            marker=marker,
            linestyle=linestyle,
            linewidth=3.0,
            markersize=8,
        )

        # Shaded ±1 std band
        lower = mean_opt - std_opt
        upper = mean_opt + std_opt
        ax.fill_between(
            x_idx,
            lower,
            upper,
            color=color,
            alpha=0.20,
            linewidth=0,
        )

    # Axes labels and ticks
    ax.set_xlabel("Time limit (sec)", fontsize=12)
    ax.set_ylabel("Percentage of scenarios with\npreference-optimal paths (%)", fontsize=12)

    ax.set_xticks(x_idx)
    ax.set_xticklabels([str(t) for t in TIME_LIMITS], fontsize=11)

    ax.set_ylim(0, 105)
    ax.set_yticks(np.arange(0, 101, 20))
    ax.tick_params(axis="y", labelsize=11)

    # Decreasing times are already ordered in TIME_LIMITS, so no invert needed.
    # If you ever reorder TIME_LIMITS ascending, you could call ax.invert_xaxis().

    ax.grid(True, linestyle="--", alpha=0.3)

    ax.legend(fontsize=9, frameon=False, loc="lower left")

    fig.tight_layout()
    fig.savefig("../figures/optimality_lines_all_algs.png", dpi=300, bbox_inches="tight")
    plt.close(fig)

    print("Saved optimality_lines_all_algs.[pdf|png]")

if __name__ == "__main__":
    main()
