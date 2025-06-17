#pragma once

#include "Definitions.h"
#include "HighLevel/HighLevelSolver.h"
#include "LowLevel/Utils/MapQueue.h"
#include "HighLevel/ConflictResolver.h"
#include "LowLevel/RunLAstar.h" // <- to be implemented

class LCBS : public HighLevelSolver {
protected:
    void calculate_CAT(HighLevelNodePtr node, VertexCAT& vertex_cat, EdgeCAT& edge_cat, int agent_id);

public:
    virtual ~LCBS() = default;
    LCBS(size_t graph_size, int agent_num, Algorithm algorithm, bool if_eager, int dim, int turn_dim, int turn_cost, int time_limit)
        : HighLevelSolver(graph_size, agent_num, algorithm, if_eager, dim, turn_dim, turn_cost, time_limit) {}

    OutputTuple run(std::vector<Edge>& edges,
                    std::vector<std::pair<size_t, size_t>>& start_end,
                    HSolutionID& hsolution_ids,
                    std::vector<CostVector>& hsolution_costs,
                    LoggerPtr& logger) override;
};
