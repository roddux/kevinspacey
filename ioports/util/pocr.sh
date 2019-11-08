#!/usr/bin/env bash

cd poc
for x in *.bin; do
	echo "Running case $x"
	./$x
	echo "OKAY"
done
