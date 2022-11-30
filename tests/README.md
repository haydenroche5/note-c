# Overview

This directory contains note-c unit tests written in C++ using GoogleTest. The
[Fake Function Framework](https://github.com/meekrosoft/fff) (fff) is used for
mocking and is bundled with the test code.

# Dependencies

- [CMake](https://cmake.org/install), version 3.4+
- [GoogleTest](https://github.com/google/googletest/tree/main/googletest)

# Running the Tests

```sh
mkdir build
cd build
cmake ..
make -j
ctest
```
