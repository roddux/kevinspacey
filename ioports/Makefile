default:
	rm -f ./iof; gcc -Wall -Wextra -Wpedantic iof.c -o ./iof

gen-poc:
	python util/poc_gen.py

build-poc:
	./util/pocb.sh

run-poc:
	./util/pocr.sh

portlist:
	cat /proc/ioports|sed -s 's/ //g'|egrep -i '(ahci|ata|pci|dma)'|sed -s 's/:.*//g'|sort|uniq | ./util/portListGen.py > portlist.h

poc: gen-poc build-poc run-poc

clean:
	rm -rf ./iof ./poc/*.{c,bin} fuzz.id __pycache__
