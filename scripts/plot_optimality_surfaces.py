# plot_optimality_surfaces.py
import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401

# ---------------- Configuration ----------------

BASE_DIR = "../data/OPTIMALITY_CHECK"
OUTPUT_DIR = "../figures"

MAP_NAMES = [
    "empty-32-32",
    # "warehouse-10-20-10-2-1",
    "random-32-32-20",
    "random-64-64-20",
    "room-64-64-8",
    "room-32-32-4",
    "empty-48-48",
    "maze-32-32-4",
    "random-64-64-10",
    "maze-32-32-2",
]

TIME_LIMITS = [120, 60, 30, 10, 5, 1]  # decreasing

# (CSV file stem, pretty title)
ALGORITHMS = [
    ("LCBS",         "LCBS (ours)"),
    ("BBMOCBS_k1",   "BB-MO-CBS-k (k=1)"),
    ("BBMOCBS_k5",   "BB-MO-CBS-k (k=5)"),
    ("BBMOCBS_k10",  "BB-MO-CBS-k (k=10)"),
    ("BBMOCBS_pex",  "BB-MO-CBS-pex"),
    ("BBMOCBS_eps",  "BB-MO-CBS-ε"),
]

# ---------------- Utilities ----------------

def load_surface_for_algorithm(alg_id):
    """
    Load a (len(TIME_LIMITS) x len(MAP_NAMES)) matrix Z with
    optimality percentages for algorithm `alg_id`.
    """
    n_maps = len(MAP_NAMES)
    n_times = len(TIME_LIMITS)

    # map_name -> index; time_limit -> index
    map_index = {m: i for i, m in enumerate(MAP_NAMES)}
    time_index = {t: i for i, t in enumerate(TIME_LIMITS)}

    Z = np.full((n_times, n_maps), np.nan, dtype=float)

    csv_path = os.path.join(BASE_DIR, f"{alg_id}.csv")
    if not os.path.exists(csv_path):
        raise FileNotFoundError(f"CSV not found: {csv_path}")

    with open(csv_path, "r") as f:
        header = f.readline()
        for line in f:
            if not line.strip():
                continue
            parts = [x.strip() for x in line.split(",")]
            if len(parts) < 3:
                continue
            map_name, time_str, pct_str = parts[:3]

            if map_name not in map_index:
                continue
            try:
                time_val = int(time_str)
                pct_val = float(pct_str)
            except ValueError:
                continue
            if time_val not in time_index:
                continue

            ti = time_index[time_val]
            mi = map_index[map_name]
            Z[ti, mi] = pct_val

    return Z


def ensure_dir(path):
    if not os.path.exists(path):
        os.makedirs(path, exist_ok=True)

