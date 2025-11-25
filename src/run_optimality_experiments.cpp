#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <filesystem>

// namespace fs = std::filesystem;

// ---------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------

static const std::string BINARY = "./bin/lcbs_sim";

// 10 benchmark maps (same as in your existing scripts)
static const std::vector<std::string> MAP_NAMES = {
    "room-32-32-4",
    "empty-32-32",
    "maze-32-32-2",
    "warehouse-10-20-10-2-1",
    "random-32-32-20",
    "room-64-64-8",
    "random-64-64-10",
    "random-64-64-20",
    "maze-32-32-4",
    "empty-48-48"
};

struct AlgorithmConfig {
    std::string id;   // short id used in filenames
    std::string arg;  // argument string passed to -a
};

// LCBS + Pareto baselines
static const std::vector<AlgorithmConfig> ALGORITHMS = {
    {"LCBS",          "LCBS"},
    {"BBMOCBS_k1",    "BBMOCBS-k -k 1"},
    {"BBMOCBS_k5",    "BBMOCBS-k -k 5"},
    {"BBMOCBS_k10",   "BBMOCBS-k -k 10"},
    {"BBMOCBS_pex",   "BBMOCBS-pex"},
    {"BBMOCBS_eps",   "BBMOCBS-eps"}
};

static const std::vector<int> TIME_LIMITS = {120, 60, 30, 10};
static const int TOTAL_RUNS = 25;   // 25 scenarios per map
static const int NUM_AGENTS = 5;
static const int NUM_OBJECTIVES = 2;  // d = 2

// ---------------------------------------------------------------------
// Utilities: lexicographic comparison and cost parsing
// ---------------------------------------------------------------------

bool lex_less(const std::vector<long long>& a,
              const std::vector<long long>& b) {
    const std::size_t d = std::min(a.size(), b.size());
    for (std::size_t i = 0; i < d; ++i) {
        if (a[i] < b[i]) return true;
        if (a[i] > b[i]) return false;
    }
    return a.size() < b.size();
}

bool lex_equal(const std::vector<long long>& a,
               const std::vector<long long>& b) {
    if (a.size() != b.size()) return false;
    for (std::size_t i = 0; i < a.size(); ++i) {
        if (a[i] != b[i]) return false;
    }
    return true;
}

// Try to extract a "{c1, c2, ...}" style vector from a line.
// Only the first NUM_OBJECTIVES entries are kept.
std::vector<long long> parse_cost_vector_from_line(const std::string& line,
                                                   int dims) {
    static const std::regex brace_pattern("\\{([^}]*)\\}");
    std::smatch match;
    if (!std::regex_search(line, match, brace_pattern)) {
        return {};
    }

    std::string inside = match[1].str();
    std::istringstream iss(inside);
    std::vector<long long> vals;
    while (iss) {
        long double x;
        if (!(iss >> x)) break;
        vals.push_back(static_cast<long long>(std::llround(x)));
        // skip commas or other separators
        if (iss.peek() == ',' || iss.peek() == ';') {
            iss.get();
        }
    }

    if (static_cast<int>(vals.size()) < dims) {
        return {};
    }
    vals.resize(dims);
    return vals;
}

// ---------------------------------------------------------------------
// RunResult: parsed result of one (map, scen, alg, time_limit) log
// ---------------------------------------------------------------------

struct RunResult {
    bool solved = false;      // saw "SUCCESS"
    bool has_cost = false;    // saw at least one cost vector
    std::vector<long long> best_cost;  // lexicographically best in log
};

RunResult parse_log_file(const std::string& log_path, int dims) {
    RunResult result;

    std::ifstream infile(log_path);
    if (!infile.is_open()) {
        std::cerr << "Warning: could not open log file " << log_path << "\n";
        return result;
    }

    std::string line;
    while (std::getline(infile, line)) {
        if (line.find("SUCCESS") != std::string::npos) {
            result.solved = true;
        }

        auto v = parse_cost_vector_from_line(line, dims);
        if (!v.empty()) {
            if (!result.has_cost || lex_less(v, result.best_cost)) {
                result.best_cost = v;
                result.has_cost = true;
            }
        }
    }

    return result;
}

// ---------------------------------------------------------------------
// Command construction
// ---------------------------------------------------------------------

std::string make_command(const std::string& map_name,
                         const std::string& scen_file,
                         int agent_num,
                         const AlgorithmConfig& alg,
                         int time_limit_sec,
                         const std::string& output_file) {
    std::ostringstream cmd;
    std::string map_file = "../maps/" + map_name + "/" + map_name + ".map";
    std::string scen_dir = "../maps/" + map_name + "/scen-random";

    cmd << BINARY
        << " -m " << map_file
        << " -s " << scen_dir + "/" + scen_file
        << " -e 0.03"
        << " -d " << NUM_OBJECTIVES
        << " --c1 ../maps/" << map_name << "/random-1.cost"
        << " --c2 ../maps/" << map_name << "/random-2.cost"
        << " --c3 ../maps/" << map_name << "/random-3.cost"
        << " --c4 ../maps/" << map_name << "/random-4.cost"
        << " --c5 ../maps/" << map_name << "/random-5.cost"
        << " --c6 ../maps/" << map_name << "/random-6.cost"
        << " --c7 ../maps/" << map_name << "/random-7.cost"
        << " --c8 ../maps/" << map_name << "/random-8.cost"
        << " --c9 ../maps/" << map_name << "/random-9.cost"
        << " --c10 ../maps/" << map_name << "/random-10.cost"
        << " --CB true"
        << " --eager true"
        << " -t " << time_limit_sec
        << " -o " << output_file
        << " -a " << alg.arg
        << " -n " << agent_num;

    return cmd.str();
}

