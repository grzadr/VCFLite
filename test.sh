#!/bin/bash

set -eux

rm -rf build
mkdir -p build
cd build
cmake ..
make -j 8

DATA_DIR="${1}"
OUTPUT_DIR="${2}"

./VCFLite -h || true
time ./VCFLite --build -oi -s "89" --samples "120" --vcf="${DATA_DIR}/output.ann.vcf" -c -v="${DATA_DIR}/output.ann.vcf" "${OUTPUT_DIR}/output.db"
time ./VCFLite --build -oi --vcf "${DATA_DIR}/output.ann.vcf" -c -v "${DATA_DIR}/output.ann.vcf" "${OUTPUT_DIR}/output.db"
#time ./VCFLite -c -o -i -v "${DATA_DIR}/homo_sapiens-chr21.vcf" "${OUTPUT_DIR}/homo.db"
