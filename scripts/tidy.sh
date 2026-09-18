#!/bin/bash

clang-format -i $(find src -name '*.cpp') $(find src -name '*.h') &&
clang-tidy --quiet --fix -p . $(find src -name '*.cpp') $(find src -name '*.h')
