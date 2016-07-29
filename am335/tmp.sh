#!/bin/bash

if ! ./build.sh || ! ./build.sh install; then
    exit 1
fi

pushd
cd ../test/am335
./build.sh && ./build.sh install
popd
