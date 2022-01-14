all: tree.o memsys.o
	clang -std=c99 -Wall -pedantic tree.o memsys.o -o tree

tree.o: tree.c memsys.c tree.h memsys.h
	clang -std=c99 -Wall -pedantic -c tree.c memsys.c

clean: 
	rm *.o tree
