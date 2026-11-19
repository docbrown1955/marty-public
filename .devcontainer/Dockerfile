# Ubuntu 22.04 with GCC 11 (validated toolchain in the README)
FROM ubuntu:22.04

ARG username=vscode

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        gcc-11 \
        g++-11 \
        g++ \
        gfortran-11 \
        gfortran \
        libgsl-dev \
        libgslcblas0 \
        texlive \
        texlive-luatex \
        dvipng \
        coreutils \
        imagemagick \
        qtbase5-dev \
        qtbase5-dev-tools \
        qtchooser \
        qt5-qmake \
        cmake \
        make \
        git \
        git-lfs \
        ca-certificates && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN useradd --create-home ${username}

USER ${username}

ENV CC=gcc-11
ENV CXX=g++-11
ENV FC=gfortran-11

CMD ["sleep", "infinity"]
