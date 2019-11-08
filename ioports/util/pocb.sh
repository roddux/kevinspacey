#!/usr/bin/env bash

cd poc
#for x in *.c; do gcc $x -o $x.bin -O2; done
fd . -e c | xargs -P10 -I {} gcc -O2 {} -o {}.bin
