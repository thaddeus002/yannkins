
ifndef CC
CC=gcc
endif

OBJS=data.o ../xml/xml.o ../csv/csv.o ../csv/utils.o

data.o: data.c data.h
	$(CC) -Wall -c data.c

../xml/xml.o:
	make -C ../xml xml.o

../csv/csv.o:
	make -C ../csv csv.o

../csv/utils.o:
	make -C ../csv utils.o

test_data: test_data.c $(OBJS)
	$(CC) -Wall -o test_data test_data.c $(OBJS)

test: test_data
	# Are there error while analyse data?
	./test_data
	rm test_log.output.csv

convert_log: convert_svn_log_from_xml_to_csv.c $(OBJS)
	$(CC) -Wall -o convert_log convert_svn_log_from_xml_to_csv.c $(OBJS)

clean:
	rm -f *.o *~

mrproper: clean
	rm -f test_data convert_log
