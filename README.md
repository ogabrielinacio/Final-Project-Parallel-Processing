# Distributed Merge Sort with Docker and MPI

This project demonstrates a distributed merge sort algorithm using **Docker** and **MPI (Message Passing Interface)**. Below are the steps to set up and execute the distributed merge sort.

---

## Prerequisites

- **Docker**: Ensure Docker is installed and running.
- **Docker Compose**: Required for managing the containers.

---
## Steps to Run the Project

1. **Generate SSH Keys**
   Generate an SSH key pair to enable password-less SSH communication between nodes. Follow these steps:

   1.1 Run the following command to create an RSA key pair:
    ```bash
    ssh-keygen -t rsa -b 2048 -f id_rsa
    ```
    1.2 When prompted, press `Enter` twice to accept the default file location and leave the password empty (this generates a certificate without a passphrase).

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
Log into the primary node container (mpi-node-1) via SSH. The default password is root.
    ```bash
    ssh root@172.21.0.2
    ```

6. **Access the Working Directory**
  Navigate to the application's working directory to execute scripts or manage files:
    ```bash
    cd /app
    ```

7. **Run the Distributed Program**
Once inside the primary node container, execute the distributed merge sort script:
    ```bash
    ./dist-run.sh
    ```
