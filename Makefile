#General Makefile variables
CXX = g++
CXXFLAGS = -Wall -pedantic -O1 --std=c++11 -larmadillo 
FLAGS_OMP = #-fopenmp

#Source files that should be compiled
SRC = $(wildcard *.cpp)

#Output files that should be cleared
OUTPUT = solver

#Optimize Makefile with stating that theses words regarding rules are not actual files
.PHONY: build

#State which rules should be silent (no console output)
.SILENT: clean

#General rule buils FEM Solver
build: solver

#Rule to build the solver program
solver:
	$(CXX) $(SRC) -o $@ $(CXXFLAGS) $(FLAGS_OMP)

#Rule to clear out output files and generated vector graphics
clean:
	rm -rf $(OUTPUT)
	rm -rf output/*.svg
	rm -rf output/*.csv
