ssh-keygen -t rsa -b 2048 -f id_rsa

docker-compose build

docker-compose up -d

docker-compose exec mpi-node-1 mpirun -np 4 --host mpi-node-1,mpi-node-2,mpi-node-3,mpi-node-4 /app/mergesort
