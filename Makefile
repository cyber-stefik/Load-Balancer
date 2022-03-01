# Copyright 2021 Stefanita Ionita
CC=gcc
CFLAGS=-Wall -Wextra -std=c99
lb=load_balancer
cdll=CircularDoublyLinkedList
ht=Hashtable
ll=LinkedList
sv=server

.PHONY: build clean

build: tema2

tema2: main.o $(cdll).o $(ht).o $(ll).o $(lb).o $(sv).o
	$(CC) $^ -o tema2

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(cdll).o: $(cdll).c $(cdll).h
	$(CC) $(CFLAGS) $^ -c

$(ht).o: $(ht).c $(ht).h
	$(CC) $(CFLAGS) $^ -c

$(ll).o: $(ll).c $(ll).h
	$(CC) $(CFLAGS) $^ -c

$(lb).o: $(lb).c $(lb).h
	$(CC) $(CFLAGS) $^ -c

$(sv).o: $(sv).c $(sv).h
	$(CC) $(CFLAGS) $^ -c

pack: 
	zip -FSr 314CA_IONITA_Stefanita_tema2.zip README Makefile *.c *.h

clean:
	rm -f *.o tema2 *.h.gch

.PHONY: pack clean run