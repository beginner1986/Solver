#!/bin/sh

RES_FILE=result.csv
DIRECTORY="input/tests"/*

printf '\e[1;31m' 
printf "solver; DOFs; nodes; elements; remainder; solving time; summary time\n" | tee $RES_FILE
printf "_______________________________________________________________________________________\n"

for file in $DIRECTORY
do
    printf '\e[1;34m%-6s\e[m' "$file: "
    ./solver "$file" -dense -experiment | sed -r 's/\./,/g' | tee -a $RES_FILE
    printf "\n" | tee -a $RES_FILE
done
