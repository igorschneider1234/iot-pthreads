#!/bin/bash
set -e

make
./main
echo "Arquivo gerado em output/resultado.csv"
