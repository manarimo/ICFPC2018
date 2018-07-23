#!/usr/bin/sh

g++ -O2 -std=c++11 ../yuusti/ジーフィル速いんだけど微妙にバグる.cpp -o solver
g++ -O2 -std=c++11 prushka.cpp -o prushka
cat $1 | ./solver | ./prushka $1
