#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status
set -x  # Print commands and their arguments as they are executed

echo "Cleaning up previous build..."

echo "Generating build files..."
cmake --preset=default
cmake -S . -B build

echo "Building the project..."
cmake --build build

echo "Running the executable..."
./build/volum-backend "$@"