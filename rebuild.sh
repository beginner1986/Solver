#!/bin/sh

echo Cleaning the solver...
make clean

echo Building the solver...
make build

echo Cleaning the generator...
cd input
make clean

echo Building the generator...
make build

echo Running the project...
cd ..
./solver input/2.truss -dense