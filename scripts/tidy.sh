#!/bin/sh

clang-format -i src/*.cpp src/*.h || exit 1
clang-tidy --quiet --fix -p . src/*.cpp || exit 1
