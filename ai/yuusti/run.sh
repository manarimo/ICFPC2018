#!/usr/bin/env bash
g++ -O2 -std=c++11 ジーフィル速いんだけど微妙にバグる.cpp -o a.out
./a.out < $1
