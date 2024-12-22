# Project description
- Language: C++20
- Build: CMake
- Testing: GTest

# Build instructions

Build system uses CMake

## Build
Build the binary in ./build/logparser and ./build/logparser_sanitized:
```sh
make
```

## Test (Google Tests, links with system)
Run test
```sh
make test
```

Run tests with sanitizers on:
```sh
make test_sanitized
```

# Usage example
./build/logparser test1.txt
