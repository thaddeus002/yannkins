
#ifndef PREFIX
	PREFIX=/usr/local
#endif

OBJS=cree_page.o project.o csv/csv.o html/html.o
CFLAGS="-I$(PREFIX)/include"

all:cree_page convert_log

%.c: %.h

csv/csv.o:
	make -C csv

html/html.o:
	make -C html

%.o: %.c
	gcc -c $(CFLAGS) $<

cree_page: $(OBJS)
	gcc -o cree_page $(OBJS)

convert_log:
	make -C data convert_log
	mv data/convert_log .

clean:
	rm -f $(OBJS) cree_page.o

mrproper: clean
	rm -f cree_page
