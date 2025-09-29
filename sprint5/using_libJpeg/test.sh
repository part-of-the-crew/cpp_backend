#!/bin/bash

g++ hmirr.cpp ppm_image.cpp img_lib.cpp
./a.out ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_h.ppm
    rm ./a.out ./test_cases/_.ppm
g++ vmirr.cpp ppm_image.cpp img_lib.cpp
./a.out ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_v.ppm
    rm ./a.out ./test_cases/_.ppm
g++ sobel.cpp ppm_image.cpp img_lib.cpp
./a.out ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_s.ppm
    rm ./a.out ./test_cases/_.ppm
