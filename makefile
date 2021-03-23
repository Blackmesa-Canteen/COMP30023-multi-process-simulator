# Created by Xiaotian Li on Mar 23, 2021
# A make file for Project 1
allocate: main.o linkedlist.o sim.o process.o readFile.o hashtable.o
	gcc -o allocate main.o linkedlist.o sim.o process.o readFile.o hashtable.o -lm

sim.o: sim.c sim.h linkedlist.h hashtable.h readFile.h process.h
	gcc -c -Wall sim.c

process.o: process.c process.h hashtable.h
	gcc -c -Wall process.c

readFile.o: readFile.c readFile.h
	gcc -c -Wall readFile.c

hashtable.o: hashtable.c hashtable.h
	gcc -c -Wall hashtable.c

main.o: main.c sim.h linkedlist.h
	gcc -c -Wall main.c

linkedlist.o: linkedlist.c linkedlist.h
	gcc -c -Wall linkedlist.c

clean:
	rm -f allocate *.o