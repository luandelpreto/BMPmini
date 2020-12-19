# Build example and create the distribution tar ball
CFLAGS=-pedantic -W -Wall -O2
CDEBUG=-g -O0
LDFLAGS=-static -lBMPmini
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

all: $(LIBNAME) $(EXEC).c
	$(CC) $(CFLAGS) -c $(EXEC).c
	$(CC) $(CFLAGS) $(EXEC).o -o $(EXEC) $(LDFLAGS)

debug: $(LIBNAME) $(EXEC).c
	$(CC) $(CFLAGS) $(CDEBUG) -c $(EXEC).c
	$(CC) $(CDEBUG) $(CFLAGS) $(EXEC).o -o $(EXEC) $(LDFLAGS)

###############################
# Distribution
###############################

dist: $(distdir).tar.gz

$(distdir).tar.gz: $(distdir)
	tar chof - $(distdir) | gzip -9 -c > $@
	rm -rf $(distdir)

$(distdir): FORCE
	mkdir -p $(distdir)/src
	cp -r Makefile LICENSE README.md example.c test/ $(distdir)
	mkdir -p $(distdir)/images/ && cp -r images/examples $(distdir)/images/
	cp -r src/Makefile src/BMPmini* $(distdir)/src

distcheck: $(distdir).tar.gz
	gzip -cd $(distdir).tar.gz | tar xvf -
	-cd $(distdir) && $(MAKE) build
	cd $(distdir) && $(MAKE) all
	cd $(distdir) && $(MAKE) DESTDIR=$${PWD}/_inst install
	cd $(distdir) && $(MAKE) DESTDIR=$${PWD}/_inst uninstall
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

clean: clean/$(LIBNAME)
	-rm $(EXEC).o
	-rm $(EXEC)
	-rm test/BMP_generate

include src/Makefile

.PHONY: FORCE all build debug clean dist distcheck install uninstall
