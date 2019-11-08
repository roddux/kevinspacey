default:
	rm -f ./bf; gcc -g -std=c2x -Wall -Wextra -Wpedantic -Wno-unused-parameter bitflip.c -o bf && ./bf

gen-poc:
	python  single_poc_gen.py

run-poc:
	./util/pocr.sh

build-poc:
	./util/pocb.sh

poc: gen-poc build-poc run-poc

clean:
	rm -rf ./iof ./poc/*.{c,bin} fuzz.id __pycache__
