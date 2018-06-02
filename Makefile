
# Directory where install the binaries
ifndef PREFIX
	PREFIX=/usr/local
endif

# Yannkins's working directory
ifndef YANNKINS_HOME
	YANNKINS_HOME=/var/yannkins
endif

all: src/cree_page src/convert_log src/tache

src/cree_page src/convert_log src/tache:
	make YANNKINS_HOME=$(YANNKINS_HOME) -C src
	@printf "\nYannkins working directory is fixed to %s\n" "$(YANNKINS_HOME)"
	@printf "Use the environment variable YANNKINS_HOME to override this setting\n"
	@printf "Then type \"make install\" to install yannkins\n\n"

clean:
	make -C src clean

mrproper:
	make -C src mrproper

install: src/cree_page src/convert_log src/tache
	install -d $(PREFIX)/bin
	install -m 755 src/cree_page src/convert_log src/tache $(PREFIX)/bin/
	install -m 755 bin/*.sh $(PREFIX)/bin/
	install -d $(YANNKINS_HOME)/www
	install -m 644 www/*.html $(YANNKINS_HOME)/www/
	install -d $(YANNKINS_HOME)/www/icons $(YANNKINS_HOME)/www/style
	install -m 644 www/icons/* $(YANNKINS_HOME)/www/icons/
	install -m 644 www/style/* $(YANNKINS_HOME)/www/style
	install -d $(YANNKINS_HOME)/log
	ln -s ../log $(YANNKINS_HOME)/www
	@printf "\nInstallation of Yannkins completed\n"
	@printf "Yannkins working dir is %s\n\n" "$(YANNKINS_HOME)"

tests:
	make -C src tests

documentation:
	doxygen
