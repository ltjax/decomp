all: docker_build

INSTALL_DIRECTORY=decomp_install

build: ${INSTALL_DIRECTORY}/lib/libdecomp.a

${INSTALL_DIRECTORY}/lib/libdecomp.a:
	mkdir -p build && \
    cd build && \
    cmake \
        -Ddecomp_USE_CONAN:BOOL=OFF \
        -Ddecomp_BUILD_TESTS:BOOL=ON \
        -DCMAKE_INSTALL_PREFIX=$(shell pwd)/${INSTALL_DIRECTORY} \
        .. && \
    make && \
    make install

demo/demo: demo/demo.cpp ${INSTALL_DIRECTORY}/lib/libdecomp.a
	g++ -std=c++11 \
		demo/demo.cpp \
		-o demo/demo \
		-I${INSTALL_DIRECTORY}/include \
		-L${INSTALL_DIRECTORY}/lib -ldecomp

plot: demo/demo
	./demo/demo | python

clean:
	@rm -rf build
	@rm -rf ${INSTALL_DIRECTORY}
	@rm -f demo/demo

.PHONY = plot clean

docker_build:
	docker build . -t decomp
