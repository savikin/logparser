# Build instructions

- Build
```sh
make
```
Builds the binary in ./build/logparser

    - Alternatively
```sh
mkdir -p build && cd build && cmake .. && cmake --build .
```

- Tests
```sh
make test
```
Runs tests

# Usage example
./build/logparser test1.txt
