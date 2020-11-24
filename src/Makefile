CC=gcc
CFLAGS=-pedantic -W -Wall
OPTIMIZATION=-O2
DEBUGFLAG=-g
LIB=BMPmini

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

build: $(LIB).o $(LIB).c
	$(CC) $(OPTIMIZATION) -c $(LIB).c
	ar rcs libBMPmini.a $(LIB).o
	ranlib libBMPmini.a

debug: $(LIB).o $(LIB).c
	$(CC) $(DEBUGFLAG) -c $(LIB).c $(LIB).o
	ar rcs libBMPmini.a $(LIB).o
	ranlib libBMPmini.a

install: libBMPmini.a
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libBMPmini.a $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 BMPmini.h $(DESTDIR)$(PREFIX)/include/

$(LIB).o: $(LIB).h $(LIB).h

.PHONY: clean

clean:
	rm -rfv *.o
