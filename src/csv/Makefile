
ifndef CC
CC=gcc
endif

all: utils.o csv.o

utils.o: utils.c utils.h
	$(CC) -Wall -c utils.c

csv.o: csv.c csv.h
	$(CC) -Wall -c csv.c

clean:
	rm -f *.o *~

.PHONY: clean
