
#ifndef PREFIX
	PREFIX=/usr/local
#endif


all:
	make -C src

clean:
	make -C src clean
	
mrproper:
	make -C src mrproper

install: all
	install -m 755 src/cree_page src/convert_log $(PREFIX)/bin/
	install -m 755 bin/*.sh $(PREFIX)/bin/
	@echo "Installation of Yannkins completed"
	@echo "Define YANNKINS_HOME environment variable"
