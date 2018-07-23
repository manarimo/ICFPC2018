#!/usr/bin/sh

g++ -O2 -std=c++11 ../yuusti/ハローワーク.cpp -o solver
g++ -O2 -std=c++11 prushka.cpp -o prushka
cat $1 | ./solver | ./prushka $1
