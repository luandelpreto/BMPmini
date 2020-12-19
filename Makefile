# Build example and create the distribution tar ball
CC=cc
CFLAGS=-pedantic -W -Wall
DEBUGFLAG=-g
LFLAGS=-static -lBMPmini
EXEC=example
LIB=BMPmini
LIBNAME=libBMPmini.a
VPATH=src
# Distribution info
version=0.0.1
tarname=$(LIB)
distdir=$(tarname)-$(version)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif
export PREFIX
export CC
export CFLAGS
export OPTIMIZE
export LIB
export LIBNAME

all: $(LIBNAME) $(EXEC).c
	$(CC) $(CFLAGS) -c $(EXEC).c
	$(CC) $(CFLAGS) $(EXEC).o -o $(EXEC) $(LFLAGS)

debug: $(LIBNAME) $(EXEC).c
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c $(EXEC).c
	$(CC) $(DEBUGFLAG) $(CFLAGS) $(EXEC).o -o $(EXEC) $(LFLAGS)

###############################
# Distribution
###############################

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir): FORCE
	mkdir -p $(distdir)/src
	cp -r Makefile LICENSE README.md example.c images/examples test/ $(distdir)
	cp -r src/Makefile src/BMPmini* $(distdir)/src

distcheck: $(distdir).tar.gz
	gzip -cd $(distdir).tar.gz | tar xvf -
	-cd $(distdir)/src && $(MAKE)
	cd $(distdir) && $(MAKE) all
	cd $(distdir) && $(MAKE) PREFIX=$${PWD}/_inst install
	cd $(distdir) && $(MAKE) PREFIX=$${PWD}/_inst uninstall
	@remaining="`find $(distdir)/_inst -type f | wc -l`";             \
	if test "$${remaining}" -ne 0; then                               \
		echo "*** $${remaining} file(s) remaining in stage directory!"; \
		exit 1;                                                         \
	fi
	cd $(distdir) && $(MAKE) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is ready for distribution"

FORCE:
	-rm -f $(distdir).tar.gz
	-rm -rf $(distdir)

build:
	cd src && $(MAKE) $@

clean:
	-rm $(EXEC).o
	-rm $(EXEC)
	-rm test/BMP_generate
	-cd src && $(MAKE) $@

install uninstall:
	cd src && $(MAKE) $@

.PHONY: FORCE all debug clean dist distcheck install uninstall

