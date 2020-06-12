#!/bin/bash

bench=('bench.cc')

rm -rf ./data/test-*
for f in ${bench[@]}; do
    exe=$(echo $f | cut -d . -f1)
    echo $f
    g++ -std=c++11 -o $exe -g -I.. $f  -L../lib -lengine -lpthread
done
