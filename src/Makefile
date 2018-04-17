
#ifndef PREFIX
	PREFIX=/usr/local
#endif

OBJS=cree_page.o project.o log_analyse.o csv/csv.o csv/utils.o xml/xml.o html/html.o logger.o
CFLAGS="-I$(PREFIX)/include"

all:cree_page convert_log

%.c: %.h

csv/utils.o:
	make -C csv

csv/csv.o:
	make -C csv

html/html.o:
	make -C html

xml/xml.o:
	make -C xml

%.o: %.c
	gcc -c $(CFLAGS) $<

cree_page: $(OBJS)
	gcc -o cree_page $(OBJS)

tache: tache.c
	gcc -o tache tache.c

convert_log:
	make -C data convert_log
	mv data/convert_log .

tests:
	make -C xml test
	make -C data test

clean:
	rm -f $(OBJS) cree_page.o data/convert_log.o

mrproper: clean
	rm -f cree_page convert_log tache

.PHONY: clean mrproper tests
