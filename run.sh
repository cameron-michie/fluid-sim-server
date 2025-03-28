#!/bin/bash

# Exit immediately if a command exits with a non-zero status.
set -e

# Compile the C++ program into a shared library
./compile_cpp.sh

# Activate the virtual environment
source venv/bin/activate

# Run the Python script
python3 scripts/main2.py
