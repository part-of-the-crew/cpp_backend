#!/usr/bin/env bash
#set -e  # stop on first error

# --- Configuration ---
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/tests"  # change this to your target name
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
    cmake --build "$BUILD_DIR" --parallel $(nproc) 2>&1
}

clean() {
    echo "🧹 Cleaning build directory..."
    rm -rf "$BUILD_DIR"
}

run() {
    if [[ ! -x "$EXECUTABLE" ]]; then
        echo "❌ Executable not found! Run ./build.sh build first."
        exit 1
    fi
    echo "🚀 Running program..."
    "$EXECUTABLE"
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
    run)
        run "$@"
        ;;
    *)
        #clean
        configure
        build || exit 1
        run
        ;;
esac
