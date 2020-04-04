all: docker_build

INSTALL_DIRECTORY=decomp_install
build:
	mkdir -p build
	cd build && \
	cmake \
        -Ddecomp_USE_CONAN:BOOL=OFF \
        -Ddecomp_BUILD_TESTS:BOOL=ON \
        -DCMAKE_INSTALL_PREFIX=$(shell pwd)/${INSTALL_DIRECTORY} \
        .. && \
    make && \
    make install && \

demo: demo.cpp
	g++ -std=c++11 demo.cpp -o demo -I${INSTALL_DIRECTORY}/include -L${INSTALL_DIRECTORY}/lib -ldecomp

plot: demo
	./demo | python

.PHONY = plot

docker_build:
	docker build . -t decomp
