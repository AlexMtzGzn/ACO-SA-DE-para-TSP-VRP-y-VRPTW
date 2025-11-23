#!/bin/bash
./main 7 15 C204 50
./main 7 15 C204 50
for i in {1..13}; do
    echo "Ejecución $i"
    ./main 7 15 C203 50
    ./main 7 15 C204 50
done



