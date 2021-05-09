#!/bin/sh

RES_FILE=result.csv

echo 'file; DOFs; nodes; elements; solver; remainder; solving time; summary time' > $RES_FILE

./solver input/2.truss -dense -experiment >> $RES_FILE