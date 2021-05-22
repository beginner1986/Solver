#!/bin/sh

LU_RES_FILE=result-lu.csv
QR_RES_FILE=result-qr.csv
JACOBI_RES_FILE=result-jacobi.csv
GS_RES_FILE=result-gs.csv
LAPACK_RES_FILE=result-lapack.csv
SUPERLU_RES_FILE=result-superlu.csv

DIRECTORY="input/tests"/*

printf '\e[1;31m' 
printf "solver; DOFs; nodes; elements; remainder; solving time; summary time\n" | tee $LU_RES_FILE | tee $QR_RES_FILE | tee $JACOBI_RES_FILE 
echo "solver; DOFs; nodes; elements; remainder; solving time; summary time" | tee $GS_RES_FILE | tee $LAPACK_RES_FILE | tee $SUPERLU_RES_FILE
printf "_______________________________________________________________________________________\n"
printf '\e[1;34m%-6s\e[m\n'

for file in $DIRECTORY
do
    printf '\e[1;34m%-6s\e[m' "LU | $file: "
    ./luSolver "$file" -dense -experiment | sed -r 's/\./,/g' | tee -a $LU_RES_FILE
    printf "\n" | tee -a $LU_RES_FILE

    printf '\e[1;34m%-6s\e[m' "QR | $file: "
    ./qrSolver "$file" -dense -experiment | sed -r 's/\./,/g' | tee -a $QR_RES_FILE
    printf "\n" | tee -a $QR_RES_FILE

    printf '\e[1;34m%-6s\e[m' "JAC | $file: "
    ./jacobiSolver "$file" -dense -experiment | sed -r 's/\./,/g' | tee -a $JACOBI_RES_FILE
    printf "\n" | tee -a $JACOBI_RES_FILE

    printf '\e[1;34m%-6s\e[m' "GS | $file: "
    ./gsSolver "$file" -dense -experiment | sed -r 's/\./,/g' | tee -a $GS_RES_FILE
    printf "\n" | tee -a $GS_RES_FILE

    printf '\e[1;34m%-6s\e[m' "LAP | $file: "
    ./lapackSolver "$file" -sparse -experiment | sed -r 's/\./,/g' | tee -a $LAPACK_RES_FILE
    printf "\n" | tee -a $LAPACK_RES_FILE

    printf '\e[1;34m%-6s\e[m' "SLU | $file: "
    ./superluSolver "$file" -sparse -experiment | sed -r 's/\./,/g' | tee -a $SUPERLU_RES_FILE
    printf "\n" | tee -a $SUPERLU_RES_FILE

    rm $file
done
