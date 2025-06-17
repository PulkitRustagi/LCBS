#pragma once

#include "Definitions.h"
#include "ShortestPathHeuristic.h"
#include "LowLevel/Utils/Logger.h"

#include <queue>
#include <unordered_set>
#include <unordered_map>

class RunLAstar {
public:
    RunLAstar(size_t graph_size,
              const AdjacencyMatrix& graph,
              Heuristic& heuristic,
              size_t source,
              size_t target,
              LoggerPtr logger,
              unsigned int time_limit,
              VertexConstraint& vertex_constraints,
              EdgeConstraint& edge_constraints,
              int turn_dim,
              int turn_cost);

    virtual std::string get_solver_name() {return "LAstar"; }

    // Returns true if path found
    bool search(PathSet& waypoints,
                CostSet& apex_costs,
                CostSet& real_costs,
                std::unordered_map<int, int>& conflict_num_map);

private:
    size_t graph_size_;
    const AdjacencyMatrix& graph_;
    Heuristic& heuristic_;
    size_t source_, target_;
    LoggerPtr logger_;
    unsigned int time_limit_;
    VertexConstraint& v_constraints_;
    EdgeConstraint& e_constraints_;
    int turn_dim_;
    int turn_cost_;

    struct Node {
        size_t id;
        CostVector g;  // accumulated cost
        CostVector h;  // heuristic
        std::vector<size_t> path;

        bool operator>(const Node& other) const {
            for (size_t i = 0; i < g.size(); ++i) {
                if (g[i] != other.g[i])
                    return g[i] > other.g[i];
            }
            return false;
        }
        bool operator<(const Node& other) const {
            for (size_t i = 0; i < g.size(); ++i) {
                if (g[i] != other.g[i])
                    return g[i] < other.g[i];
            }
            return false;
        }
    };

    struct NodeHasher {
        size_t operator()(const std::pair<size_t, int>& p) const {
            return std::hash<size_t>()(p.first) ^ std::hash<int>()(p.second);
        }
    };

    bool is_constrained(size_t node_id, size_t timestep);
    bool is_edge_constrained(size_t from, size_t to, size_t timestep);
};
