CC=gcc
CFLAGS=-pedantic -W -Wall
OPTIMIZE=-O2
DEBUGFLAG=-g
LIB=BMPmini

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

build: build_msg $(LIB).o $(LIB).c
	$(CC) $(OPTIMIZE) -c $(LIB).c
	ar rcs libBMPmini.a $(LIB).o
	ranlib libBMPmini.a

debug: $(LIB).o $(LIB).c
	$(CC) $(DEBUGFLAG) -c $(LIB).c $(LIB).o
	ar rcs libBMPmini.a $(LIB).o
	ranlib libBMPmini.a

.PHONY: install
install: libBMPmini.a
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 libBMPmini.a $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 BMPmini.h $(DESTDIR)$(PREFIX)/include/

$(LIB).o: $(LIB).h $(LIB).h

.PHONY: clean
clean:
	rm -fv *.o

.PHONY: build_msg
build_msg:
	@printf "#\n# [$(LIB)]: Building $(LIB)...\n#\n"
