# Color
GREEN=$(shell tput -Txterm setaf 2)
RESETCOLOR=$(shell tput -Txterm sgr0)

build: build_msg $(LIB).o $(LIB).c
	$(CC) $(OPTIMIZE) -c $(LIB).c
	ar rcs $(LIBNAME) $(LIB).o
	ranlib $(LIBNAME)

debug: $(LIB).o $(LIB).c
	$(CC) $(DEBUGFLAG) -c $(LIB).c $<
	ar rcs $(LIBNAME) $<
	ranlib $(LIBNAME)

install: $(LIBNAME)
	install -d $(PREFIX)/lib/
	install -m 644 $(LIBNAME) $(PREFIX)/lib
	install -d $(PREFIX)/include/
	install -m 644 $(LIB).h $(PREFIX)/include/

uninstall:
	rm -f $(PREFIX)/lib/$(LIBNAME)
	rm -f $(PREFIX)/include/$(LIB).h
	-rmdir $(PREFIX)/lib > /dev/null 2>&1       # remove if empty
	-rmdir $(PREFIX)/include > /dev/null 2>&1

$(LIB).o: $(LIB).h $(LIB)def.h

clean:
	-rm -fv *.o

build_msg:
	@printf "$(GREEN)#\n# [$(LIB)]: Building $(LIB)...\n#\n$(RESETCOLOR)"

.PHONY: build debug build_msg clean install uninstall
