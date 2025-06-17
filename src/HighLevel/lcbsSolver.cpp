#include "HighLevel/lcbsSolver.h"

#include "Utils.h"
#include "HighLevel/ConflictResolver.h"

#include <cstdlib>
#include <list>
#include <algorithm>
#include <set>
#include <random>
#include <iostream>
#include <vector>

extern std::unordered_map<size_t, std::vector<int>> id2coord;
extern std::string map_name;
extern std::ofstream output;

void LCBS::calculate_CAT(HighLevelNodePtr node, VertexCAT& vertex_cat, EdgeCAT& edge_cat, int agent_id)
{
    vertex_cat.clear(); edge_cat.clear();
    for(int i = 0; i < node->cur_ids.size(); i++){
        if(i == agent_id){
            continue;
        }
        auto path_nodes = node->sop_waypoints.at(i)[node->cur_ids.at(i)];
        for(int t = 0; t < path_nodes.size(); t++){
            size_t node_id = path_nodes.at(t);
            if(!vertex_cat.count(t)){
                vertex_cat.insert(std::make_pair(t, std::vector<int>(GRAPH_SIZE, 0)));
            }
            vertex_cat.at(t).at(node_id) ++;
        }
        for(int t = 0; t < path_nodes.size() - 1; t++){
            size_t source_id = path_nodes.at(t);
            size_t target_id = path_nodes.at(t+1);
            if(!edge_cat.count(t)){
                edge_cat.insert(std::make_pair(t, std::vector<std::unordered_map<size_t, int>>(GRAPH_SIZE)));
            }
            if(!edge_cat.at(t).at(source_id).count(target_id)){
                edge_cat.at(t).at(source_id).insert(std::make_pair(target_id, 0));
            }
            edge_cat.at(t).at(source_id).at(target_id) ++;
        }
    }
}


