#!/bin/bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

count=$(printf '%s\n' ./test_cases/input* | wc -l)

for (( i=1; i<=${count}; i++ ))
do
   echo "Test$i..."
   ./executable < ./test_cases/input$i.txt > ./test_cases/output$i.txt
    #valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out$i.txt \
    #     ./executable <./test_cases/input$i.txt > ./test_cases/output$i.txt 2>&1
   diff -Bb --strip-trailing-cr -U 3 ./test_cases/et_output$i.txt ./test_cases/output$i.txt
   if [ $? -eq 0 ]; then
       printf ${GREEN}___PASS!___${NC}
   else
       RET=$?
       printf ${RED}___FAIL!!!___${NC}
   fi
   echo ""
   rm ./test_cases/output$i.txt
done


