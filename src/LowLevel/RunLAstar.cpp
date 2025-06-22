#include "LowLevel/RunLAstar.h"
#include <queue>
#include <set>
#include <limits>
#include <cmath>

RunLAstar::RunLAstar(size_t graph_size,
                     const AdjacencyMatrix& graph,
                     Heuristic& heuristic,
                     size_t source,
                     size_t target,
                     LoggerPtr logger,
                     unsigned int time_limit,
                     VertexConstraint& vertex_constraints,
                     EdgeConstraint& edge_constraints,
                     int turn_dim,
                     int turn_cost)
    : graph_size_(graph_size),
      graph_(graph),
      heuristic_(heuristic),
      source_(source),
      target_(target),
      logger_(logger),
      time_limit_(time_limit),
      v_constraints_(vertex_constraints),
      e_constraints_(edge_constraints),
      turn_dim_(turn_dim),
      turn_cost_(turn_cost)
{}

bool RunLAstar::is_constrained(size_t node_id, size_t timestep) {
    auto it = v_constraints_.find(timestep);
    return it != v_constraints_.end() && std::find(it->second.begin(), it->second.end(), node_id) != it->second.end();
}

bool RunLAstar::is_edge_constrained(size_t from, size_t to, size_t timestep) {
    auto it = e_constraints_.find(timestep);
    return it != e_constraints_.end() &&
           it->second.count(from) &&
           std::find(it->second.at(from).begin(), it->second.at(from).end(), to) != it->second.at(from).end();
}

bool RunLAstar::search(PathSet& waypoints,
                       CostSet& apex_costs,
                       CostSet& real_costs,
                       std::unordered_map<int, int>& conflict_num_map)
{
    using PQ = std::priority_queue<Node, std::vector<Node>, std::greater<Node>>;
    PQ open;
    std::unordered_map<size_t, CostVector> best_cost;

    CostVector init_g(graph_.get_num_of_objectives(), 0);
    CostVector h = heuristic_(source_, target_, 0);
    open.push({source_, init_g, h, {source_}});
    best_cost[source_] = init_g;

    // size_t timestep = 0;
    int solution_id = 0; // since LAstar is programmed to have just a single solution
    while (!open.empty()) {
        Node curr = open.top(); open.pop();
        size_t curr_node = curr.id;
        CostVector g = curr.g;
        std::vector<size_t> path = curr.path;

        // Constraint check
        if (is_constrained(curr_node, path.size() - 1)) continue;

        if (curr_node == target_) {
            waypoints[solution_id] = path;
            apex_costs[solution_id] = g;
            real_costs[solution_id] = g;
            return true;
        }

        for (const auto& neighbor : graph_[curr_node]) {
            size_t next_id = neighbor.target;
            if (is_edge_constrained(curr_node, next_id, path.size() - 1)) continue;

            CostVector new_g = g;
            for (size_t i = 0; i < new_g.size(); ++i)
                new_g[i] += neighbor.cost[i];

            if (best_cost.count(next_id) && new_g >= best_cost[next_id]) continue;

            CostVector new_h = heuristic_(next_id, target_, path.size());
            std::vector<size_t> new_path = path;
            new_path.push_back(next_id);
            open.push({next_id, new_g, new_h, new_path});
            best_cost[next_id] = new_g;
        }
    }

    return false;
}