# ---------------- Plotting ----------------
def plot_surface_for_algorithm(alg_id, title, Z):
    """
    Generate and save a 3D surface plot for one algorithm.

    - X: benchmarks, shown as B10..B1 (reversed)
    - Y: time limits, shown as 120..1 (reversed), equally spaced
    - Z: optimality percentage
    - Colorbar: fixed 0–100 (yellow high, red low)
    """
    # ---- keep only times >= 5s ----
    TIME_LIMITS_ = TIME_LIMITS[:-1]  # [120, 60, 30, 10, 5]
    Z = Z[:-1, :]
    n_maps  = len(MAP_NAMES)
    n_times = len(TIME_LIMITS_)

    # ----- reorder time dimension: largest time first (e.g., 120 -> 1) -----
    times = np.array(TIME_LIMITS_, dtype=float)
    time_order = np.argsort(times)[::-1]         # indices in descending time
    times_desc = times[time_order]               # e.g. [120, 60, 30, 10, 5, 1]

    # ----- reorder benchmark dimension: B10 -> B1 -----
    bench_order = np.arange(n_maps)[::-1]        # 9..0

    # apply both permutations to Z
    Z_plot = Z[time_order, :][:, bench_order]

    # equally spaced indices along each axis
    x_idx = np.arange(n_maps)
    y_idx = np.arange(n_times)

    X, Y = np.meshgrid(x_idx, y_idx)

    fig = plt.figure(figsize=(4, 3))
    ax  = fig.add_subplot(111, projection="3d")

    # Surface with fixed color range 0–100
    surf = ax.plot_surface(
        X, Y, Z_plot,           # or Z, depending on your code
        cmap="RdYlGn",          # red -> yellow -> green (0 -> 100)
        edgecolor="k",
        linewidth=0.3,
        antialiased=True,
        rstride=1,
        cstride=1,
        vmin=0.0,
        vmax=100.0,             # keep fixed 0–100 range
    )

    ax.set_title(title, fontsize=9)

    ax.set_xlabel("Benchmark", fontsize=8)
    ax.set_ylabel("Time limit (s)", fontsize=8)
    ax.set_zlabel("Optimality (%)", fontsize=8)

    # X ticks: B10..B1
    ax.set_xticks(x_idx)
    ax.set_xticklabels(
        [f"B{i}" for i in range(n_maps, 0, -1)],  # B10, B9, ..., B1
        fontsize=7,
        rotation=0,
        ha="right",
    )

    # Y ticks: 120..1 (descending), equally spaced
    ax.set_yticks(y_idx)
    ax.set_yticklabels([str(int(t)) for t in times_desc], fontsize=7)
    ax.invert_yaxis()

    # Z axis fixed 0–100
    ax.set_zlim(0.0, 100.0)

    # View angle (tune if desired)
    ax.view_init(elev=30, azim=-135)

    # Colorbar shared range 0–100
    cbar = fig.colorbar(surf, shrink=0.55, aspect=12, pad=0.08)
    cbar.set_label("Optimality (%)", fontsize=8)
    cbar.set_ticks([0, 20, 40, 60, 80, 100])

    plt.tight_layout()

    ensure_dir(OUTPUT_DIR)
    out_base = os.path.join(OUTPUT_DIR, f"optimality_{alg_id}")
    fig.savefig(out_base + ".png", dpi=300, bbox_inches="tight")
    plt.close(fig)




# def plot_surface_for_algorithm(alg_id, title, Z):
#     """
#     Generate and save a 3D surface plot for one algorithm.
#     """
#     n_maps = len(MAP_NAMES)
#     n_times = len(TIME_LIMITS)

#     # X axis: benchmarks 1..10 (B1..B10)
#     x_vals = np.arange(1, n_maps + 1)
#     # Y axis: time limits (already in decreasing order)
#     y_vals = np.array(TIME_LIMITS)

#     X, Y = np.meshgrid(x_vals, y_vals)

#     fig = plt.figure(figsize=(4, 3))
#     ax = fig.add_subplot(111, projection="3d")

#     # Surface: yellow (high) to red (low)
#     surf = ax.plot_surface(
#         X, Y, Z,
#         cmap="YlOrRd_r",
#         edgecolor="k",
#         linewidth=0.3,
#         antialiased=True,
#         rstride=1,
#         cstride=1,
#     )

#     ax.set_title(title, fontsize=9)

#     ax.set_xlabel("Benchmark", fontsize=8)
#     ax.set_ylabel("Time limit (s)", fontsize=8)
#     ax.set_zlabel("Optimality (%)", fontsize=8)

#     # X ticks as B1..B10
#     ax.set_xticks(x_vals)
#     ax.set_xticklabels([f"B{i}" for i in x_vals], fontsize=7, rotation=45, ha="right")

#     # Y ticks are the time limits; keep decreasing order
#     ax.set_yticks(y_vals)
#     ax.set_yticklabels([str(t) for t in y_vals], fontsize=7)

#     # Z axis from 0 to 100
#     ax.set_zlim(0.0, 100.0)

#     # View similar to your existing figure (adjust if needed)
#     ax.view_init(elev=30, azim=-135)

#     fig.colorbar(surf, shrink=0.55, aspect=12, pad=0.08)

#     plt.tight_layout()

#     ensure_dir(OUTPUT_DIR)
#     out_base = os.path.join(OUTPUT_DIR, f"optimality_{alg_id}")
#     # fig.savefig(out_base + ".pdf", bbox_inches="tight")
#     fig.savefig(out_base + ".png", dpi=300, bbox_inches="tight")
#     plt.close(fig)


def main():
    for alg_id, title in ALGORITHMS:
        Z = load_surface_for_algorithm(alg_id)
        plot_surface_for_algorithm(alg_id, title, Z)
        print(f"Saved surface for {alg_id}")

if __name__ == "__main__":
    main()
