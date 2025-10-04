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
./build/imgconv ./test_cases/reading_dog.ppm ./build/_.jpeg
colordiff -yq ./build/_.jpeg ./test_cases/_.jpeg

if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC}
else
    printf ${RED}___FAIL!!!___${NC}
fi
echo ""

rm -rf ./build

