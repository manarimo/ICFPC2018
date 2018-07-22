#!/usr/bin/sh

g++ -O2 -std=c++11 ../yuusti/大体正の点数を取る.cpp -o solver
g++ -O2 -std=c++11 bondrewd.cpp -o bondrewd
cat $1 | ./solver | ./bondrewd $1
