#!/bin/sh
g++ -O3 -std=c++11 hydrant20.cpp -o a.out
./a.out < $1
