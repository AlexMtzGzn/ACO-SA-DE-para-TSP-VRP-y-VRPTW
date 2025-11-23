#!/bin/bash
    ./main 5 8 C105 50
    ./main 5 8 C106 50
for i in {1..4}; do
    echo "Ejecución $i"
    ./main 5 8 C104 50
    ./main 5 8 C105 50
    ./main 5 8 C106 50
done








