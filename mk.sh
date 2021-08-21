#!/bin/sh

rm -rf build/* bin/*
cd build
cmake ..
make
