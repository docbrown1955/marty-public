# Fedora 41 uses gcc-14
FROM registry.fedoraproject.org/fedora:41

ARG username=vscode

RUN dnf update -y && \
    dnf upgrade -y && \
    dnf install \
        gcc \
        gfortran \
        g++ \
        gsl \
        gsl-devel \
        texlive \
        ImageMagick \
        dvipng \
        coreutils \
        qtchooser \
        cmake \
        git \
        ninja-build \
        git-clang-format \
        -y && \
    dnf clean all  

RUN useradd ${username}

USER ${username}

ENV CC=gcc
ENV CXX=g++
ENV FC=gfortran

CMD ["sleep", "infinity"]
