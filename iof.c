/* iof - IO port fuzzer
 * roddux / 2019-07-14
 * Remember to compile with 'gcc -O2 -o example example.c'
 * Unless you keep the 'extern static' before the io.h include
 */
#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>

#define extern static
#include <sys/io.h>
#undef extern

#include <sys/random.h>

uint32_t getSeed() {
        uint32_t seed;
        ssize_t randRet;
	uint8_t tries = 0;
	while (1) {
		randRet = getrandom(&seed, sizeof(seed), GRND_RANDOM|GRND_NONBLOCK);
		if (randRet != sizeof(seed)) {
			printf("[!] Could not obtain decent random seed, attempt %d\n", tries);
			//exit(1);
		}
		tries+=1;
		return seed;
	}
}

volatile sig_atomic_t stop = 0;
void inthand(int signum) {
        stop = 1;
}

//uint16_t portList[] = {0x0170, 0x0171, 0x0172, 0x0173, 0x0174, 0x0175, 0x0176, 0x0177, 0x01f0, 0x01f1, 0x01f2, 0x01f3, 0x01f4, 0x01f5, 0x01f6, 0x01f7, 0x0376, 0x3c0, 0x3c1, 0x3c2, 0x3c3, 0x3c4, 0x3c5, 0x3c6, 0x3c7, 0x3c8, 0x3c9, 0x3ca, 0x3cb, 0x3cc, 0x3cd, 0x3ce, 0x3cf, 0x3d0, 0x3d1, 0x3d2, 0x3d3, 0x3d4, 0x3d5, 0x3d6, 0x3d7, 0x3d8, 0x3d9, 0x3da, 0x3db, 0x3dc, 0x3dd, 0x3de, 0x3df, 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,0xd048, 0xd049, 0xd04a, 0xd04b, 0xd04c, 0xd04d, 0xd04e, 0xd04f, 0xd050, 0xd051, 0xd052, 0xd053, 0xd054, 0xd055, 0xd056, 0xd057, 0xd058, 0xd059, 0xd05a, 0xd05b, 0xd05c, 0xd05d, 0xd05e, 0xd05f, 0xd060, 0xd061, 0xd062, 0xd063, 0xd064, 0xd065, 0xd066, 0xd067, 0xd068, 0xd069, 0xd06a, 0xd06b, 0xd06c, 0xd06d, 0xd06e, 0xd06f, 0xd070, 0xd071, 0xd072, 0xd073, 0xd074, 0xd075, 0xd076, 0xd077, 0xd078, 0xd079, 0xd07a, 0xd07b, 0xd07c, 0xd07d, 0xd07e, 0xd07f};
//#define PORTLISTLEN 32+17+32+56
 
#define FUZLOOP 31

int main(int argc, char **argv) {
        puts("[>] iof - IO port fuzzer");
	long custom_seed = 0;
	if ( argc == 2 ) {
		custom_seed = atol(argv[1]);
	}
        uint32_t seed = getSeed();
	if ( custom_seed != 0 ) {
		seed = custom_seed;
	}
        printf("[+] Using random seed: %u\n", seed);
        srandom(seed);
	uint32_t count = 0;
        signal(SIGINT, inthand);

        puts("[+] Getting access to ports");
        if (ioperm(0x0000, 0xFFFF, 1)) {
                perror("[!] Could not get access to ports");
                exit(1);
        }

	puts("[+] Entering fuzzing loop, hit Control-C to exit");
//        printf("[+] Fuzzing %d cases\n", FUZLOOP);
//	while (stop == 0) {
	while (count != FUZLOOP) {
		uint16_t portNum = rand() % 0xFFFF;
//              uint16_t portNum = rand() % PORTLISTLEN;
//		printf("[+] Chose portList offset: %d\n", portNum);
//		printf("[+] Chose portNum: %#4x\n", portList[portNum]);
//		printf("[+] Chose portNum: %#4x\n", portNum);
		/* We probably want some juicy IO ports though, not random ones.
                 * root can read /proc/ioports => make a parser */

                /* Grab a random bit and fire it at the port
                 * yes, we're truncating, no, i don't care */
                uint8_t DATA = rand();
//		printf("[+] Got random data: %#2x\n", DATA);

		/* Avoid some known-bad values 
		if (
			( DATA == 0xb3 && portNum == 0xa0 ) ||
			( DATA == 0x54 && portNum == 0x92 ) 
		) {
			printf("[!] Skipping known crash\n");
			continue;
		}*/

		// Avoid messy areas
		if (
			(portNum < 0x1040) || (portNum > 0x209f)
		) continue;

		/* Either read or write */
		if (rand() % 2) {
//	                printf("[+] Sending: outb(%#4x,%#2x)\n", DATA, portList[portNum]);	
	                printf("[+] Sending: outb(%#4x,%#2x)\n", DATA, portNum);	
	              	//outb(DATA, portList[portNum]);
			outb(DATA, portNum);
		} else {
			//printf("[+] Reading: inb(%#4x): ", portList[portNum]);
			printf("[+] Reading: inb(%#4x): ", portNum);
//			//portNum = rand() % 0xFFFF;
			//uint8_t in = inb(portList[portNum]);
			uint8_t in = inb(portNum);
//			printf("%#4x\n", inb(portList[portNum]));
//			printf("%#4x\n", inb(portNum));
			printf("%#4x\n", in);
		}
		count += 1;
        }

        puts("[+] Releasing port access");
        if (ioperm(0x0000, 0xFFFF, 0)) {
                perror("[!] Could not release port access");
                exit(1);
        }

        exit(0);
}
