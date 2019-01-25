#!/bin/bash

set -eux

rm -rf build
mkdir -p build
cd build
cmake ..
make -j 8

DATA_DIR="${1}"

time ./VCFLite -c -o -v "${DATA_DIR}/output.ann.vcf" "{DATA_DIR}/output.db"
time ./VCFLite -c -o -v "${DATA_DIR}/homo_sapiens-chr21.vcf" "${DATA_DIR}/homo.db"
