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
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/test_epsilon_effect.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map1.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map2.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map3.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map4.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map5.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map6.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map7.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map8.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map9.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map10.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_1.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_2.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_3.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_4.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_5.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_6.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_7.cpp)
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/sim_map11_8.cpp)

# Find Boost
find_package(Boost REQUIRED COMPONENTS program_options log thread filesystem system)
include_directories(${Boost_INCLUDE_DIRS})

message(${CMAKE_BINARY_DIR})

# Main solver binary
add_executable(bbmocbs_approx src/main.cpp ${SOURCES})
target_link_libraries(bbmocbs_approx ${Boost_LIBRARIES})

# Simulation for
add_executable(vary_eps_LCBS src/test_epsilon_effect.cpp)
target_link_libraries(vary_eps_LCBS ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map1 src/sim_map1.cpp)
target_link_libraries(sim_on_map1 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map2 src/sim_map2.cpp)
target_link_libraries(sim_on_map2 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map3 src/sim_map3.cpp)
target_link_libraries(sim_on_map3 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map4 src/sim_map4.cpp)
target_link_libraries(sim_on_map4 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map5 src/sim_map5.cpp)
target_link_libraries(sim_on_map5 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map6 src/sim_map6.cpp)
target_link_libraries(sim_on_map6 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map7 src/sim_map7.cpp)
target_link_libraries(sim_on_map7 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map8 src/sim_map8.cpp)
target_link_libraries(sim_on_map8 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map9 src/sim_map9.cpp)
target_link_libraries(sim_on_map9 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map10 src/sim_map10.cpp)
target_link_libraries(sim_on_map10 ${Boost_LIBRARIES})

# Simulation automation binary
add_executable(sim_on_map11_1 src/sim_map11_1.cpp)
target_link_libraries(sim_on_map11_1 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_2 src/sim_map11_2.cpp)
target_link_libraries(sim_on_map11_2 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_3 src/sim_map11_3.cpp)
target_link_libraries(sim_on_map11_3 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_4 src/sim_map11_4.cpp)
target_link_libraries(sim_on_map11_4 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_5 src/sim_map11_5.cpp)
target_link_libraries(sim_on_map11_5 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_6 src/sim_map11_6.cpp)
target_link_libraries(sim_on_map11_6 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_7 src/sim_map11_7.cpp)
target_link_libraries(sim_on_map11_7 ${Boost_LIBRARIES})
# Simulation automation binary
add_executable(sim_on_map11_8 src/sim_map11_8.cpp)
target_link_libraries(sim_on_map11_8 ${Boost_LIBRARIES})
