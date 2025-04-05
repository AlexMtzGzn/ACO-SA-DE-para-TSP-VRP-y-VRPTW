#!/bin/bash
for file in C*.txt R*.txt; do
    if [[ ! "$file" =~ \(_25\)\.txt$ ]]; then
        mv "$file" "${file%.txt}_(25).txt"
    fi
done
