#!/usr/bin/env bash
set -Eeuo pipefail

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

printf ${GREEN}___join___${NC}
echo ""
TOKEN1=$(curl -s POST http://localhost:8080/api/v1/game/join \
    -H "Content-Type: application/json" \
    -d '{"userName":"Scooby1","mapId":"map1"}' \
    | jq -r '.authToken')
echo ""

printf ${GREEN}___join___${NC}
echo ""
TOKEN2=$(curl -s POST http://localhost:8080/api/v1/game/join \
    -H "Content-Type: application/json" \
    -d '{"userName":"Scooby2","mapId":"map1"}' \
    | jq -r '.authToken')
echo ""


for i in {1..50}
do
    echo "========================================"
    echo "ITERATION: $i"
    echo "========================================"

    printf "${GREEN}___stat___${NC}\n"
    curl -s GET http://localhost:8080/api/v1/game/state \
        -H "Authorization: Bearer $TOKEN1"
    echo ""

    printf "${GREEN}___action___${NC}\n"
    curl -s POST http://localhost:8080/api/v1/game/player/action \
        -H "Content-Type: application/json" \
        -H "Authorization: Bearer $TOKEN1" \
        -d '{"move":"D"}'
    echo ""

    printf "${GREEN}___action___${NC}\n"
    curl -s POST http://localhost:8080/api/v1/game/player/action \
        -H "Content-Type: application/json" \
        -H "Authorization: Bearer $TOKEN2" \
        -d '{"move":"U"}'
    echo ""

    printf "${GREEN}___tick___${NC}\n"
    curl -s POST http://localhost:8080/api/v1/game/tick  \
        -H "Content-Type: application/json"  \
        -d '{"timeDelta":100}'
    echo ""

    printf "${GREEN}___state___${NC}\n"
    curl -s GET http://localhost:8080/api/v1/game/state \
        -H "Authorization: Bearer $TOKEN1"
    echo -e "\n"

    # sleep 0.1
done