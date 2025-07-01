#include "HighLevel/HighLevelSolver.h"

HighLevelSolver::HighLevelSolver(size_t graph_size, int agent_num, Algorithm algorithm, bool if_eager, int dim, int turn_dim, int turn_cost, int time_limit,
                                 double eps1, double eps2, double eps3, double eps4)
{
    this->GRAPH_SIZE = graph_size;
    this->AGENT_NUM = agent_num;
    this->ALGORITHM = algorithm;
    this->EAGER = if_eager;
    this->TURN_COST = turn_cost;
    this->TIME_LIMIT = time_limit;
    this->DIM = dim;
    this->TURN_DIM = turn_dim;

    if(ALGORITHM == Algorithm::BBMOCBS_EPS){
        if(this->DIM == 2){
            this->LSOLVER = LSolver::BOA;
        }else{
            this->LSOLVER = LSolver::NAMOA;
        }
    }else if(ALGORITHM == Algorithm::BBMOCBS_PEX){
        this->LSOLVER = LSolver::APEX;
    }else if(ALGORITHM == Algorithm::BBMOCBS_K){
        this->LSOLVER = LSolver::APEX;
    }else if(ALGORITHM == Algorithm::LCBS){
        this->LSOLVER = LSolver::APEX;
        this->eps1 = eps1;
        this->eps2 = eps2;
        this->eps3 = eps3;
        this->eps4 = eps4;
    }else{
        assert(0);
    }
}