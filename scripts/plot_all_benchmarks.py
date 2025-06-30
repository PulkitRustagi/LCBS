import pandas as pd
import matplotlib.pyplot as plt


def clean_algo_name(algo_name):
    """Clean the algorithm name for better readability on the plots."""
    if algo_name == "LCBS -k 1":
        return "LCBS"
    elif algo_name == "BBMOCBS-k -k 5":
        return "BBMOCBS-k (k=5)"
    elif algo_name == "BBMOCBS-k -k 10":
        return "BBMOCBS-k (k=10)"
    elif algo_name == "BBMOCBS-eps":
        return "BBMOCBS-eps"
    elif algo_name == "BBMOCBS-pex":
        return "BBMOCBS-pex"
    else:
        return algo_name
    
# time_limit = "120" + "sec"  # seconds

map_names = ["empty-32-32", 
             "empty-48-48",
             "maze-32-32-2",
             "maze-32-32-4",
             "random-32-32-20",
             "random-64-64-10",
             "random-64-64-20",
             "room-32-32-4",
             "room-64-64-16",
             "room-64-64-8"]

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

for map_name in map_names:
    # Plot configuration
    plt.figure(figsize=(6, 3))
    # Read from the file directly
    data = pd.read_csv("../data/results_"+map_name+"_120sec.txt")

    # Plot lines for each algorithm
    for algo in data['Algorithm'].unique():
        subset = data[data['Algorithm'] == algo]
        algo_name = clean_algo_name(algo)  
        plt.plot(
            subset['Agents'],
            subset['SuccessRate'],
            marker=markers.get(algo_name, 'o'),
            color=colors.get(algo_name, None),
            linestyle=linestyles.get(algo_name, 'solid'),
            label=algo_name
        )


    # Axis labels and styling
    plt.xlabel("Agents")
    plt.ylabel("Success Rate")
    plt.ylim(-0.05, 1.05)
    plt.grid(True)
    # plt.legend(title="success rate:", loc="center left", bbox_to_anchor=(1, 0.5))
    # plt.title("Success Rate of Algorithms")
    plt.tight_layout()

    # Save plot
    plt.savefig("../figures/benchmarks/"+map_name+".png", dpi=150)
    # plt.show()
