#!/bin/bash
for file in C*.txt R*.txt; do
    if [[ ! "$file" =~ \(_100\)\.txt$ ]]; then
        mv "$file" "${file%.txt}_(100).txt"
    fi
done
