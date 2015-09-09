#!/bin/sh
g++ -std=c++11 -I../../include/ -I../../src -L../../res/lib/ -Wl,-rpath=./res/lib -Wl,--no-as-needed -fpic --shared Mixer.cpp -o Mixer.so -lmaudio -ldl -lportaudio && \
mkdir -p ../../res/plugins && cp Mixer.so ../../res/plugins/Mixer.so
