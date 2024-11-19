FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive
ENV OMPI_ALLOW_RUN_AS_ROOT=1
ENV OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1

RUN apt-get update && apt-get install -y \
    iputils-ping \
    vim \
    strace \
    openssh-server \
    openmpi-bin \
    openmpi-common \
    make \
    sshpass \
    && apt-get clean

RUN mkdir /var/run/sshd \
    && echo 'root:root' | chpasswd \
    && sed -i 's/#PermitRootLogin prohibit-password/PermitRootLogin yes/' /etc/ssh/sshd_config \
    && sed -i 's/#PasswordAuthentication yes/PasswordAuthentication yes/' /etc/ssh/sshd_config

COPY id_rsa.pub /root/.ssh/id_rsa.pub
RUN cat /root/.ssh/id_rsa.pub >> /root/.ssh/authorized_keys

COPY id_rsa /root/.ssh/id_rsa
RUN cat /root/.ssh/id_rsa >> /root/.ssh/authorized_keys

RUN chmod 600 /root/.ssh/id_rsa.pub
RUN chmod 600 /root/.ssh/id_rsa

RUN chmod 700 /root/.ssh
RUN chmod 600 /root/.ssh/authorized_keys

WORKDIR /app

COPY distributed-mergesort.c .

COPY generate_array.c .

COPY dist-run.sh .

COPY trust-ssh.sh .

RUN chmod +x trust-ssh.sh

EXPOSE 22

CMD ["/usr/sbin/sshd", "-D"]
