#!/usr/bin/sh

MAIN_SOLVER=../kawatea/run.sh

g++ -O3 -std=c++11 bondrewd.cpp -o bondrewd
MAIN_SOLVER $1 | ./bondrewd $1
