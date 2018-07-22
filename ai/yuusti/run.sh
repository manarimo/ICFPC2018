#!/usr/bin/env bash
g++ -O2 -std=c++11 大体正の点数を取る.cpp -o a.out
./a.out < $1
