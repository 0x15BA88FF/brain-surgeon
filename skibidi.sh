#!/bin/bash

set -e

build() {
    echo "Building..."
    g++ -std=c++17 -Wall -O2 src/*.cpp -o build/brain-surgery
}

test() {
    ./build/brain-surgeon ./tests/hellom.bf
}

case "$1" in
    build) build ;;
    test) test ;;
    *) echo "Usage: $0 {build|test}" && exit 1 ;;
esac
