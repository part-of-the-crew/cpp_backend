#!/bin/bash
RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

mkdir build
cd build

cmake ..
cmake --build .

cd ..
./build/hmirror ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_h.ppm

if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC}
else
    printf ${RED}___FAIL!!!___${NC}
fi
echo ""
rm ./test_cases/_.ppm


./build/hmirror ./test_cases/patrik.ppm ./test_cases/_.ppm
./build/hmirror ./test_cases/_.ppm ./test_cases/__.ppm
colordiff -yq ./test_cases/__.ppm ./test_cases/patrik.ppm


if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC}
else
    printf ${RED}___FAIL!!!___${NC}
fi
echo ""
rm ./test_cases/__.ppm ./test_cases/_.ppm

./build/vmirror ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_v.ppm
if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC}
else
    printf ${RED}___FAIL!!!___${NC}
fi
echo ""
rm ./test_cases/_.ppm

./build/vmirror ./test_cases/patrik.ppm ./test_cases/_.ppm
./build/vmirror ./test_cases/_.ppm ./test_cases/__.ppm
colordiff -yq ./test_cases/__.ppm ./test_cases/patrik.ppm


if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC}
else
    printf ${RED}___FAIL!!!___${NC}
fi
echo ""
rm ./test_cases/__.ppm ./test_cases/_.ppm


./build/sobel ./test_cases/patrik.ppm ./test_cases/_.ppm
colordiff -yq ./test_cases/_.ppm ./test_cases/patrik_s.ppm
if [ $? -eq 0 ]; then
    printf ${GREEN}___PASS!___${NC}
else
    printf ${RED}___FAIL!!!___${NC}
fi
echo ""
rm ./test_cases/_.ppm


rm -rf ./build

