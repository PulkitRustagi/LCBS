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

list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/run_sim_success_contours.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/run_sim_success_rates.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/run_optimality_experiments.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/run_optimality_experiments1.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/run_optimality_experiments2.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/aggregate_lexopt_percentages.cpp)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options log thread filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

message(${CMAKE_BINARY_DIR})

# Main solver binary
add_executable(lcbs_sim src/main.cpp ${SOURCES})
target_link_libraries(lcbs_sim ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_success_rates src/run_sim_success_rates.cpp)
target_link_libraries(sim_success_rates ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_success_contours src/run_sim_success_contours.cpp)
target_link_libraries(sim_success_contours ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_opt_time_check src/run_optimality_experiments.cpp)
target_link_libraries(sim_opt_time_check ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_opt_time_check1 src/run_optimality_experiments1.cpp)
target_link_libraries(sim_opt_time_check1 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_opt_time_check2 src/run_optimality_experiments2.cpp)
target_link_libraries(sim_opt_time_check2 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_opt_aggregation src/aggregate_lexopt_percentages.cpp)
target_link_libraries(sim_opt_aggregation ${Boost_LIBRARIES})