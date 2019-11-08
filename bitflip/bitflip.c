#include <stdio.h>
#include <stdlib.h>
#include <sys/random.h>
#include <string.h>
#include <strings.h>

#define SIZE 48

void printbuf(void *);
unsigned int randomoffset();
void byteflip(void *);
void bitflip(void *);
void randbyte(void *);
void asbits(unsigned char);

int main(int argc, char **argv) {
	puts("[>] fuzz function tests");

	char *buff = malloc(sizeof(char)*SIZE);
	memcpy(buff, "JUST PRETEND that this string's super important.", SIZE);

	printbuf(buff);
	randbyte(buff);
	byteflip(buff);
	bitflip(buff);
	printbuf(buff);

	puts("[>] Done");
	return 0;
}

void printbuf(void *buf) {
	printf("[+] buffer as string:\n'%s'\n", (char *)buf);
	printf("[+] buffer as hex:");
	for(int i=0;i<SIZE;i++) {
		if(i%8==0) puts(" ");
		printf("%02X ", ((unsigned char *)buf)[i]);
	} puts("");
}

unsigned int randomoffset() {
	if(SIZE > RAND_MAX) {
		puts("[+] buffer bigger than RAND_MAX! bailing");
		exit(1);
	}
	unsigned int R;
	getrandom(&R, sizeof(unsigned int), 0);
	printf("[+] random:   %u\n", R);
	R = (R*RAND_MAX)%SIZE;
	printf("[+] mod SIZE: %u\n", R);
	return R;
}

void asbits(unsigned char val) {
	for (int i = 7; 0 <= i; i--) {
		printf("%c", (val & (1 << i)) ? '1' : '0');
	}
}

void bitflip(void *buffer) {
	unsigned int R = randomoffset();
	unsigned char *B = &((unsigned char *)buffer)[R];
	unsigned int bitNum = randomoffset();
	bitNum = bitNum%7;
	printf("[+] flipping bit %d of byte %d (0x%02x:%c)\n", bitNum, R, *B, *B);
	printf("[+] from: "); asbits(*B); puts("");
	unsigned int mask = (1 << bitNum);
	*B ^= mask;
	printf("[+] to:   "); asbits(*B); puts("");
	printf("[+] new byte is (0x%02x:%c)\n", *B, *B);
}

void byteflip(void *buffer) {
	unsigned int R = randomoffset();
	unsigned char *B = &((unsigned char *)buffer)[R];
	printf("[+] flipping bits of byte %d (0x%02x:%c)\n", R, *B, *B);
	printf("[+] from: "); asbits(*B); puts("");
	*B = *B^~0;
	printf("[+] to:   "); asbits(*B); puts("");
	printf("[+] new byte is (0x%02x:%c)\n", *B, *B);
}

void randbyte(void *buffer) {
	unsigned int R = randomoffset();
	unsigned char *B = &((unsigned char *)buffer)[R];
	unsigned char C;
	getrandom(&C, sizeof(C), 0);
	printf("[+] changing byte %d (0x%02x:%c) to 0x%02x(%c)\n", R, *B, *B, C, C);
	*B = C;
}
