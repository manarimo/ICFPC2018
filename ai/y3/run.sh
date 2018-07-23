#!/bin/sh
g++ -O3 -std=c++11 ../kawatea/shiva.cpp -o a.out
./a.out < $1 > deconstruction
g++ -O2 -std=c++11 ../yuusti/ジーフィル速いんだけど微妙にバグる.cpp -o a.out
./a.out < $2 > construction
sed '$d' deconstruction
cat construction
