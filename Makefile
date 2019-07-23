default:
	gcc -Wall -Wextra -Wpedantic iof.c -o iof

gen-poc:
	python test.py

run-poc:
	./util/pocr.sh

build-poc:
	./util/pocb.sh

poc: gen-poc build-poc run-poc

clean:
	rm -rf ./iof ./poc/*.{c,bin} fuzz.id __pycache__
