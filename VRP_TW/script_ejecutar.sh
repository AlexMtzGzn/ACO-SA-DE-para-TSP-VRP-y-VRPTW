#!/bin/bash

for i in {1..10}; do
    echo "Ejecución $i"
    ./main 10 5 C104 50
    ./main 10 5 C105 50
    ./main 10 5 C106 50
    ./main 10 5 C203 50
    ./main 10 5 C204 50
done

for i in {1..5}; do
    echo "Ejecución $i"
    ./main 10 5 C203 50
    ./main 10 5 C204 50
done







