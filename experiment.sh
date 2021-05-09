#!/bin/sh

RES_FILE=result.csv
DIRECTORY="input/tests"/*

echo 'solver; file; DOFs; nodes; elements; remainder; solving time; summary time' | tee $RES_FILE

for file in $DIRECTORY
do
    ./solver "$file" -dense -experiment | tee -a $RES_FILE
    printf "\n" | tee -a $RES_FILE
done
