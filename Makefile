.PHONY: all clean build
all: build

clean:
	rm -rf build

build:
	mkdir -p build && \
	cd build/ && \
	cmake .. && \
	cmake --build .
