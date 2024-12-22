all:
	mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .

test: all
	./build/tests

test_sanitized: all
	./build/tests_sanitized

clean:
	rm -rf build
