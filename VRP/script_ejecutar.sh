#!/bin/bash

for i in {1..5}; do
    echo "Ejecución $i"
    ./main 10 10 RC100 50
done
