
ifndef CC
CC=gcc
endif

xml.o: xml.c xml.h
	$(CC) -Wall -c xml.c

clean:
	rm -f *.o *~

test: test_xml
	./test_xml
	rm -f *.tmp
	rm -f test_xml

test_xml: xml.o test_xml.c
	$(CC) -Wall -o test_xml test_xml.c xml.o

