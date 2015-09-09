#!/bin/sh
g++ -std=c++11 -I../../include/ -I../../src -L../../res/lib/ -Wl,-rpath=./res/lib:. -Wl,--no-as-needed -fpic --shared FileWriter.cpp -o FileWriter.so -lmaudio -lsndfile -pthread && \
mkdir -p ../../res/plugins && cp FileWriter.so ../../res/plugins/FileWriter.so
