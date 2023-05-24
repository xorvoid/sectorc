#!/bin/bash
set -e
THISDIR=$(dirname $(realpath $0))
cd $THISDIR

if [ "$#" != 1 ]; then
    echo "usage: $0 <main-source-file>"
    exit 1
fi

input="rt/lib.c $1 rt/_start.c"

./build/lint $input
./run_raw.sh $input
