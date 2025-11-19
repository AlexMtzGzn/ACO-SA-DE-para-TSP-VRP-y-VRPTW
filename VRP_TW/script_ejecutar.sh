#!/bin/bash
./main 10 5 C102 50

for i in {1..3}; do
    echo "Ejecución $i"
    ./main 10 5 C101 50
    ./main 10 5 C103 50
    ./main 10 5 C104 50
    ./main 10 5 C105 50
    ./main 10 5 C106 50
    ./main 10 5 C102 50
    ./main 10 5 C108 50
    ./main 10 5 C109 50
done

for i in {1..5}; do
    echo "Ejecución $i"
    ./main 10 5 C107 50
    ./main 25 25  C208 25
done



