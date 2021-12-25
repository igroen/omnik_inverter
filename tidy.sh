#!/usr/bin/env bash

clang-tidy --fix \
           main.cpp \
           inverter.hpp \
           inverter.cpp \
           -- -stdlib=libc++ -std=c++20
