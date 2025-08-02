#!/bin/bash

set -e

build() {
    echo "Building..."
    mkdir -p build
    g++ -std=c++17 -Iinclude -Wall -O2 src/*.cpp -o build/brain-surgeon
}

case "$1" in
    build) build ;;
    *) echo "Usage: $0 {build}" && exit 1 ;;
esac
