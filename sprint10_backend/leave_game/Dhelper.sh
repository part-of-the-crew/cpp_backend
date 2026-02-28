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

runallf() {
    # -d runs them in the background. --remove-orphans cleans up old containers.
    docker compose -f docker-compose.yaml up -d
    docker stop "$CONTAINER" 2>/dev/null || true
    docker run --rm --name "$CONTAINER" -p 8080:8080 "$IMAGE" | python3 ./web_exporter.py 
}

final() {
    docker container stop postgres && docker container rm postgres
    docker run --name postgres -e POSTGRES_HOST_AUTH_METHOD=trust -d --rm postgres  2>/dev/null || true
    sleep 5
    docker stop "$CONTAINER" 2>/dev/null || true
    docker run --rm -e 'GAME_DB_URL=postgres://postgres:Mys3Cr3t@172.17.0.2:5432'  --name server -p 8080:8080 server_logging:v1
}

runf() {
    # -d runs them in the background. --remove-orphans cleans up old containers.
    docker stop "$CONTAINER" 2>/dev/null || true
    docker run --rm --name "$CONTAINER" -p 8080:8080 "$IMAGE"
}

# --- Main logic ---
case "${1:-}"  in
    configure)
        configuref
        ;;
    b)
        buildf
        ;;
    rall)
        runallf
        ;;
    final)
        final
        ;; 
    r)
        rf
        ;;
    rb)
        rebuildf
        ;;
    *)
        buildf
        runf
        ;;
esac
