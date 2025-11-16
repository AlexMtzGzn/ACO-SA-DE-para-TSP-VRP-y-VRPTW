#!/bin/bash

for i in {1..2}; do
    echo "Ejecución $i"
    ./main 8 10 C103 25
done


for i in {1..3}; do
    echo "Ejecución $i"
    ./main 15 15 C205 25
done


for i in {1..4}; do
    echo "Ejecución $i"
    ./main 15 15 C208 25
done

