#!/bin/bash

for i in {1..5}; do
    echo "Ejecución $i"
    ./main 8 10 C103 25
    ./main 8 10 C205 25
    ./main 8 10 C208 25
done
