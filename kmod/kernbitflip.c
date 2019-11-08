#include <linux/random.h>

unsigned int randomoffset(unsigned int);
void bitflip(void *, unsigned int);
void byteflip(void *, unsigned int);
void randbyte(void *, unsigned int);

unsigned int randomoffset(unsigned int SIZE) {
	unsigned int R;
	get_random_bytes(&R, sizeof(R));
	R = (R*2147483647)%SIZE;
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
	get_random_bytes(&C, sizeof(C));
	*B = C;
}
