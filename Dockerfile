FROM debian:buster-slim as builder

RUN apt-get update --yes --allow-releaseinfo-change \
 && apt-get install --no-install-recommends --yes \
        build-essential \
        ca-certificates \
        cmake \
        g++ \
        unzip \
        wget \
 && apt-get clean \
 && rm -rf /tmp/* /var/tmp/* \
 && rm -rf /var/lib/apt/lists

RUN wget https://github.com/catchorg/Catch2/archive/v2.11.3.tar.gz -O catch2.tar.gz && \
    mkdir -p catch2_SRC && \
    tar -xf catch2.tar.gz --strip 1 -C catch2_SRC && \
    mkdir -p catch2_build && \
    cd catch2_build && \
    cmake \
        -DBUILD_TESTING=OFF \
        ../catch2_SRC && \
    make install && \
    cd .. && \
    rm -rf catch2_build  && \
    rm -rf catch2.tar.gz catch2_SRC

ADD source /opt/decomp/source
ADD test /opt/decomp/test
ADD demo /opt/decomp/demo
ADD CMakeLists.txt /opt/decomp/.
RUN cd /opt/decomp \
 && ls \
 && mkdir -p build \
 && cd build \
 && cmake \
       -Ddecomp_USE_CONAN:BOOL=OFF \
       -Ddecomp_BUILD_TESTS:BOOL=ON \
        .. \
 && make \
 && make test \
 && make install

FROM debian:buster-slim as runner
COPY --from=builder /usr/local/bin/decomp_demo /usr/local/bin/decomp_demo
ENTRYPOINT [ "/usr/local/bin/decomp_demo" ]
