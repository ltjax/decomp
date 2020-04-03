FROM gcc:9.1

RUN mkdir -p /opt/cmake && \
    wget https://github.com/Kitware/CMake/releases/download/v3.17.0/cmake-3.17.0-Linux-x86_64.sh -O cmake.sh && \
    sh ./cmake.sh --exclude-subdir --prefix=/opt/cmake && \
    rm -rf cmake.sh
ENV PATH="/opt/cmake/bin:${PATH}"

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

RUN gcc --version && cmake --version
ADD source /opt/decomp/source
ADD test /opt/decomp/test
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
 && make install
ADD demo.cpp /opt/decomp_demo/demo.cpp
RUN cd /opt/decomp_demo \
 && g++ -std=c++11 demo.cpp -o demo -ldecomp \
 && ./demo
