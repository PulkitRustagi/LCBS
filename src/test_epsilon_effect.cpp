#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstdlib>

std::string map_file = "../maps/random-32-32-20.map";
std::string cost1 = "../maps/random-3.cost";
std::string cost2 = "../maps/random-2.cost";
std::string cost3 = "../maps/random-1.cost";
std::string scen_dir = "../maps/scen-random";
std::string output_file = "../vary_slack_temp.txt";
std::string binary = "./bin/bbmocbs_approx";

std::string make_command(const std::string& scen_file, int agent_num, const std::string& algorithm) {
    std::ostringstream cmd;
    cmd << binary
        << " -m " << map_file
        << " -d 3"
        << " -s " << scen_dir << "/" << scen_file
        << " -e 0.03"
        << " --c1 " << cost1
        << " --c2 " << cost2
        << " --c3 " << cost3
        << " --CB true"
        << " --eager true"
        << " -t 120"
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
    std::vector<std::string> algorithms = { "LCBS -k 1 --eps1 5.0 --eps2 5.0 --eps3 5.0",
                                            "LCBS -k 1 --eps1 1.0 --eps2 1.0 --eps3 1.0",
                                            "LCBS -k 1 --eps1 0.5 --eps2 0.5 --eps3 0.5",
                                            "LCBS -k 1 --eps1 0.1 --eps2 0.1 --eps3 0.1",
                                            "LCBS -k 1 --eps1 0.05 --eps2 0.05 --eps3 0.05",
                                            "LCBS -k 1 --eps1 0.03 --eps2 0.03 --eps3 0.03",
                                            "LCBS -k 1 --eps1 0 --eps2 0 --eps3 0"};

    std::vector<int> agent_counts = {5, 10, 15, 20, 25, 30, 35};
    int total_runs = 25;

    std::ofstream summary("../sim_eps_data.txt", std::ios::app);
    summary.seekp(0, std::ios::end);
    summary << "Algorithm,Agents,SuccessCount,Total,SuccessRate\n\n";

    for (const auto& algorithm : algorithms) {
        for (int agents : agent_counts) {
            std::cout << "\033[1;33m" << "----- Number of agents = " << agents << "\033[0m" << std::endl;
            for (int i = 1; i <= total_runs; ++i) {
                std::cout << "\033[1;34m" << "----- Scenario " << i << "/" << total_runs << "\033[0m" << std::endl;
                std::ostringstream scen_name;
                scen_name << "random-32-32-20-random-" << i << ".scen";
                std::string command = make_command(scen_name.str(), agents, algorithm);
                std::system(command.c_str());
            }

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
