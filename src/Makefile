CC=gcc
CFLAGS=-pedantic -W -Wall
OPTIMIZE=-O2
DEBUGFLAG=-g
LIB=BMPmini
LIBNAME=libBMPmini.a
# Color
GREEN=$(shell tput -Txterm setaf 2)
RESETCOLOR=$(shell tput -Txterm sgr0)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif

build: build_msg $(LIB).o $(LIB).c
	$(CC) $(OPTIMIZE) -c $(LIB).c
	ar rcs $(LIBNAME) $(LIB).o
	ranlib $(LIBNAME)

debug: $(LIB).o $(LIB).c
	$(CC) $(DEBUGFLAG) -c $(LIB).c $<
	ar rcs $(LIBNAME) $<
	ranlib $(LIBNAME)

.PHONY: install
install: $(LIBNAME)
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 $(LIBNAME) $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 BMPmini.h $(DESTDIR)$(PREFIX)/include/

$(LIB).o: $(LIB).h $(LIB)def.h

.PHONY: clean
clean:
	rm -fv *.o

.PHONY: build_msg
build_msg:
	@printf "$(GREEN)#\n# [$(LIB)]: Building $(LIB)...\n#\n$(RESETCOLOR)"
