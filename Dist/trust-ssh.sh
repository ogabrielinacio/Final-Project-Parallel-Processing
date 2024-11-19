#!/bin/bash

hosts=("mpi-node-1" "mpi-node-2" "mpi-node-3" "mpi-node-4")

for host in "${hosts[@]}"; do
    echo "Testing connection to $host..."

    sshpass ssh -o StrictHostKeyChecking=accept-new "$host" exit

    if [ $? -eq 0 ]; then
        echo "Connection to $host successful and SSH key trusted."
    else
        echo "Failed to connect to $host."
    fi
done
