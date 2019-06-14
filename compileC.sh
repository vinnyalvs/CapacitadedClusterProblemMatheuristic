#!/bin/bash

g++ main.cpp Source/*.cpp ExactSources/*.cpp -o program
./program ../Instancias/RanReal480/RanReal480_05.txt

