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
start_time=$(date +%s%3N)  # Capture the start time in milliseconds

# Compile using g++ with OpenMP support (-fopenmp)
g++ src/*.cpp -o bin/LatteLab -std=c++17 -fopenmp

end_time=$(date +%s%3N)    # Capture the end time in milliseconds
duration=$(( end_time - start_time ))  # Calculate the duration in milliseconds
seconds=$(echo "scale=3; $duration / 1000" | bc)

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Compilation successful. Executable is located at 'bin/LatteLab'. Compiling time: $seconds seconds."
    echo "Executing LatteLab..."
    echo " "

    # Time the execution of the program
    start_time=$(date +%s%3N)  # Capture the start time in milliseconds

    # Run the executable
    ./bin/LatteLab

    # Capture the end time and calculate the duration
    end_time=$(date +%s%3N)    # Capture the end time in milliseconds
    duration=$(( end_time - start_time ))  # Calculate the duration in milliseconds
    seconds=$(echo "scale=3; $duration / 1000" | bc)
    echo "Finished. Total execution time: $seconds seconds."

else
    echo "Compilation failed."
    exit 1
fi