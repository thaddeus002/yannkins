
ifndef CC
CC=gcc
endif

xml.o: xml.c xml.h
	$(CC) -Wall -c xml.c

../csv/utils.o:
	make -C ../csv utils.o

clean:
	rm -f *.o *~

test: test_xml
	./test_xml
	rm -f *.tmp
	rm -f test_xml

test_xml: test_xml.c xml.o ../csv/utils.o
	$(CC) -Wall -o test_xml test_xml.c xml.o ../csv/utils.o

