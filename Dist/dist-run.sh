#!/usr/bin/env bash

ARRAY=(10 100 1000 10000 25000 50000 75000 100000 1000000 5000000 10000000 50000000)

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

    echo "Compiling distributed-mergesort.c"
    mpicc  distributed-mergesort.c -o mergesort
    if [ $? -ne 0 ]; then
        echo "Error compiling distributed-mergesort.c"
        exit 1
    fi

    echo "Running distributed-mergesort..."
    { time mpirun  --allow-run-as-root --mca btl tcp,self --host mpi-node-1,mpi-node-2,mpi-node-3,mpi-node-4 -np 4 mergesort  > temp_output.txt; } 2> temp_time.txt

    real_time=$(grep real temp_time.txt | awk '{print $2}')
    user_time=$(grep user temp_time.txt | awk '{print $2}')
    sys_time=$(grep sys temp_time.txt | awk '{print $2}')
	echo "ARRAY_VALUE=$ARRAY_VALUE: real=$real_time, user=$user_time, sys=$sys_time" >> fix_time.txt

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
    awk 'NR % 3 == 1 { printf "%s ", $0; next } NR % 3 == 0 { print $0 } NR % 3 == 2 { printf "%s ", $0 }' fix_time.txt > results/time.txt

    rm fix_time.txt

   echo "All values processed successfully."
