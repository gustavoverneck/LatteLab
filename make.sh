#!/usr/bin/env bash

# Navigate to the project directory
cd /home/gverneck/codes/LatteLab

# Clean previous builds
echo "Cleaning previous builds..."
mkdir -p bin
rm -rf bin/LatteLab

# Compile the code
echo "Compiling the code..."
g++ src/*.cpp -o bin/LatteLab -std=c++17

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable is located at 'bin/LatteLab'."
    echo "Running the program..."
    ./bin/LatteLab
    
else
    echo "Compilation failed."
    exit 1
fi