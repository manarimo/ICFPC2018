#!/usr/bin/sh
g++ -O3 -std=c++11 custom-splattershot.cpp -o a.out
./a.out < $1
