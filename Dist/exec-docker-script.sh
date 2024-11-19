#!  /usr/bin/env bash

containers=("dist_mpi-node-1_1" "dist_mpi-node-2_1" "dist_mpi-node-3_1" "dist_mpi-node-4_1")

for container in "${containers[@]}"; do
  echo "Executing trust-ssh.sh on container: $container"
  docker exec -it "$container" bash -c "/app/trust-ssh.sh"
done
