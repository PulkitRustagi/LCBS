project (Multi_objective_search)

cmake_minimum_required (VERSION 2.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LINKER "/usr/bin/ld")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Wall")

include_directories("inc")

file(GLOB SOURCES "src/*.cpp" "src/HighLevel/*.cpp" "src/LowLevel/*.cpp" "src/LowLevel/Utils/*.cpp")

# Remove simulate_algo.cpp to avoid duplicate main() error
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/simulate_baseline_comparisons_2min.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/simulate_baseline_comparisons_1min.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/simulate_slack_experiments.cpp)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options log thread filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

message(${CMAKE_BINARY_DIR})

# Main solver binary
add_executable(bbmocbs_approx src/main.cpp ${SOURCES})
target_link_libraries(bbmocbs_approx ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim1_comparisons_2min src/simulate_baseline_comparisons_2min.cpp)
target_link_libraries(sim1_comparisons_2min ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim1_comparisons_1min src/simulate_baseline_comparisons_1min.cpp)
target_link_libraries(sim1_comparisons_1min ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim2_slack_experiment src/simulate_slack_experiments.cpp)
target_link_libraries(sim2_slack_experiment ${Boost_LIBRARIES})
