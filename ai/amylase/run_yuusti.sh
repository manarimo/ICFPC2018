#!/usr/bin/sh

g++ -O2 -std=c++11 ../yuusti/ジーフィル速いんだけど微妙にバグる.cpp -o solver
g++ -O2 -std=c++11 bondrewd.cpp -o bondrewd
cat $1 | ./solver | ./bondrewd $1