OutputTuple LCBS::run(std::vector<Edge>& edges, std::vector<std::pair<size_t, size_t>>& start_goal, HSolutionID& hsolution_ids, std::vector<CostVector>& hsolution_costs, LoggerPtr& logger)
{
    std::chrono::high_resolution_clock::time_point   precise_start_time, precise_end_time;
    precise_start_time = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point t1, t2;
    double duration;

    bool is_success = true;
    double HLMergingTime = 0, LowLevelTime = 0, TotalTime;
    int ConflictSolvingNum = 0;

    ConflictResolver conflict_resolver;

    start_time = time(NULL);
    HLQueue open_list;

    VertexCAT  vertex_cat;
    EdgeCAT    edge_cat;

    // calculate heuristic
    std::vector<Heuristic> heuristics(AGENT_NUM);
    AdjacencyMatrix graph(GRAPH_SIZE, edges);   // can run outside and only once
    AdjacencyMatrix inv_graph(GRAPH_SIZE, edges, true);
    for(int i = 0; i < AGENT_NUM; i++){
        ShortestPathHeuristic sp_heuristic(start_goal.at(i).second, GRAPH_SIZE, inv_graph, TURN_DIM, TURN_COST);
        heuristics.at(i) = std::bind( &ShortestPathHeuristic::operator(), sp_heuristic, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    }

    //  initialize open_list
    HighLevelNodePtr root_node = std::make_shared<HighLevelNode>(AGENT_NUM);

    for (size_t i = 0; i < AGENT_NUM; ++i) {
        t1 = std::chrono::high_resolution_clock::now();

        RunLAstar lex_search(GRAPH_SIZE, graph, heuristics[i], start_goal[i].first, start_goal[i].second,
                            logger, TIME_LIMIT, root_node->vertex_constraints[i], root_node->edge_constraints[i],
                            TURN_DIM, TURN_COST);

        bool success = lex_search.search(root_node->sop_waypoints[i],
                                        root_node->sop_apex[i],
                                        root_node->sop_cost[i],
                                        root_node->conflict_num);

        t2 = std::chrono::high_resolution_clock::now();
        duration = (double)(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()) / 1000000.0;
        LowLevelTime += duration;

        if (!success || root_node->sop_waypoints[i].empty()) {
            output << "FAIL" << std::endl;
            return std::make_tuple(0, LowLevelTime, duration, 0, 0, false);
        }

        std::cout << "Agent " << i << ": Lex A* time = " << duration << ", 1 path found\n";
    }

    if(difftime(time(NULL), start_time) > TIME_LIMIT){
        output << "FAIL" << std::endl ;
            // << "apex cost:" << std::endl 
            // << std::endl 
            // << "real cost:" << std::endl 
            // << std::endl << std::endl << std::endl;

        return std::make_tuple(HLMergingTime, LowLevelTime, difftime(time(NULL), start_time), 0, 0, false);
    }

    root_node->cur_ids.clear();
    for (int i = 0; i < AGENT_NUM; ++i){
        root_node->cur_ids.push_back(0);
    }
    open_list.insert(root_node);

    // std::tuple<int, int, CostVector, size_t> cft;
    //  main loop    
    while(!open_list.empty())
    {
        if(difftime(time(NULL), start_time) > TIME_LIMIT){
            is_success = false;
            break;
        }
        auto node = open_list.pop();
        node->cur_ids.clear();
        for (int i = 0; i < AGENT_NUM; ++i) {
            node->cur_ids.push_back(0); // Always index 0 since only one path per agent
        }

        CostVector dummy_cost(DIM, 0);                 // we don’t use the apex cost in LCBS
        JointPathPair jp(dummy_cost, node->cur_ids);   // build the required pair
        auto cft = conflict_resolver.DetectConflict(jp, node->sop_waypoints);

        if (std::get<2>(cft).empty()) {
            CostVector total_cost(DIM, 0);
            std::vector<std::vector<size_t>> solution;   // instead of HSolutionID here

            for (int i = 0; i < AGENT_NUM; ++i) {
                add_cost(total_cost, node->sop_cost[i][0]);
                solution.push_back(node->sop_waypoints[i][0]);
            }

            hsolution_ids.clear();
            hsolution_costs.clear();
            hsolution_ids.emplace_back(solution);
            hsolution_costs.push_back(total_cost);

            is_success = true;
            break; // done
        }
        
        // print constraint info
        ConflictSolvingNum ++;
        if (ConflictSolvingNum % (500/AGENT_NUM) == 0) {
            std::cout << "[INFO] * Solver::Search, after " << ConflictSolvingNum << " conflict splits " 
            << "       time = " << difftime(time(NULL), start_time) << std::endl
            << map_name << std::endl;
        }


        if(std::get<2>(cft).size() == 1){   // vertex confliction, split 1 or 2 constraints
            for(int i = 0; i < 2; i ++){
                int agent_id = i == 0 ? std::get<0>(cft) : std::get<1>(cft);
                if(agent_id < 0){
                    continue;
                }
                auto new_node = std::make_shared<HighLevelNode>(*node);
                new_node->sop_waypoints.at(agent_id).clear();
                new_node->sop_apex.at(agent_id).clear();
                new_node->sop_cost.at(agent_id).clear();
                new_node->conflict_num.clear();
                


                conflict_resolver.AddConstraint(new_node->vertex_constraints, agent_id, std::get<2>(cft).front(), std::get<3>(cft));

                // //  Low Level Search
                // if(MS == MergingStrategy::CONFLICT_BASED){
                //     calculate_CAT(node, vertex_cat, edge_cat, agent_id);
                // }

                RunLAstar lex_search(GRAPH_SIZE, graph, heuristics[agent_id], start_goal[agent_id].first, start_goal[agent_id].second,
                     logger, TIME_LIMIT, new_node->vertex_constraints[agent_id], new_node->edge_constraints[agent_id],
                     TURN_DIM, TURN_COST);

                t1 = std::chrono::high_resolution_clock::now();
                bool success = lex_search.search(new_node->sop_waypoints[agent_id],
                                                new_node->sop_apex[agent_id],
                                                new_node->sop_cost[agent_id],
                                                new_node->conflict_num);
                t2 = std::chrono::high_resolution_clock::now();

                duration = (double)(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count())/1000000.0;
                LowLevelTime += duration;
                std::cout << "Replanned agent " << agent_id << " with LA*, took " << duration << " sec\n";

                if (!success || new_node->sop_waypoints[agent_id].empty())
                    continue;
                
                new_node->cur_ids.clear();
                for (int j = 0; j < AGENT_NUM; ++j)
                    new_node->cur_ids.push_back(0);
                open_list.insert(new_node);
            }

        }else{  //  edge confliction
            for(int i = 0; i < 2; i++){
                int agent_id = i == 0 ? std::get<0>(cft) : std::get<1>(cft);
                auto new_node = std::make_shared<HighLevelNode>(*node);
                new_node->sop_waypoints.at(agent_id).clear();
                new_node->sop_apex.at(agent_id).clear();
                new_node->sop_cost.at(agent_id).clear();


                conflict_resolver.AddConstraint(new_node->edge_constraints, agent_id, std::get<2>(cft).at(i), std::get<2>(cft).at(1-i), std::get<3>(cft));

                // if(MS == MergingStrategy::CONFLICT_BASED){
                //     calculate_CAT(node, vertex_cat, edge_cat, agent_id);
                // }

                RunLAstar lex_search(GRAPH_SIZE, graph, heuristics[agent_id], start_goal[agent_id].first, start_goal[agent_id].second,
                     logger, TIME_LIMIT, new_node->vertex_constraints[agent_id], new_node->edge_constraints[agent_id],
                     TURN_DIM, TURN_COST);

                t1 = std::chrono::high_resolution_clock::now();
                bool success = lex_search.search(new_node->sop_waypoints[agent_id],
                                                new_node->sop_apex[agent_id],
                                                new_node->sop_cost[agent_id],
                                                new_node->conflict_num);
                t2 = std::chrono::high_resolution_clock::now();

                duration = (double)(std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count())/1000000.0;
                LowLevelTime += duration;
                std::cout << "Replanned agent " << agent_id << " with LA*, took " << duration << " sec\n";

                if (!success || new_node->sop_waypoints[agent_id].empty())
                    continue;
                
                new_node->cur_ids.clear();
                for (int j = 0; j < AGENT_NUM; ++j)
                    new_node->cur_ids.push_back(0);
                open_list.insert(new_node);
            }
        }
    }
    precise_end_time = std::chrono::high_resolution_clock::now();
    TotalTime = (double)(std::chrono::duration_cast<std::chrono::microseconds>(precise_end_time - precise_start_time).count())/1000000.0;

    if(is_success){
        output << "SUCCESS" << std::endl;
    }else{
        output << "FAIL" << std::endl;
    }


    return std::make_tuple(HLMergingTime, LowLevelTime, TotalTime, ConflictSolvingNum, static_cast<int>(hsolution_costs.size()), is_success);
}