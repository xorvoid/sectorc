#!/bin/bash
set -e
THISDIR=$(dirname $(realpath $0))
cd $THISDIR

if [ "$#" -lt 1 ]; then
    echo "usage: $0 [<source-file> ...]"
    exit 1
fi

# Handle running on Linux that usually uses PulseAudio and doesn't have coreaudio.
is_linux=$(uname | grep Linux)
if [[ -z "$is_linux" ]]; then
  audiodev=coreaudio
else
  audiodev=alsa
fi 

cat $@ | qemu-system-i386 -hda build/sectorc.bin -serial stdio -audiodev $audiodev,id=audio0 -machine pcspk-audiodev=audio0
