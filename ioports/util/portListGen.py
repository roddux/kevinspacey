#!/usr/bin/env python3
from sys import stdin

# pipe to this from 
# cat /proc/ioports|sed -s 's/ //g'|sed -s 's/:.*//g'|sort|uniq
portListRaw = stdin.read()
portList = portListRaw.split("\n")

# Get rid of blank entry at end of file
if "" in portList: portList.pop(portList.index(""))

PORTS = []
for RANGE in portList:
	start,end = RANGE.split("-")
	start = int("0x0"+start,16)
	end = int("0x0"+end,16)
	for Q in range(end-start):
		PORTS += [str(hex(Q+start)), ]

PORTLIST = f"uint16_t portList[] = {{{','.join(PORTS)}}}\n#define PORTLISTLEN {len(PORTS)}\n"

print(PORTLIST)
