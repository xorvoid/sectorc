#!/bin/bash
set -e
THISDIR=$(dirname $(realpath $0))
cd $THISDIR

SRC=sectorc.s
BIN=build/sectorc.bin

## output dir for build artifacts
mkdir -p build

## assemble sectorc
nasm -f bin -o $BIN $SRC

## build a helpful linter
gcc -std=c11 -Wall -Werror -O2 -g -o build/lint lint/lint.c
