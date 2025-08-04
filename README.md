# Lexicographic Conflict-Based Search (LCBS)

You can compile the project by 

```
mkdir build
cd build
cmake ..
make -j
```

You can type `lcbs_sim --help` to see the expected input arguments.

To run simulation for success rate for all approaches on all benchmarks (scalability in #agents):
```
./bin/sim_success_rates
```

To run simulation for success contours from all approaches on all benchmarks (scalability in #objectives):
```
./bin/sim_success_contours
```
