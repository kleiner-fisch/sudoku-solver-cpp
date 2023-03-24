# sudoku-solver-cpp
Simple Sudoku solver written in c++

## Dependencies
This project uses boost v1.74.

## Building
On my system I did not need to include `boost/property_tree/ptree.hpp` and `boost/property_tree/json_parser.hpp` during compilation.
 
I could compile with `/usr/bin/g++ -g <REPO_HOME>/src/Sudoku2.cpp -o <REPO_HOME>/src/Sudoku2 -lboost_program_options`, where REPO_HOME is the directory where this readme resides in.

## Usage
To use this solver call `./<path-to-binary> --input <path-to-input-file>`, where
* path-to-binary is the path to the executable,
* path-to-input-file is the path to the json file containing the starting sudoku problem.

The format of the json file can be seen in `<REPO_HOME>/src/sudoku-example1.json`
