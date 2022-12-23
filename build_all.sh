#! /bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --leak-resolution=high --verbose --log-file=log.txt --smc-check=all ./apis_mt76xx
