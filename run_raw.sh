#!/bin/bash
set -e
THISDIR=$(dirname $(realpath $0))
cd $THISDIR

if [ "$#" -lt 1 ]; then
    echo "usage: $0 [<source-file> ...]"
    exit 1
fi

cat $@ | qemu-system-i386 -drive file=build/sectorc.bin,format=raw -serial stdio -audiodev coreaudio,id=audio0 -machine pcspk-audiodev=audio0
