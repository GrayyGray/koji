#!/bin/sh

clang-format -i src/*.cpp src/*.h || exit 1
clang-tidy --fix -p src/*.cpp || exit 1
bear -- make all || exit 1
