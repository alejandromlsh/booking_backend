FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install all dependencies including Python and SSL libraries
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        build-essential \
        g++ \
        cmake \
        git \
        wget \
        ca-certificates \
        python3 \
        python3-dev \
        autoconf \
        automake \
        libtool \
        libicu-dev \
        libbz2-dev \
        zlib1g-dev \
        libssl-dev \
        pkg-config \
        python-is-python3 \
        && rm -rf /var/lib/apt/lists/*

# Install Boost 1.85 with fallback download sources
WORKDIR /tmp
RUN (wget https://archives.boost.io/release/1.85.0/source/boost_1_85_0.tar.gz || \
     wget http://download.openpkg.org/components/cache/boost/boost_1_85_0.tar.gz) && \
    tar xzf boost_1_85_0.tar.gz && \
    cd boost_1_85_0 && \
    ./bootstrap.sh --prefix=/usr/local --with-python=python3 && \
    ./b2 install -j$(nproc) link=shared runtime-link=shared && \
    cd .. && \
    rm -rf boost_1_85_0 boost_1_85_0.tar.gz

# Install GoogleTest
RUN git clone --depth 1 --branch release-1.14.0 https://github.com/google/googletest.git && \
    cd googletest && \
    mkdir build && \
    cd build && \
    cmake .. -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=/usr/local -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc) && \
    make install && \
    cd /tmp && \
    rm -rf googletest

WORKDIR /app
COPY . /app

CMD ["/bin/bash"]
