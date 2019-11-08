#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <string.h>
#include <strings.h>

#define SIZE 48

unsigned int randomoffset(unsigned int);
void bitflip(void *, unsigned int);
void byteflip(void *, unsigned int);
void randbyte(void *, unsigned int);

void printbuf(void *);
void asbits(unsigned char);

int main(int argc, char **argv) {
	puts("[>] bitflip library tests");

	char *buff = malloc(sizeof(char)*SIZE);
	memcpy(buff, "JUST PRETEND that this string's super important.", SIZE);

	printbuf(buff);
	randbyte(buff, SIZE);
	printbuf(buff);
	byteflip(buff, SIZE);
	printbuf(buff);
	bitflip(buff, SIZE);
	printbuf(buff);

	puts("[>] Done");
	return 0;
}

void printbuf(void *buf) {
	printf("[+] buffer as string: '%s'\n", (char *)buf);
	printf("[+] buffer as hex:");
	for(int i=0;i<SIZE;i++) {
		if(i%8==0) puts(" ");
		printf("%02X ", ((unsigned char *)buf)[i]);
	} puts("");
}

void asbits(unsigned char val) {
	for (int i = 7; 0 <= i; i--) {
		printf("%c", (val & (1 << i)) ? '1' : '0');
	}
}
