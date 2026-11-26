# Ubuntu 24.04 with Clang 18 and GCC/GFortran 14 (toolchain used by CI)
FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install -y \
        clang-18 \
        gcc-14 \
        g++-14 \
        gfortran-14 \
        libgsl-dev \
        libgslcblas0 \
        texlive \
        texlive-luatex \
        texlive-science \
        texlive-latex-extra \
        dvipng \
        lcov \
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
        python3 \
        python3-pip \
        python3-venv \
        clang-format \
        gdb \
        valgrind \
        vim \
        ca-certificates && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ENV CC=clang-18
ENV CXX=clang++-18
ENV FC=gfortran-14

CMD ["sleep", "infinity"]
