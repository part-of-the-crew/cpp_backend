#!/usr/bin/env bash
#set -e  # stop on first error

# --- Configuration ---
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/spreadsheet"  # change this to your target name

# --- Helper functions ---
configure() {
    mkdir build
    echo "⚙️  Configuring CMake..."
    cmake -S . -B "$BUILD_DIR"
}

build() {
    echo "🔨 Building project..."
    #cmake --build "$BUILD_DIR" -j
    setsid cmake --build "$BUILD_DIR" -j >/dev/null 2>&1 &
}

clean() {
    echo "🧹 Cleaning build directory..."
    rm -rf "$BUILD_DIR" "$GENERATED_DIR"
    rm -rf dist build
}

run() {
    if [[ ! -x "$EXECUTABLE" ]]; then
        echo "❌ Executable not found! Run ./build.sh build first."
        exit 1
    fi
    echo "🚀 Running program..."
    "$EXECUTABLE" "$@"
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
    *)
        configure
        build
        ;;
esac
