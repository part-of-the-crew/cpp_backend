#!/usr/bin/env bash
#set -e  # stop on first error
set -Eeuo pipefail
# --- Configuration ---
IMAGE="server_logging:v1"
CONTAINER="server"

# --- Helper functions ---
configuref() {
    clang-format --style=file --assume-filename=../.clang-format -i ./src/*.cpp  ./src/*.h
}

buildf() {
    echo "🔨 Building project..."
    docker build -t "$IMAGE" .
}

rebuildf() {
    echo "🔨 Rebuilding project..."
    docker build --no-cache -t "$IMAGE" .
}
runf() {
    # -d runs them in the background. --remove-orphans cleans up old containers.
    docker compose -f docker-compose.yaml up -d
    docker stop "$CONTAINER" 2>/dev/null || true
    docker run --rm --name "$CONTAINER" -p 8080:8080 "$IMAGE" | python3 ./web_exporter.py 
}

# --- Main logic ---
case "${1:-}" in
    configure)
        configuref
        ;;
    b)
        buildf
        ;;
    r)
        runf
        ;;
    rb)
        rebuildf
        ;;
    *)
        buildf
        runf
        ;;
esac
