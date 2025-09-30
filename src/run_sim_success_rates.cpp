#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <algorithm>

std::string time_lim = "120"; // seconds ==> 2 minutes
std::string binary = "./bin/lcbs_sim";
std::vector<std::string> map_names = {"warehouse-10-20-10-2-1",
                                    //   "empty-32-32", 
                                    //   "maze-32-32-2", 
                                    //   "room-32-32-4",
                                    //   "random-32-32-20", 
                                    //   "room-64-64-8", 
                                    //   "random-64-64-10",
                                    //   "random-64-64-20", 
                                    //   "maze-32-32-4", 
                                    //   "empty-48-48"
                                      };

std::vector<std::string> algorithms = {"LCBS", "BBMOCBS-k -k 1", "BBMOCBS-k -k 5", "BBMOCBS-k -k 10", "BBMOCBS-pex", "BBMOCBS-eps"};
std::vector<int> agent_counts = {5, 10, 15, 20, 25, 30, 35};
int total_runs = 25;

std::string make_command(const std::string& map_name, const std::string& scen_file, int agent_num, const std::string& algorithm) {
    std::ostringstream cmd;
    std::string map_file = "../maps/" + map_name + "/" + map_name + ".map";
    std::string scen_dir = "../maps/" + map_name + "/scen-random";
    std::string output_file = "../data/LOG_CONTOUR_" + map_name + "_output_log_" + time_lim + "sec.txt";
    cmd << binary
        << " -m " << map_file
        << " -s " << scen_dir + "/" + scen_file
        << " -e 0.03"
        << " -d 3" // CHANGE this for number of objectives - currently set to 2, can go up to 10
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
        << " -o " << output_file
        << " -a " << algorithm
        << " -n " << agent_num;
    return cmd.str();
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

int main() {
    for (const auto& map_name : map_names) {
        std::ofstream summary("../data/dimensional_trends/" + map_name + "/summary.txt", std::ios::app);
        summary << "Algorithm,Agents,SuccessRate\n\n";

        std::string output_file = "../data/LOG_CONTOUR_" + map_name + "_output_log_" + time_lim + "sec.txt";

        for (const auto& algorithm : algorithms) {
            for (int agents : agent_counts) {
                std::cout << "\033[1;33m----- Map: " << map_name << ", Agents: " << agents << "\033[0m" << std::endl;

                for (int i = 1; i <= total_runs; ++i) {
                    std::ostringstream scen_name;
                    scen_name << map_name << "-random-" << i << ".scen";
                    std::string command = make_command(map_name, scen_name.str(), agents, algorithm);
                    std::cout << "\033[1;34m[" << time_lim << " sec] Running scenario " << i << " for algorithm: " << algorithm << "\033[0m" << std::endl;
                    std::system(command.c_str());
                }

                int success_count = count_successes(output_file);
                double rate = success_count / static_cast<double>(total_runs);
                summary << algorithm << "," << agents << "," << rate << "\n";

                std::cout << "===> Algorithm: " << algorithm
                          << " | Agents: " << agents
                          << " | Success: " << success_count << "/" << total_runs
                          << " (" << rate * 100 << "%)\n\n";

                std::ofstream clear_file(output_file, std::ios::trunc);
                clear_file.close();
            }
            summary << std::endl;
        }
        summary.close();
    }
    return 0;
}
