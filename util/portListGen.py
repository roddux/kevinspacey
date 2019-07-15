#!/usr/bin/env python3
import sys

# WIP

# pipe to this from 
# cat /proc/ioports|sed -s 's/ //g'|sed -s 's/:.*//g'|sort|uniq
portListRaw = sys.stdin.read()
portList = portListRaw.split("\n")

# Get rid of blank entry at end of file
if "" in portList: portList.pop(portList.index(""))

# How do we deal with buses?
for RANGE in portList:
	start,end = RANGE.split("-")
	start = hex(int("0x0"+start,16))
	end = hex(int("0x0"+end,16))
	print(start, "to", end)
