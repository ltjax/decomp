all: plot

INSTALL_DIRECTORY=decomp_install

build: ${INSTALL_DIRECTORY}/lib/libdecomp.a

${INSTALL_DIRECTORY}/bin/decomp_demo: ${INSTALL_DIRECTORY}/lib/libdecomp.a

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

plot: ${INSTALL_DIRECTORY}/bin/decomp_demo
	${INSTALL_DIRECTORY}/bin/decomp_demo | python

clean:
	@rm -rf build
	@rm -rf ${INSTALL_DIRECTORY}
	@rm -f demo/demo

.PHONY = plot clean