// ---------------------------------------------------------------------
// Main experiment driver
// ---------------------------------------------------------------------

int main() {
    // For each map, each time limit, run all algorithms on all 25 scenarios.
    // Then compare each baseline to LCBS on a per-instance basis.

    for (const auto& map_name : MAP_NAMES) {
        std::cout << "==== Map: " << map_name << " ====\n";

        for (int T : TIME_LIMITS) {
            std::cout << "  >> Time limit: " << T << " seconds\n";

            // results[alg_index][scenario_index]
            std::vector<std::vector<RunResult>> results(
                ALGORITHMS.size(),
                std::vector<RunResult>(TOTAL_RUNS + 1) // 1-based for scenarios
            );

            // Ensure output directory exists
            std::string base_dir = "../data/OPTIMALITY_CHECK/" + map_name;
            // fs::create_directories(base_dir);

            // Run all algorithms for all scenarios at this time limit
            for (int scen_idx = 1; scen_idx <= TOTAL_RUNS; ++scen_idx) {
                std::ostringstream scen_name;
                scen_name << map_name << "-random-" << scen_idx << ".scen";

                for (std::size_t a = 0; a < ALGORITHMS.size(); ++a) {
                    const auto& alg = ALGORITHMS[a];

                    std::ostringstream log_name;
                    log_name << base_dir
                             << "/"
                             << map_name
                             << "_alg_" << alg.id
                             << "_T" << T
                             << "_S" << scen_idx
                             << ".log";
                    std::string log_path = log_name.str();

                    std::string command =
                        make_command(map_name,
                                     scen_name.str(),
                                     NUM_AGENTS,
                                     alg,
                                     T,
                                     log_path);

                    std::cout << "    [T=" << T << "s] Map=" << map_name
                              << " scen=" << scen_idx
                              << " alg=" << alg.id
                              << " -> running...\n";

                    int ret = std::system(command.c_str());
                    if (ret != 0) {
                        std::cerr << "      Warning: command returned "
                                  << ret << " for " << log_path << "\n";
                    }

                    // Parse the log to get lexicographically best cost
                    results[a][scen_idx] = parse_log_file(log_path, NUM_OBJECTIVES);
                }
            }

            // Build CSV summary for this (map, T) over all algorithms
            std::ostringstream summary_name;
            summary_name << base_dir
                         << "/opt_summary_T" << T << ".csv";
            std::ofstream summary(summary_name.str());
            if (!summary.is_open()) {
                std::cerr << "Error: could not open summary file "
                          << summary_name.str() << "\n";
                continue;
            }

            summary << "map,scenario,time_limit_sec,algorithm,agents,"
                    << "lcbs_c1,lcbs_c2,alg_c1,alg_c2,match\n";

            // LCBS is assumed to be ALGORITHMS[0]
            const std::size_t LCBS_INDEX = 0;

            // Aggregate counters for quick console summary
            struct Counters {
                int comparable = 0; // instances where LCBS and alg both solved + have costs
                int matches = 0;
            };
            std::vector<Counters> counters(ALGORITHMS.size());

            for (int scen_idx = 1; scen_idx <= TOTAL_RUNS; ++scen_idx) {
                const RunResult& lcbs_res = results[LCBS_INDEX][scen_idx];

                for (std::size_t a = 1; a < ALGORITHMS.size(); ++a) {
                    const auto& alg = ALGORITHMS[a];
                    const RunResult& alg_res = results[a][scen_idx];

                    bool match = false;
                    bool comparable =
                        lcbs_res.solved && lcbs_res.has_cost &&
                        alg_res.solved && alg_res.has_cost;

                    if (comparable) {
                        match = lex_equal(lcbs_res.best_cost, alg_res.best_cost);
                        counters[a].comparable += 1;
                        if (match) {
                            counters[a].matches += 1;
                        }
                    }

                    auto print_cost = [](const RunResult& r,
                                         std::size_t idx) -> std::string {
                        if (!r.has_cost || r.best_cost.size() <= idx) {
                            return "";
                        }
                        return std::to_string(r.best_cost[idx]);
                    };

                    summary << map_name << ","
                            << scen_idx << ","
                            << T << ","
                            << alg.id << ","
                            << NUM_AGENTS << ","
                            << print_cost(lcbs_res, 0) << ","
                            << print_cost(lcbs_res, 1) << ","
                            << print_cost(alg_res, 0) << ","
                            << print_cost(alg_res, 1) << ","
                            << (match ? 1 : 0)
                            << "\n";
                }
            }

            summary.close();

            // Console summary per algorithm at this time limit
            std::cout << "  Summary for map=" << map_name
                      << ", T=" << T << "s:\n";
            for (std::size_t a = 1; a < ALGORITHMS.size(); ++a) {
                const auto& alg = ALGORITHMS[a];
                const auto& c = counters[a];
                std::cout << "    " << alg.id
                          << " | comparable instances: " << c.comparable
                          << " | matches LCBS: " << c.matches
                          << "\n";
            }
            std::cout << "\n";
        }
    }

    return 0;
}
