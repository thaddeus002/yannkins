
OBJS=cree_page.o project.o log_analyse.o csv/csv.o csv/utils.o xml/xml.o html/html.o logger.o
CFLAGS=

ifdef YANNKINS_HOME
CFLAGS+=-DYANNKINS_HOME=\"$(YANNKINS_HOME)\"
endif

all: cree_page convert_log tache

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
	gcc $(CFLAGS) -o cree_page $(OBJS)

tache: tache.c
	gcc $(CFLAGS) -o tache tache.c logger.o

convert_log:
	make -C data convert_log
	mv data/convert_log .

tests:
	make -C xml test
	make -C data test

clean:
	rm -f $(OBJS)
	make -C data clean

mrproper: clean
	rm -f cree_page convert_log tache

.PHONY: clean mrproper tests
