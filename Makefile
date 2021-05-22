#General Makefile variables
CXX = g++
CXXFLAGS = -Wall -pedantic -O3 --std=c++11
FLAGS_ARMADILLO = -larmadillo 
FLAGS_OMP = -fopenmp

#Source files that should be compiled
SRC = $(filter-out generator.cpp, $(wildcard *.cpp))

#Output files that should be cleared
OUTPUT = solver

#Optimize Makefile with stating that theses words regarding rules are not actual files
.PHONY: build

#State which rules should be silent (no console output)
.SILENT: clean

#General rule buils FEM Solver
build: solver generator

#Rule to build the solver program
solver:
	$(CXX) $(SRC) -o $@ $(CXXFLAGS) $(FLAGS_ARMADILLO) $(FLAGS_OMP)

generator:
	$(CXX) generator.cpp -o generator $(CXXFLAGS)

#Rule to clear out output files and generated vector graphics
clean:
	rm -rf $(OUTPUT)
	rm -rf generator
	rm -rf output/*.svg
	rm -rf output/*.csv
	rm -rf *.txt
