#!/bin/bash

# N = 5 + n * 4

for ELEMENTS in {5..100..4}
do
    ./generator $ELEMENTS 1 input/tests
done