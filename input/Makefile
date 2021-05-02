#General Makefile variables
CXX = g++
CXXFLAGS = -Wall -pedantic --std=c++11

#Source files that should be compiled
SRC = $(wildcard *.cpp)

#Output files that should be cleared
OUTPUT = generator

#Optimize Makefile with stating that theses words regarding rules are not actual files
.PHONY: build

#State which rules should be silent (no console output)
.SILENT: clean

#General rule buils FEM Solver
build: generator

#Rule to build the solver program
generator:
	$(CXX) $(SRC) -o $@ $(CXXFLAGS)

#Rule to clear out output files and generated vector graphics
clean:
	rm -rf $(OUTPUT)
