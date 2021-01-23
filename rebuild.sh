#!/bin/sh

echo Cleaning the project...
make clean

echo Building the project...
make build

echo Running the project...
echo 
./solver input/2.truss