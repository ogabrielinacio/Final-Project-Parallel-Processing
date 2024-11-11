# Use Ubuntu as the base image
FROM ubuntu:20.04

# Set the environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

# Update packages and install OpenMPI and OpenSSH
RUN apt-get update && apt-get install -y \
    openssh-server \
    openmpi-bin \
    openmpi-common \
    make \
    sshpass \
    && apt-get clean

# Create the SSH directory and configure SSH
RUN mkdir /var/run/sshd \
    && echo 'root:root' | chpasswd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

COPY id_rsa.pub /root/.ssh/id_rsa.pub
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys

COPY id_rsa /root/.ssh/id_rsa
RUN cat /root/.ssh/id_rsa >> /root/.ssh/authorized_keys

RUN chmod 700 /root/.ssh
RUN chmod 600 /root/.ssh/authorized_keys

# Set the working directory
WORKDIR /app

COPY distributed-mergesort.c .

RUN mpicc  distributed-mergesort.c -o mergesort

# Expose SSH port
EXPOSE 22

# Start SSH server
CMD ["/usr/sbin/sshd", "-D"]
