#!/bin/bash

for i in {8..15}; do
    echo "Ejecución $i"
    python3 ajustador.py C108 $i 100 
done



