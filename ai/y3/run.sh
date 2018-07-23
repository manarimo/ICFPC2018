#!/bin/sh
g++ -O3 -std=c++11 hydrant.cpp -o a.out
./a.out < $1
