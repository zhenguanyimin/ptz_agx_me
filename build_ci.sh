#!/bin/bash
set -e

cd $(dirname $0)

rm -rf build

mkdir build

cd build

cmake .. -DCI=1 "$@"

make 
