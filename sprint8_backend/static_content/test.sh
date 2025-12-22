#!/usr/bin/env bash
#set -e  # stop on first error

# --- Configuration ---
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/bin/game_server"  # change this to your target name
ARGS="data/config.json static"
# --- Helper functions ---
configure() {
    clang-format --style=file --assume-filename=../.clang-format -i ./src/*.cpp  ./src/*.h
    mkdir $BUILD_DIR
    echo "⚙️  Configuring Conan..."$BUILD_DIR"/"
    cd $BUILD_DIR
    conan install ..
    cd ..
    echo "⚙️  Configuring CMake..."
    cmake -S . -B "$BUILD_DIR"
}

build() {
    echo "🔨 Building project..."
    cmake --"$BUILD_DIR" "$BUILD_DIR" -j
    #setsid cmake --build "$BUILD_DIR" -j >/dev/null 2>&1 &
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
    "$EXECUTABLE" $ARGS
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
    configure)
        configure
        ;;
    build)
        build
        ;;
    clean)
        clean
        ;;
    run)
        shift
        run "$@"
        ;;
    runv)
        shift
        runv "$@"
        ;;
    vrun)
        shift
        runv "$@"
        ;;
    *)
        clean
        configure
        #echo "cmake --build ./$BUILD_DIR"
        build
        ;;
esac
