all:
	mkdir -p build && cd build && cmake .. && cmake --build .

test: all
	./build/tests

test_sanitized: all
	./build/tests_sanitized

clean:
	rm -rf build
