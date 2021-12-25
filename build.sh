#!/usr/bin/env bash

cd cmake_build/ && \
    CC=clang CXX=clang++ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 .. && \
    make clean && \
    make && \
    cd -
