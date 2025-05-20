#!/bin/bash

for i in {1..10}; do
    echo "Ejecución $i"
    ./main 10 10 RC200 25
done
