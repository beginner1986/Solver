#!/bin/sh

RES_FILE=result.csv
DIRECTORY="input/tests"/*

echo 'file; DOFs; nodes; elements; solver; remainder; solving time; summary time' > $RES_FILE

for file in $DIRECTORY
do
    ./solver "$file" -dense -experiment >> $RES_FILE
    printf "\n" >> $RES_FILE
done
