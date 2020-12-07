/* iof - IO port fuzzer
 * roddux / 2019-07-14
 * Remember to compile with 'gcc -O2 -o example example.c'
 * Unless you keep the 'extern static' before the io.h include
 */

// TODO: update seed to be uint64_t

#include <stdio.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include "portlist.h"

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

/*volatile sig_atomic_t stop = 0;
void inthand(int signum) {
        stop = 1;
}*/
 
#define FUZLOOP 1000

int main(int argc, char **argv) {
        puts("[>] iof - IO port fuzzer");
	uint32_t custom_seed = 0;

	if ( argc == 2 ) {
		custom_seed = atol(argv[1]);
	}
        uint32_t seed = 0;
	if ( custom_seed != 0 ) {
		seed = custom_seed;
	} else {
		seed = getSeed();
	}
        printf("[+] Using random seed: %u\n", seed);
        srandom(seed);
	uint32_t count = 0;
//        signal(SIGINT, inthand);

        puts("[+] Getting access to ports");
        if (ioperm(0x0000, 0xFFFF, 1)) {
                perror("[!] Could not get access to ports");
                exit(1);
        }

//	puts("[+] Entering fuzzing loop, hit Control-C to exit");
        printf("[+] Fuzzing %d cases\n", FUZLOOP);
//	while (stop == 0) {
	while (count != FUZLOOP) {
//		uint16_t portNum = rand() % 0xFFFF;
		uint16_t portNum = portList[rand() % PORTLISTLEN];
//		printf("[+] Chose portList offset: %d\n", portNum);
//		printf("[+] Chose portNum: %#4x\n", portList[portNum]);
//		printf("[+] Chose portNum: %#4x\n", portNum);

                /* Grab a random bit and fire it at the port
                 * yes, we're truncating, no, i don't care */
                uint8_t DATA = rand();
		uint8_t in = 0;
//		printf("[+] Got random data: %#2x\n", DATA);

		/* Either read or write */
		if (rand() % 2) {
			switch(rand() % 3) {
				case 0:
					printf("[+] Sending: outb(%#4x,%#2x)\n", DATA, portNum);	
					outb(DATA, portNum);
					break;
				case 1:
					printf("[+] Sending: outw(%#4x,%#2x)\n", DATA, portNum);	
					outw(DATA, portNum);
					break;
				case 2:
					printf("[+] Sending: outl(%#4x,%#2x)\n", DATA, portNum);	
					outl(DATA, portNum);
					break;
			}
		} else {
			switch(rand() % 3) {
				case 0:
					printf("[+] Reading: inb(%#4x): ", portNum);
					in = inb(portNum);
					printf("%#4x\n", in);
					break;
				case 1:
					printf("[+] Reading: inw(%#4x): ", portNum);
					in = inw(portNum);
					printf("%#4x\n", in);
					break;
				case 2:
					printf("[+] Reading: inl(%#4x): ", portNum);
					in = inl(portNum);
					printf("%#4x\n", in);
					break;
			}
			//printf("[+] Reading: inb(%#4x): ", portList[portNum]);
//			//portNum = rand() % 0xFFFF;
			//uint8_t in = inb(portList[portNum]);
//			printf("%#4x\n", inb(portList[portNum]));
//			printf("%#4x\n", inb(portNum));
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
