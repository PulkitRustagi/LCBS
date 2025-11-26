#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

// -----------------------------------------------------------
// Configuration (aligned with run_optimality_experiments.cpp)
// -----------------------------------------------------------

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

// Same time limits as in your updated driver
static const std::vector<int> TIME_LIMITS = {120, 60, 30, 10, 5, 1};

// Algorithm IDs exactly as written into the "algorithm" column
// in opt_summary_T*.csv by your current driver
static const std::vector<std::string> ALGORITHM_IDS = {
    "LCBS",
    "BBMOCBS_k1",
    "BBMOCBS_k5",
    "BBMOCBS_k10",
    "BBMOCBS_pex",
    "BBMOCBS_eps"
};

// -----------------------------------------------------------
// Helpers
// -----------------------------------------------------------

struct Stat {
    int total   = 0;  // number of rows (scenarios considered)
    int matches = 0;  // rows with match == 1
};

struct LcbsScenario {
    bool seen   = false;  // we saw this (map, T, scenario) at all
    bool solved = false;  // LCBS had a cost vector
};

static inline std::string trim(const std::string& s) {
    std::size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    std::size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

// -----------------------------------------------------------
// Main
// -----------------------------------------------------------

int main() {
    // stats[algorithm][map][time_limit] -> Stat  (for non-LCBS methods)
    using TimeMap   = std::map<int, Stat>;
    using MapMap    = std::map<std::string, TimeMap>;
    std::map<std::string, MapMap> stats;

    // lcbs_stats[map][time_limit][scenario] -> LcbsScenario
    using ScenarioMap = std::map<int, LcbsScenario>;
    using LcbsTimeMap = std::map<int, ScenarioMap>;
    using LcbsMapMap  = std::map<std::string, LcbsTimeMap>;
    LcbsMapMap lcbs_stats;

    const std::string base_dir = "../data/OPTIMALITY_CHECK_2D";

    // -------------------------------------------------------
    // 1) Parse all opt_summary_T*.csv files and accumulate
    // -------------------------------------------------------
    for (const auto& map_name : MAP_NAMES) {
        for (int T : TIME_LIMITS) {
            std::string summary_path =
                base_dir + "/" + map_name + "/opt_summary_T" + std::to_string(T) + ".csv";

            std::ifstream in(summary_path);
            if (!in.is_open()) {
                std::cerr << "Warning: could not open " << summary_path << "\n";
                continue;
            }

            std::string line;
            // Skip header
            if (!std::getline(in, line)) {
                continue;
            }

            while (std::getline(in, line)) {
                if (line.empty()) continue;

                std::stringstream ss(line);
                std::string cell;
                std::vector<std::string> tokens;

                while (std::getline(ss, cell, ',')) {
                    tokens.push_back(trim(cell));
                }

                // Expected columns:
                // 0: map
                // 1: scenario
                // 2: time_limit_sec
                // 3: algorithm
                // 4: agents
                // 5: lcbs_c1
                // 6: lcbs_c2
                // 7: alg_c1
                // 8: alg_c2
                // 9: match
                if (tokens.size() < 10) {
                    std::cerr << "Warning: malformed line in " << summary_path
                              << " -> " << line << "\n";
                    continue;
                }

                const std::string& map_in  = tokens[0];
                int scenario_id            = 0;
                int time_from_file         = 0;
                const std::string& alg_id  = tokens[3];

                try {
                    scenario_id    = std::stoi(tokens[1]);
                    time_from_file = std::stoi(tokens[2]);
                } catch (...) {
                    std::cerr << "Warning: bad scenario/time in " << summary_path
                              << " -> " << line << "\n";
                    continue;
                }

                // LCBS success: lcbs_c1 non-empty -> LCBS has a cost vector
                bool lcbs_has_cost = !tokens[5].empty();

                // Update LCBS per-scenario data
                {
                    LcbsScenario& entry = lcbs_stats[map_in][time_from_file][scenario_id];
                    entry.seen = true;
                    if (lcbs_has_cost) entry.solved = true;
                }

                // For other algorithms, use match column
                int match_val = 0;
                try {
                    match_val = std::stoi(tokens[9]);
                } catch (...) {
                    match_val = 0;
                }

                // The summary does not contain LCBS rows (only baselines).
                // We still skip any accidental LCBS rows, just in case.
                if (alg_id == "LCBS") {
                    continue;
                }

                Stat& st = stats[alg_id][map_in][time_from_file];
                st.total += 1;
                if (match_val != 0) {
                    st.matches += 1;
                }
            }
        }
    }

    // -------------------------------------------------------
    // 2) Write one CSV per algorithm with "optimality percentage"
    // -------------------------------------------------------
    for (const auto& alg_id : ALGORITHM_IDS) {
        std::string out_path = base_dir + "/" + alg_id + ".csv";
        std::ofstream out(out_path);
        if (!out.is_open()) {
            std::cerr << "Error: could not open output file " << out_path << "\n";
            continue;
        }

        out << "map,time_limit_sec,optimality_percentage\n";

        for (const auto& map_name : MAP_NAMES) {
            for (int T : TIME_LIMITS) {
                double percentage = 0.0;

                if (alg_id == "LCBS") {
                    // For LCBS: fraction of scenarios (out of those seen) where LCBS solved.
                    auto map_it = lcbs_stats.find(map_name);
                    if (map_it != lcbs_stats.end()) {
                        auto time_it = map_it->second.find(T);
                        if (time_it != map_it->second.end()) {
                            int total_scenarios  = 0;
                            int solved_scenarios = 0;
                            for (const auto& kv : time_it->second) {
                                const LcbsScenario& s = kv.second;
                                if (!s.seen) continue;
                                total_scenarios += 1;
                                if (s.solved) solved_scenarios += 1;
                            }
                            if (total_scenarios > 0) {
                                percentage = 100.0 *
                                             static_cast<double>(solved_scenarios) /
                                             static_cast<double>(total_scenarios);
                            }
                        }
                    }
                } else {
                    // For other methods: fraction of scenarios where match == 1
                    auto alg_it = stats.find(alg_id);
                    if (alg_it != stats.end()) {
                        auto map_it = alg_it->second.find(map_name);
                        if (map_it != alg_it->second.end()) {
                            auto time_it = map_it->second.find(T);
                            if (time_it != map_it->second.end() &&
                                time_it->second.total > 0) {
                                const Stat& st = time_it->second;
                                percentage = 100.0 *
                                             static_cast<double>(st.matches) /
                                             static_cast<double>(st.total);
                            }
                        }
                    }
                }

                out << map_name << ","
                    << T << ","
                    << std::fixed << std::setprecision(2) << percentage
                    << "\n";
            }
        }

        std::cout << "Wrote " << out_path << "\n";
    }

    return 0;
}
