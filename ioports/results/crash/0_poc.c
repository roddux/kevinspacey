#include <sys/io.h>
#include <stdio.h>
#define DATA 0xb3
#define PORT 0xa0
void main(){printf("outb(%x,%x)\n",DATA,PORT);ioperm(PORT,PORT,1);outb(DATA,PORT);}
