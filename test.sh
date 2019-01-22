#!/bin/bash

set -eux

rm -rf build
mkdir -p build
cd build
cmake ..
make -j 8
./VCFLite -c -o -v /Dropbox/NGS/homo_sapiens-chr21.vcf homo.db
./VCFLite -c -o -v /Dropbox/NGS/output.ann.vcf output.db

