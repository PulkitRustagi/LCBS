'''
This script generates 3D contour plots for the success rates of various algorithms across different numbers of objectives and agents.
'''

import os
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D  # needed even if not used directly

map_names =["empty-32-32",
            "empty-48-48",
            "maze-32-32-2",
            "maze-32-32-4",
            "random-32-32-20",
            "random-64-64-10",
            "random-64-64-20",
            "room-32-32-4",
            "room-64-64-8"]

for map_name in map_names:
    # Input data folder and output figures folder
    data_folder = "../data/dimensional_trends/" + map_name
    figure_folder = "../figures/contours/" + map_name
    os.makedirs(figure_folder, exist_ok=True)

    # Mapping of method display names to file names
    method_files = {
        "LCBS": "LCBS_new.txt",
        "BBMOCBS-k_1": "BBMOCBS-k_k_1.txt",
        "BBMOCBS-k_5": "BBMOCBS-k_k_5.txt",
        "BBMOCBS-k_10": "BBMOCBS-k_k_10.txt",
        "BBMOCBS-eps": "BBMOCBS-eps.txt",
        "BBMOCBS-pex": "BBMOCBS-pex.txt"
    }

    algorith_name = {
        "LCBS": "LCBS",
        "BBMOCBS-k_1": "BBMOCBS-k (k=1)",
        "BBMOCBS-k_5": "BBMOCBS-k (k=5)",
        "BBMOCBS-k_10": "BBMOCBS-k (k=10)",
        "BBMOCBS-eps": r"BBMOCBS-$\epsilon$",
        "BBMOCBS-pex": "BBMOCBS-pex"
    }

    # Function to parse a method's data file
    def parse_file(filepath):
        objectives, agents, success = [], [], []

        with open(filepath, "r") as f:
            lines = f.readlines()[1:]  # Skip header
            for line in lines:
                if line.strip():
                    obj, ag, sr = line.strip().split(",")
                    objectives.append(int(obj))
                    agents.append(int(ag))
                    success.append(float(sr))

        return np.array(objectives), np.array(agents), np.array(success)

    # Generate and save 3D contour plots
    for method_name, filename in method_files.items():
        filepath = os.path.join(data_folder, filename)
        if not os.path.exists(filepath):
            print(f"File not found: {filepath}")
            continue

        obj, ag, sr = parse_file(filepath)

        # Create meshgrid
        unique_obj = np.unique(obj)
        unique_ag = np.unique(ag)
        X, Y = np.meshgrid(unique_obj, unique_ag)
        Z = np.full(X.shape, np.nan)

        for o, a, s in zip(obj, ag, sr):
            i = np.where(unique_ag == a)[0][0]
            j = np.where(unique_obj == o)[0][0]
            Z[i, j] = s

        # Plotting
        fig = plt.figure(figsize=(8, 6))
        ax = fig.add_subplot(111, projection='3d')
        surf = ax.plot_surface(X, Y, Z, cmap=cm.viridis, edgecolor='k', linewidth=0.3, antialiased=True, vmin=0.0, vmax=1.0)

        # ax.set_title(f"{method_name}", fontsize=20, fontweight='bold', pad=10)
        ax.text(
        -20,-1600,22,  # x, y, z in axes fraction (or you can use data coordinates)
        f"{algorith_name[method_name]}",
        transform=ax.transAxes,
        ha='center',
        fontsize=20,
        fontweight='bold'
    )
        ax.set_xlabel("\n#Objectives", fontsize=16, fontweight='bold')
        ax.set_ylabel("\n#Agents", fontsize=16, fontweight='bold')
        # ax.set_zlabel("Success Rate", rotation=100, fontsize=15)
        ax.zaxis.labelpad = 5
        ax.xaxis.set_tick_params(labelsize=15)
        ax.yaxis.set_tick_params(labelsize=15)
        ax.zaxis.set_tick_params(labelsize=15)
        ax.set_zlim(0, 1.0)
        ax.invert_yaxis()
        # ax.view_init(elev=30, azim=-270)
        
        cbar = fig.colorbar(surf, shrink=0.6, aspect=10)
        cbar.set_label("Success Rate", rotation=90, labelpad=-64, fontsize=15, fontweight='bold')
        cbar.ax.tick_params(labelsize=12)


        output_path = os.path.join(figure_folder, f"{method_name}.png")
        plt.tight_layout()
        plt.savefig(output_path, bbox_inches='tight', pad_inches=0)
        plt.close(fig)  # Close figure to free memory

        print(f"Saved: {output_path}")
