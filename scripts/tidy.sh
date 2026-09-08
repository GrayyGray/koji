#!/bin/sh

clang-format -i $(find src -name '*.cpp') $(find src -name '*.h') || exit 1
clang-tidy --quiet --fix -p . $(find src -name '*.cpp') $(find src -name '*.h') || exit 1