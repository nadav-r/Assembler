main: main.c main.h data_structures.o parsing.o assembling.o data_structures.h  printings.o
	gcc -g -ansi -Wall -pedantic translating.o printings.o parsing.o data_structures.o assembling.o auxilary.o main.c -o main

printings.o: printings.c printings.h main.h 
	gcc -c -ansi -Wall -pedantic printings.c -o printings.o

parsing.o: parsing.c parsing.h data_structures.o data_structures.h auxilary.o auxilary.h main.h 
	gcc -c -ansi -Wall -pedantic parsing.c -o parsing.o

translating.o: translating.c translating.h main.h auxilary.o auxilary.h
	gcc -c -ansi -Wall -pedantic  translating.c -o translating.o

assembling.o: main.h assembling.c translating.h translating.o printings.h printings.o data_structures.h data_structures.o auxilary.h auxilary.o
	gcc -c -ansi -Wall -pedantic  assembling.c -o assembling.o

auxilary.o: main.h auxilary.c
	gcc -c -ansi -Wall -pedantic auxilary.c -o auxilary.o

data_structures.o:data_structures.h main.h data_structures.c
	gcc -c -ansi -Wall -pedantic data_structures.c -o data_structures.o
CFLAGS= -ansi -Wall -pedantic
