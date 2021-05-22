#!/bin/bash

# N = 5 + n * 4

for ELEMENTS in {5..100..10}
do
    ./generator $ELEMENTS 1 input/tests
done

for ELEMENTS in {110..1000..100}
do
    ./generator $ELEMENTS 1 input/tests
done

for ELEMENTS in {1010..10000..1000}
do
    ./generator $ELEMENTS 1 input/tests
done

for ELEMENTS in {10010..100000..10000}
do
    ./generator $ELEMENTS 1 input/tests
done

for ELEMENTS in {100010..1000010..100000}
do
    ./generator $ELEMENTS 1 input/tests
done
