#!/usr/bin/sh

g++ -O2 -std=c++11 ../mkut/custom-splattershot.cpp -o solver
g++ -O2 -std=c++11 bondrewd.cpp -o bondrewd
cat $1 | ./solver | ./bondrewd $1
