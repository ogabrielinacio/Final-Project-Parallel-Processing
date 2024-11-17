#!  /usr/bin/env bash

containers=("tr_final_mpi-node-1_1" "tr_final_mpi-node-2_1" "tr_final_mpi-node-3_1" "tr_final_mpi-node-4_1")

for container in "${containers[@]}"; do
  echo "Executing trust-ssh.sh on container: $container"
  docker exec -it "$container" bash -c "/app/trust-ssh.sh"
done
