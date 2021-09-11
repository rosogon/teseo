(cd ./simulator && ./unittest_follow_wall_simulator.cpp.bin $*)
gnuplot -p -e 'plot "simulator/data.csv"'