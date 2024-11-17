# Distributed Merge Sort with Docker and MPI

This project demonstrates a distributed merge sort algorithm using **Docker** and **MPI (Message Passing Interface)**. Below are the steps to set up and execute the distributed merge sort.

---

## Prerequisites

- **Docker**: Ensure Docker is installed and running.
- **Docker Compose**: Required for managing the containers.

---
## Steps to Run the Project

1. **Generate SSH Keys**
   Generate an SSH key pair for enabling password-less SSH communication between the nodes:
   ```bash
   ssh-keygen -t rsa -b 2048 -f id_rsa
    ```

2. **Build the Docker Containers**
Build the Docker containers using the docker-compose.yml file:
   ```bash
   docker-compose build
    ```

3. **Start the Docker Containers**
Launch the containers in detached mode:
    ```bash
    docker-compose up -d
    ```

4. **Run Initialization Scripts**
Execute the initialization script to configure SSH and prepare the environment:
    ```bash
    ./exec-script-docker.sh
    ```

5. **Access the Primary Node**
Log into the primary node (mpi-node-1) container:
    ```bash
    docker exec -it tr_final_mpi-node-1_1 bash
    ```

5. **Run the Distributed Program**
Inside the primary node container, execute the distributed merge sort script:
    ```bash
    ./dist-run.sh
    ```
