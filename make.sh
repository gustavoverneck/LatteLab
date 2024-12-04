#!/usr/bin/env bash

# Navigate to the project directory
cd "$(dirname "$0")"

# Clean previous builds
echo "Cleaning previous builds..."
mkdir -p bin
rm -rf exports
mkdir -p exports
rm -rf bin/LatteLab

# Compile the code with OpenMP support
echo "Compiling the code..."

# Compile using g++ with OpenMP support (-fopenmp)
g++ src/*.cpp -o bin/LatteLab -std=c++17 -fopenmp


# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable is located at 'bin/LatteLab'. Compiling time: $seconds seconds."
    echo "Executing LatteLab..."
    echo " "

    # Run the executable
    ./bin/LatteLab

    echo "Finished."

else
    echo "Compilation failed."
    exit 1
fi