#!/usr/bin/env bash
g++ -O2 -std=c++11 ハローワーク.cpp -o a.out
./a.out < $1
