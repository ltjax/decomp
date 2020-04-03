all:
	mkdir -p build
	cd build && \
	cmake \
        -Ddecomp_USE_CONAN:BOOL=OFF \
        -Ddecomp_BUILD_TESTS:BOOL=ON \
        .. && \
    make

docker_build:
	docker build . -t pouet