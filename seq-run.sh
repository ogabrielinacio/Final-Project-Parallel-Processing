#!/usr/bin/env bash

ARRAY=(10 100 1000 10000 100000 1000000 5000000 10000000 50000000 100000000)

if [ ${#ARRAY[@]} -eq 0 ]; then
    echo "Error: ARRAY is empty"
    exit 1
fi

rm -rf results/
mkdir -p results

for ARRAY_VALUE in "${ARRAY[@]}"; do
    echo "Processing ARRAY_VALUE=$ARRAY_VALUE"

    echo "Compiling generate_array.c with ARRAY_VALUE=$ARRAY_VALUE"
    gcc -DARRAY_VALUE=$ARRAY_VALUE -o generate_array generate_array.c
    if [ $? -ne 0 ]; then
        echo "Error compiling generate_array.c"
        exit 1
    fi

    echo "Running generate_array..."
    ./generate_array
    if [ $? -ne 0 ]; then
        echo "Error running generate_array"
        exit 1
    fi

    echo "Compiling sequencial-mergesort.c"
    gcc -o sequencial-mergesort sequencial-mergesort.c
    if [ $? -ne 0 ]; then
        echo "Error compiling sequencial-mergesort.c"
        exit 1
    fi

    echo "Running sequencial-mergesort..."
    { time ./sequencial-mergesort > temp_output.txt; } 2> temp_time.txt

    real_time=$(grep real temp_time.txt | awk '{print $2}')
    user_time=$(grep user temp_time.txt | awk '{print $2}')
    sys_time=$(grep sys temp_time.txt | awk '{print $2}')
    echo "ARRAY_VALUE=$ARRAY_VALUE: real=$real_time, user=$user_time, sys=$sys_time" >> results/time.txt

    echo "Processing output into separate files..."
    awk -v value="$ARRAY_VALUE" '
    /EOF/ { found_eof = 1; next }
    !found_eof { print > "results/non_ordered_array_" value ".txt"; next }
    found_eof { print > "results/ordered_array_" value ".txt" }
    ' temp_output.txt

    rm temp_time.txt
    rm temp_output.txt

    echo "Outputs saved to non_ordered_array_$ARRAY_VALUE.txt and ordered_array_$ARRAY_VALUE.txt"
done

echo "All values processed successfully."
