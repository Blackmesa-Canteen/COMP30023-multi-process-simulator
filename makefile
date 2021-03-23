# Created by Xiaotian Li on Mar 23, 2021
# A make file for Project 1
allocate: main.o linkedlist.o sim.o process.o readFile.o myHashtable.o
	gcc -o allocate main.o linkedlist.o sim.o process.o readFile.o myHashtable.o -lm

sim.o: sim.c sim.h linkedlist.h myHashtable.h readFile.h process.h
	gcc -c -Wall sim.c

process.o: process.c process.h myHashtable.h
	gcc -c -Wall process.c

readFile.o: readFile.c readFile.h
	gcc -c -Wall readFile.c

myHashtable.o: myHashtable.c myHashtable.h
	gcc -c -Wall myHashtable.c

main.o: main.c sim.h linkedlist.h
	gcc -c -Wall main.c

linkedlist.o: linkedlist.c linkedlist.h
	gcc -c -Wall linkedlist.c

clean:
	rm -f allocate *.o