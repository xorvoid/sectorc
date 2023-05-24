#!/bin/bash
set -e
objdump -D -b binary -m i386 -Maddr16,data16 -M intel "$1"
