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
    // stats[algorithm][map][time_limit] -> Stat
    using TimeMap = std::map<int, Stat>;
    using MapMap  = std::map<std::string, TimeMap>;
    std::map<std::string, MapMap> stats;

    const std::string base_dir = "../data/OPTIMALITY_CHECK";

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

                const std::string& alg_id = tokens[3];

                int match_val = 0;
                try {
                    match_val = std::stoi(tokens[9]);
                } catch (...) {
                    match_val = 0;
                }

                Stat& st = stats[alg_id][map_name][T];
                st.total += 1;
                if (match_val != 0) {
                    st.matches += 1;
                }
            }
        }
    }

    // -------------------------------------------------------
    // 2) Write one CSV per algorithm with percentages
    // -------------------------------------------------------
    for (const auto& alg_id : ALGORITHM_IDS) {
        std::string out_path = base_dir + "/" + alg_id + ".csv";
        std::ofstream out(out_path);
        if (!out.is_open()) {
            std::cerr << "Error: could not open output file " << out_path << "\n";
            continue;
        }

        out << "map,time_limit_sec,percentage_match\n";

        for (const auto& map_name : MAP_NAMES) {
            for (int T : TIME_LIMITS) {
                double percentage = 0.0;

                if (alg_id == "LCBS") {
                    // By design choice: LCBS is lexicographically optimal
                    // whenever it returns a solution; reported as 100%.
                    percentage = 100.0;
                } else {
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
