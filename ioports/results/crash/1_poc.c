#include <sys/io.h>
#include <stdio.h>
#define DATA 0x54
#define PORT 0x92
void main(){printf("outb(%x,%x)\n",DATA,PORT);ioperm(PORT,PORT,1);outb(DATA,PORT);}
