#!/bin/bash

N = 5 + 20 * 4

for ELEMENTS in {5..85..4}
do
    ./generator $ELEMENTS 1 input/tests
done