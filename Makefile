# Build example and create the distribution tar ball
CC=cc
CFLAGS=-pedantic -W -Wall
DEBUGFLAG=-g
LFLAGS=-static -lBMPmini
EXEC=example
VPATH=src
# Distribution info
version=0.0.1
tarname=BMPmini
distdir=$(tarname)-$(version)

all: libBMPmini.a $(EXEC).c
	$(CC) $(CFLAGS) -c $(EXEC).c
	$(CC) $(CFLAGS) $(EXEC).o -o $(EXEC) $(LFLAGS)

debug: libBMPmini.a $(EXEC).c
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
	cd $(distdir) && $(MAKE) clean
	rm -rf $(distdir)
	@echo "*** Package $(distdir).tar.gz is ready for distribution"

FORCE:
	-rm -f $(distdir).tar.gz
	rm -rf $(distdir)

.PHONY: FORCE all clean dist distcheck

clean:
	-rm -v $(EXEC).o
	-rm -v $(EXEC)
	-rm -v test/BMP_generate
