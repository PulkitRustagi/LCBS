#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

std::string time_lim = "300"; // seconds
std::string map_name = "empty-48-48";

std::string map_file = "../maps/"+map_name+"/"+map_name+".map";
std::string cost1 = "../maps/"+map_name+"/random-1.cost";
std::string cost2 = "../maps/"+map_name+"/random-2.cost";
std::string cost3 = "../maps/"+map_name+"/random-3.cost";
std::string cost4 = "../maps/"+map_name+"/random-4.cost";
std::string cost5 = "../maps/"+map_name+"/random-5.cost";
std::string cost6 = "../maps/"+map_name+"/random-6.cost";
std::string cost7 = "../maps/"+map_name+"/random-7.cost";
std::string cost8 = "../maps/"+map_name+"/random-8.cost";
std::string cost9 = "../maps/"+map_name+"/random-9.cost";
std::string cost10 = "../maps/"+map_name+"/random-10.cost";
std::string scen_dir = "../maps/"+map_name+"/scen-random";
std::string output_file = "../data/"+map_name+"_output_log_"+time_lim+"sec.txt";
std::string binary = "./bin/bbmocbs_approx";

std::string make_command(const std::string& scen_file, int agent_num, const std::string& algorithm) {
    std::ostringstream cmd;
    cmd << binary
        << " -m " << map_file
        << " -s " << scen_dir << "/" << scen_file
        << " -e 0.03"
        << " --c1 " << cost1
        << " --c2 " << cost2
        << " --c3 " << cost3
        << " --c4 " << cost4
        << " --c5 " << cost5
        << " --c6 " << cost6
        << " --c7 " << cost7
        << " --c8 " << cost8
        << " --c9 " << cost9
        << " --c10 " << cost10
        << " --CB true"
        << " --eager true"
        << " -t "+time_lim
        << " -o " << output_file
        << " -a " << algorithm
        << " -n " << agent_num;
    return cmd.str();
}

std::string clean_algorithm_name(const std::string& algo) {
    if (algo == "LCBS -k 1 -d 5") return "LCBS 5D";
    if (algo == "LCBS -k 1 -d 6") return "LCBS 6D";
    if (algo == "LCBS -k 1 -d 7") return "LCBS 7D";
    if (algo == "LCBS -k 1 -d 8") return "LCBS 8D";
    if (algo == "LCBS -k 1 -d 9") return "LCBS 9D";
    if (algo == "LCBS -k 1 -d 10") return "LCBS 10D";

    std::ostringstream cleaned;
    for (size_t i = 0; i < algo.size(); ++i) {
        if (algo[i] == '-') {
            cleaned << '_';
        } else if (algo[i] != ' ') {
            cleaned << algo[i];
        }
    }
    std::string s = cleaned.str();
    size_t pos;
    while ((pos = s.find("_k")) != std::string::npos && s[pos + 2] == 'k') {
        s.replace(pos, 3, "_");
    }
    return s;
}

int count_successes(const std::string& output_file) {
    std::ifstream infile(output_file);
    std::string line;
    int count = 0;
    while (std::getline(infile, line)) {
        if (line.find("SUCCESS") != std::string::npos) {
            count++;
        }
    }
    return count;
}

void write_run_outcomes(const std::string& output_file, const std::string& algo_label, int agents) {
    std::ifstream infile(output_file);
    std::ofstream outfile;

    // Clean up algorithm name for file-safe naming
    std::string clean_algo = clean_algorithm_name(algo_label);
    std::replace(clean_algo.begin(), clean_algo.end(), ' ', '_');
    std::replace(clean_algo.begin(), clean_algo.end(), '-', '_');


    std::ofstream out_name("../data/performance_stats/"+map_name+"/"+clean_algo+"_"+std::to_string(agents)+"_t"+time_lim+".stats", std::ios::app);
    out_name.seekp(0, std::ios::end);
    // out_name << "../data/performance_stats_" << clean_algo << "_" << agents << ".stats";
    outfile.open("../data/performance_stats/"+map_name+"/"+clean_algo+"_"+std::to_string(agents)+"_t"+time_lim+".stats", std::ios::app);

    // Read the entire file line-by-line
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(infile, line)) {
        lines.push_back(line);
    }

    // Parse blocks delimited by "------"
    std::vector<std::string> results;
    std::string current_block;
    for (const auto& l : lines) {
        if (l.find("------") != std::string::npos) {
            results.push_back(current_block);
            current_block.clear();
        } else {
            current_block += l + "\n";
        }
    }

    // Optional: include last block if file doesn't end with "------"
    if (!current_block.empty()) {
        results.push_back(current_block);
    }

    // Write outcomes indexed by scenario number
    for (size_t i = 0; i < results.size(); ++i) {
        const std::string& block = results[i];
        std::string outcome = "UNKNOWN";
        std::istringstream block_stream(block);
        std::string l;
        while (std::getline(block_stream, l)) {
            if (l.find("SUCCESS") != std::string::npos) {
                outcome = "SUCCESS";
                break;
            } else if (l.find("FAIL") != std::string::npos) {
                outcome = "FAIL";
                break;
            }
        }
        // std::cout << "Scenario " << (i + 1) << ": " << outcome << std::endl;
        outfile << (i + 1) << " - " << outcome << "\n";
    }

    outfile.close();
}


int main() {
    std::vector<std::string> algorithms = {"LCBS -k 1 -d 5", "LCBS -k 1 -d 6", "LCBS -k 1 -d 7", "LCBS -k 1 -d 8", "LCBS -k 1 -d 9", "LCBS -k 1 -d 10"};
    std::vector<int> agent_counts = {5};//, 10, 15, 20, 25, 30, 35};
    int total_runs = 25;

    std::ofstream summary("../data/results_"+map_name+"_"+time_lim+"sec.txt", std::ios::app);
    summary.seekp(0, std::ios::end);
    summary << "Algorithm,Agents,SuccessCount,Total,SuccessRate\n\n";

    for (const auto& algorithm : algorithms) {
        for (int agents : agent_counts) {
            std::cout << "\033[1;33m" << "----- Number of agents = " << agents << "\033[0m" << std::endl;
            for (int i = 1; i <= total_runs; ++i) {
                std::cout << "\033[1;34m" << "["+time_lim+" sec]Running scenario " << i << " for algorithm: " << algorithm << "\033[0m" << std::endl;
                std::ostringstream scen_name;
                scen_name << ""+map_name+"-random-" << i << ".scen";
                std::string command = make_command(scen_name.str(), agents, algorithm);
                std::system(command.c_str());
            }
            
            write_run_outcomes(output_file, algorithm, agents);

            int success_count = count_successes(output_file);
            double rate = success_count / static_cast<double>(total_runs);

            summary << algorithm << "," << agents << "," << success_count
                    << "," << total_runs << "," << rate << "\n";
            std::cout << "Algorithm: " << algorithm
                      << " | Agents: " << agents
                      << " | Success: " << success_count << "/" << total_runs
                      << " (" << rate * 100 << "%)\n";

            std::ofstream clear_file(output_file, std::ios::trunc);
            clear_file.close();
        }
        summary << std::endl;  // Add a newline after each algorithm's results
    }

    summary.close();
    return 0;
}