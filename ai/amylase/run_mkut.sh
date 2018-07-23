#!/usr/bin/sh

g++ -O2 -std=c++11 ../mkut_test/splattershotjr.cpp -o solver
g++ -O2 -std=c++11 bondrewd.cpp -o bondrewd
cat $1 | ./solver | ./bondrewd $1
