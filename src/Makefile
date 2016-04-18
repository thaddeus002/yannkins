
#ifndef PREFIX
	PREFIX=/usr/local
#endif

OBJS=cree_page.o project.o
CFLAGS="-I$(PREFIX)/include"

all:cree_page

%.c: %.h

%.o: %.c
	gcc -c $(CFLAGS) $<

cree_page: $(OBJS)
	gcc -o cree_page $(OBJS) -L$(PREFIX)/lib -lcsv -lhtml

clean:
	rm -f $(OBJS) cree_page.o

mrproper: clean
	rm -f cree_page
