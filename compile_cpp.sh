#!/bin/bash

# Ensure we are in the correct directory
cd "$(dirname "$0")/shared"

# Set the output shared library name
OUTPUT="mylib.so"

# List of source files
SOURCES="Main.cpp Simulation.cpp Kernel.cpp Particles.cpp RectangularGrid.cpp Grid.cpp SimulationWrapper.cpp Triangulate.cpp Delaunator.hpp"

# Compile each source file into an object file
echo "Compiling source files into object files..."
for source in $SOURCES; do
    g++ -std=c++17 -fPIC -c $source || exit 1
done

# Link all object files into the final shared library
echo "Linking object files into a shared library..."
g++ -shared -o $OUTPUT *.o || exit 1

# Clean up object files
echo "Cleaning up object files..."
rm -f *.o

echo "C++ library compiled to shared/$OUTPUT"
