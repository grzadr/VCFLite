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
./VCFLite -v || true
./VCFLite --version || true
time ./VCFLite --build -oi -s "89" --samples "120" --vcf="${DATA_DIR}/output.ann.vcf" -c -v="${DATA_DIR}/output2.ann.vcf" -l 41200 "${OUTPUT_DIR}/output.db" "additional"
#time ./VCFLite --build -oi -s "89" --samples "120" --vcf="${DATA_DIR}/output.ann.vcf" -c -v="${DATA_DIR}/output2.ann.vcf" "${OUTPUT_DIR}/output.db" "additional"
time ./VCFLite -bc -o -v "${DATA_DIR}/homo_sapiens-chr21.vcf" "${OUTPUT_DIR}/homo.db" -i -l 5000
