import random
import os

def parse_map(map_path):
    with open(map_path, "r") as f:
        lines = f.readlines()

    height = int([line for line in lines if line.startswith("height")][0].split()[1])
    width = int([line for line in lines if line.startswith("width")][0].split()[1])
    map_start_idx = lines.index("map\n") + 1
    grid = [list(line.strip()) for line in lines[map_start_idx:]]
    free_cells = [(x, y) for y in range(height) for x in range(width) if grid[y][x] == '.']
    return width, height, free_cells

def generate_scenario_files(map_file, output_dir, num_scenarios=10, agents_per_scenario=100):
    width, height, free_cells = parse_map(map_file)
    map_name = os.path.basename(map_file)
    os.makedirs(output_dir, exist_ok=True)

    if len(free_cells) < 2 * agents_per_scenario:
        raise ValueError("Not enough free cells on the map to assign unique start and goal positions.")

    for scen_num in range(num_scenarios):
        sampled = random.sample(free_cells, 2 * agents_per_scenario)
        starts = sampled[:agents_per_scenario]
        goals = sampled[agents_per_scenario:]

        output_path = os.path.join(output_dir, f"{map_name.replace('.map', '')}-random-{scen_num}.scen")
        with open(output_path, "w") as f:
            f.write("version 1\n")
            for agent_id, (start, goal) in enumerate(zip(starts, goals)):
                distance = ((start[0] - goal[0]) ** 2 + (start[1] - goal[1]) ** 2) ** 0.5
                f.write(f"{agent_id}\t{map_name}\t{width}\t{height}\t{start[0]}\t{start[1]}\t{goal[0]}\t{goal[1]}\t{distance:.8f}\n")

# Example usage:
generate_scenario_files("../maps/custom-32-32-2/custom-32-32-2.map", "../maps/custom-32-32-2/scen-random")
