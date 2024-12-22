# Project description
- Language: C++20
- Build: CMake
- Testing: GTest

# Build instructions

Build system uses CMake

## Build
```sh
make
```
or
```sh
mkdir -p build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

Builds the binary in ./build/logparser


## Tests, (Google Tests, links with system)
```sh
make test
```
Runs tests

# Usage example
./build/logparser test1.txt
