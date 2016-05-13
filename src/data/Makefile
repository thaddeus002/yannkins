
ifndef CC
CC=gcc
endif

OBJS=data.o ../xml/xml.o ../csv/csv.o

data.o: data.c data.h
	$(CC) -Wall -c data.c

../xml/xml.o:
	make -C ../xml xml.o

../csv/csv.o:
	make -C ../csv csv.o

test_data: test_data.c $(OBJS)
	$(CC) -Wall -o test_data test_data.c $(OBJS)

convert_log: convert_svn_log_from_xml_to_csv.c $(OBJS)
	$(CC) -Wall -o convert_log convert_svn_log_from_xml_to_csv.c $(OBJS)

clean:
	rm -f *.o *~

mrproper: clean
	rm -f test_data
