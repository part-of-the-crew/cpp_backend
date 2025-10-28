#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

mkdir build
cd build



cmake ../ImgConverter -DCMAKE_BUILD_TYPE=Debug -DLIBJPEG_DIR=/usr/lib/x86_64-linux-gnu/
if [ $? -eq 1 ]; then
    printf ${RED}___FAIL!!!___"cmake ../ImgConverter"${NC} && echo ""
    cd ..
    rm -rf ./build
    exit 1
fi

cmake --build . --verbose
cd ..

#ppm -> jpeg
./build/imgconv ./test_cases/reading_dog.ppm ./build/1.jpeg
colordiff -yq ./build/1.jpeg ./test_cases/1.jpeg

if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC} && echo ""
else
    printf ${RED}___FAIL!!!___${NC} && echo ""
fi

#bmp -> ppm
./build/imgconv ./test_cases/b4.bmp ./build/2.ppm
colordiff -yq ./build/2.ppm ./test_cases/b4.ppm

if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC} && echo ""
else
    printf ${RED}___FAIL!!!___${NC} && echo ""
fi


#ppm -> bmp
./build/imgconv ./test_cases/b4.ppm ./build/3.bmp
colordiff -yq ./build/3.bmp ./test_cases/b4.bmp

if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC} && echo ""
else
    printf ${RED}___FAIL!!!___${NC} && echo ""
fi


rm -rf ./build

