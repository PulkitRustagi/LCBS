#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

std::string time_lim = "300"; // seconds ==> 5 minutes
std::vector<std::string> map_names = {"empty-32-32", 
                                      "maze-32-32-2", 
                                      "room-32-32-4",
                                      "random-32-32-20", 
                                      "room-64-64-8", 
                                      "random-64-64-10",
                                      "random-64-64-20", 
                                      "maze-32-32-4", 
                                      "empty-48-48"};

std::string binary = "./bin/lcbs_sim";

std::string make_command(const std::string& map_name, const std::string& scen_file, int agent_num, const std::string& algorithm, const std::string& d) {
    std::ostringstream cmd;
    cmd << binary
        << " -m ../maps/" << map_name << "/" << map_name << ".map"
        << " -s ../maps/" << map_name << "/scen-random/" << scen_file
        << " -e 0.03"
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
        << " -t " << time_lim
        << " -o ../data/LOG_CONTOUR_" << map_name << "_output_log_" << time_lim << "sec.txt"
        << " -a " << algorithm
        << " " << d
        << " -n " << agent_num;
    return cmd.str();
}

std::string clean_algorithm_name(const std::string& algo, const std::string& d) {
    std::ostringstream cleaned;
    for (char c : algo) {
        if (c == '-') cleaned << '_';
        else if (c != ' ') cleaned << c;
    }
    std::string s = cleaned.str();
    s += "_" + d.substr(3); // append dimension number (after -d)
    return s;
}

int count_successes(const std::string& output_file) {
    std::ifstream infile(output_file);
    std::string line;
    int count = 0;
    while (std::getline(infile, line)) {
        if (line.find("SUCCESS") != std::string::npos) count++;
    }
    return count;
}

void write_run_outcomes(const std::string& map_name, const std::string& output_file, const std::string& algo_label, int agents, const std::string& d) {
    std::ifstream infile(output_file);
    std::ofstream outfile;

    std::string clean_algo = clean_algorithm_name(algo_label, d);
    std::string out_path = "../data/performance_stats/" + map_name + "/" + clean_algo + "_" + std::to_string(agents) + "_t" + time_lim + ".stats";
    outfile.open(out_path, std::ios::app);

    std::string line, block;
    std::vector<std::string> blocks;
    while (std::getline(infile, line)) {
        if (line.find("------") != std::string::npos) {
            blocks.push_back(block);
            block.clear();
        } else {
            block += line + "\n";
        }
    }
    if (!block.empty()) blocks.push_back(block);

    for (size_t i = 0; i < blocks.size(); ++i) {
        std::istringstream ss(blocks[i]);
        std::string l, outcome = "UNKNOWN";
        while (std::getline(ss, l)) {
            if (l.find("SUCCESS") != std::string::npos) { outcome = "SUCCESS"; break; }
            else if (l.find("FAIL") != std::string::npos) { outcome = "FAIL"; break; }
        }
        outfile << (i + 1) << " - " << outcome << "\n";
    }
    outfile.close();
}

int main() {
    std::vector<std::string> algorithms = {"LCBS", "BBMOCBS-k -k 1", "BBMOCBS-k -k 5", "BBMOCBS-k -k 10", "BBMOCBS-pex", "BBMOCBS-eps"};
    std::vector<std::string> dimensions = {"-d 2", "-d 3", "-d 4", "-d 5", "-d 6", "-d 7", "-d 8", "-d 9", "-d 10"}; // dimensions 2 to 10
    std::vector<int> agent_counts = {5, 10, 15, 20, 25, 30, 35};
    int total_runs = 10;

    for (const auto& map_name : map_names) {
        std::string summary_file = "../data/dimensional_trends/" + map_name + "/LCBS_new.txt";
        std::ofstream summary(summary_file, std::ios::app);
        summary.seekp(0, std::ios::end);
        summary << "Objectives,Agents,SuccessRate\n\n";

        for (const auto& algorithm : algorithms) {
            for (const auto& d : dimensions) {
                for (int agents : agent_counts) {
                    std::cout << "\033[1;33m----- Map: " << map_name << " | Agents = " << agents << "\033[0m" << std::endl;
                    for (int i = 1; i <= total_runs; ++i) {
                        std::ostringstream scen_name;
                        scen_name << map_name << "-random-" << i << ".scen";
                        std::string cmd = make_command(map_name, scen_name.str(), agents, algorithm, d);
                        std::cout << "\033[1;34m[" << time_lim << " sec] Running scenario " << i << " for algorithm: " << algorithm << " " << d << "\033[0m" << std::endl;
                        std::system(cmd.c_str());
                    }

                    std::string output_file = "../data/LOG_CONTOUR_" + map_name + "_output_log_" + time_lim + "sec.txt";
                    write_run_outcomes(map_name, output_file, algorithm, agents, d);

                    int success_count = count_successes(output_file);
                    double rate = success_count / static_cast<double>(total_runs);
                    summary << d.substr(3) << "," << agents << "," << rate << "\n";

                    std::cout << "\n===> Algorithm: " << algorithm
                              << " " << d << " | Agents: " << agents
                              << " | Success: " << success_count << "/" << total_runs
                              << " (" << rate * 100 << "%)\n\n";

                    std::ofstream clear_file(output_file, std::ios::trunc);
                    clear_file.close();
                }
                summary << std::endl;
            }
        }
        summary.close();
    }
    return 0;
}
