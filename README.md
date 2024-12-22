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
Builds the binary in ./build/logparser and ./build/logparser_sanitized


## Tests, (Google Tests, links with system)
```sh
make test
```
Runs tests

```sh
make test_sanitized
```
Runs tests with sanitizers on

# Usage example
./build/logparser test1.txt
