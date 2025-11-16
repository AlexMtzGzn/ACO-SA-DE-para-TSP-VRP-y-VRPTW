#!/bin/bash

for i in {1..5}; do
    echo "Ejecución $i"
    ./main 5 5 C103 25
    ./main 5 5 C105 25
    ./main 5 5 C106 25
    ./main 5 5 C107 25
    ./main 5 5 C108 25

    ./main 5 5 C201 25
    ./main 5 5 C202 25
    ./main 5 5 C203 25
    ./main 8 10 C204 25
    ./main 5 5 C205 25
    ./main 5 5 C206 25
    ./main 5 5 C207 25
    ./main 5 5 C208 25
done
