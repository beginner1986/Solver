#!/bin/sh

LAPACK_RES_FILE=result-lapack.csv
SUPERLU_RES_FILE=result-superlu.csv

DIRECTORY="input/tests"/*

printf '\e[1;31m' 
printf "solver; DOFs; nodes; elements; remainder; solving time; summary time\n" | tee $LAPACK_RES_FILE | tee $SUPERLU_RES_FILE
printf "_______________________________________________________________________________________\n"
printf '\e[1;34m%-6s\e[m\n'

for file in $DIRECTORY
do
    printf '\e[1;34m%-6s\e[m' "LAP | $file: "
    ./lapackSolver "$file" -sparse -experiment | sed -r 's/\./,/g' | tee -a $LAPACK_RES_FILE
    printf "\n" | tee -a $LAPACK_RES_FILE

    printf '\e[1;34m%-6s\e[m' "SLU | $file: "
    ./superluSolver "$file" -sparse -experiment | sed -r 's/\./,/g' | tee -a $SUPERLU_RES_FILE
    printf "\n" | tee -a $SUPERLU_RES_FILE

    rm $file
done
