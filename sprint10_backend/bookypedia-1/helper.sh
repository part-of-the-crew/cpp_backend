#!/usr/bin/env bash
#set -e  # stop on first error

# --- Configuration ---
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/bookypedia"  # change this to your target name
ARGS="-c data/config.json -w static -s Recovery.txt"
#:  "${TEST_EXECUTABLE:=${EXECUTABLE}_tests}"
"${TEST_EXECUTABLE:=${BUILD_DIR}/tests}"

# --- Helper functions ---
configure() {
    clang-format --style=file --assume-filename=../.clang-format -i ./src/*.cpp  ./src/*.h
    mkdir $BUILD_DIR
    echo "⚙️  Configuring Conan..."$BUILD_DIR"/"
    cd $BUILD_DIR
    conan install ..  --build=missing -s compiler.libcxx=libstdc++11 -s build_type=Debug
    cd ..
    echo "⚙️  Configuring CMake..."
    cmake -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B "$BUILD_DIR"  2>&1
}

build() {
    echo "🔨 Building project..."
    cmake --build "$BUILD_DIR" --parallel $(nproc)  2>&1
}

clean() {
    echo "🧹 Cleaning build directory..."
    rm -rf "$BUILD_DIR"
}

run() {
    # First: if tests exist, run them and abort on failure.
    if [[ -x "$TEST_EXECUTABLE" ]]; then
        printf "\n🚀 Running tests...\n"
        #"$TEST_EXECUTABLE" || exit 1
        ctest --test-dir "$BUILD_DIR" || exit 1
        #echo "✅ Tests passed."
        #echo "❌ Tests failed — aborting."
    else
        echo "⚠️  No test executable found at: $TEST_EXECUTABLE, Skipping tests"
    fi
    if [[ ! -x "$EXECUTABLE" ]]; then
        echo "❌ Executable not found! Run ./build.sh build first."
        exit 1
    fi
    echo "🚀 Running program..."
    "$EXECUTABLE" $ARGS
}
runt() {
    if [[ ! -x "$TEST_EXECUTABLE" ]]; then
        echo "⚠️  No test executable found at: $TEST_EXECUTABLE"
        exit 1
    fi
    printf "\n🚀 Running tests...\n"
    "$EXECUTABLE"_tests || exit 1
}
runv() {
    if [[ ! -x "$EXECUTABLE" ]]; then
        echo "❌ Executable not found! Run ./build.sh build first."
        exit 1
    fi
    echo "🚀 Running program..."
    valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out$i.txt \
        "$EXECUTABLE" 2>&1
}

# --- Main logic ---
case "$1" in
    conf)
        configure
        ;;
    build)
        build
        ;;
    clean)
        clean
        ;;
    rt)
        runt
        ;;
    run)
        run "$@"
        ;;
    runv)
        runv "$@"
        ;;
    vrun)
        runv "$@"
        ;;
    prof)
        cmake -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DEXTRA_CXX_FLAGS="-pg -g -O2" \
                    -B "$BUILD_DIR"  2>&1
        build || exit 1
        "$EXECUTABLE" $ARG
        PID=$!
        ./run_curl.sh 
        kill $PID
        gprof "$EXECUTABLE" | gprof2dot | dot -Tpng -o output.png &
        ;;
    perfsh)
        cmake -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DEXTRA_CXX_FLAGS="-g -O2" \
                    -B "$BUILD_DIR"  2>&1
        build || exit 1
        perf record -g -o perf.data "$EXECUTABLE" $ARGS &
        ./run_curl.sh 
        perf script | ~/Downloads/FlameGraph/stackcollapse-perf.pl | ~/Downloads/FlameGraph/flamegraph.pl > flame.svg
        ;;
    perfp)
        cmake -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DEXTRA_CXX_FLAGS="-g -O2" \
                    -B "$BUILD_DIR"  2>&1
        build || exit 1
        git clone https://github.com/brendangregg/FlameGraph
        python3 shoot.py ""$EXECUTABLE" $ARGS"
        ;;
    *)
        #clean
        configure
        #echo "cmake --build ./$BUILD_DIR"
        build || exit 1
        run
        ;;
esac
