#General Makefile variables
CC = gcc
CCPLUS = g++
FLAGS = -Wall -pedantic -O3 --std=c++11 -larmadillo 
FLAGS_OMP = #-fopenmp

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
	$(CCPLUS) main.cpp -o solver $(FLAGS) $(FLAGS_OMP)

#Rule to clear out output files
clean:
	rm -rf $(OUTPUT)
