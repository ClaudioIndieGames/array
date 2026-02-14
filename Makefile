all: example

example: example.o
	gcc -o example example.o

example.o: example.c array.h
	gcc -O0 -Wall -ggdb -c example.c

clean:
	rm example.exe example.o