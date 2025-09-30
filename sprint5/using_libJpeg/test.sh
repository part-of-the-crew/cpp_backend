#!/bin/bash

mkdir build
cd build

cmake ..
cmake --build .

cd ..
./build/hmirror ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_h.ppm
    rm ./test_cases/_.ppm

./build/vmirror ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_v.ppm
    rm ./test_cases/_.ppm

./build/sobel ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_s.ppm
    rm ./test_cases/_.ppm

rm -rf ./build

