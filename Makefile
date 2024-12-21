all:
	mkdir -p build && cd build && cmake .. && cmake --build .

test: all
	./build/tests

