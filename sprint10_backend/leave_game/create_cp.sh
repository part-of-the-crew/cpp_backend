#!/usr/bin/env bash
set -Eeuo pipefail

#cp -rs ./data ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/data
#cp -rs ./src ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/src
#cp -rs ./tests ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/tests
#cp -rs ./static ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/static
#
#cp -rs ./CMakeLists.txt ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/CMakeLists.txt
#cp -rs ./conanfile.txt ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/conanfile.txt
#cp -rs ./Dockerfile ./../../../cppbackend-practice/sprint4/problems/leave_game/solution/Dockerfile

# Define your destination once to save typing
DEST="./../../../cppbackend-practice/sprint4/problems/leave_game/solution"

# Create links using absolute source paths
cp -fru "$(pwd)/data" "$DEST"
cp -fru "$(pwd)/src" "$DEST"
cp -fru "$(pwd)/tests" "$DEST"
cp -fru "$(pwd)/static" "$DEST"

cp -fu "$(pwd)/CMakeLists.txt" "$DEST/CMakeLists.txt"
cp -fu "$(pwd)/conanfile.txt" "$DEST/conanfile.txt"
cp -fu "$(pwd)/Dockerfile" "$DEST/Dockerfile"