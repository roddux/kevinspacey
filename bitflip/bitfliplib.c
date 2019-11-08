#include <stdlib.h>
#include <sys/random.h>

unsigned int randomoffset(unsigned int);
void bitflip(void *, unsigned int);
void byteflip(void *, unsigned int);
void randbyte(void *, unsigned int);

unsigned int randomoffset(unsigned int SIZE) {
	// if(SIZE > RAND_MAX) return -1;
	unsigned int R;
	getrandom(&R, sizeof(unsigned int), 0);
	R = (R*RAND_MAX)%SIZE;
	return R;
}

void bitflip(void *buffer, unsigned int BUFSZ) {
	unsigned int R = randomoffset(BUFSZ), bitNum = randomoffset(7)%7;
	unsigned char *B = &((unsigned char *)buffer)[R];
	*B ^= (1 << bitNum);
}

void byteflip(void *buffer, unsigned int BUFSZ) {
	unsigned int R = randomoffset(BUFSZ);
	unsigned char *B = &((unsigned char *)buffer)[R];
	*B = *B^~0;
}

void randbyte(void *buffer, unsigned int BUFSZ) {
	unsigned int R = randomoffset(BUFSZ);
	unsigned char *B = &((unsigned char *)buffer)[R], C;
	getrandom(&C, sizeof(C), 0);
	*B = C;
}
