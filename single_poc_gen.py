#!/usr/bin/env python3
import os,random
os.chdir("poc")

f="""#include <sys/io.h>
#include <stdio.h>
#define DATA REPLACE_DATA
#define PORT REPLACE_PORT
void main(){printf("outb(%x,%x)\\n",DATA,PORT);ioperm(PORT,PORT,1);outb(DATA,PORT);}"""

# hard-coded interesting ports
ports=["0x0170", "0x0171", "0x0172", "0x0173", "0x0174", "0x0175", "0x0176", "0x0177", "0x01f0", "0x01f1", "0x01f2", "0x01f3", "0x01f4", "0x01f5", "0x01f6", "0x01f7", "0x0376"]

for X in range(1000):
        fname=str(X)+"_poc.c"
        q=open(fname,"w")
        q.write(f.replace("REPLACE_PORT",random.choice(ports)).replace("REPLACE_DATA",hex(random.randint(0,255))))
        q.close()
