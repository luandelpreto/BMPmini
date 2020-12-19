# Color
GREEN=$(shell tput -Txterm setaf 2)
RESETCOLOR=$(shell tput -Txterm sgr0)

build: build_msg src/$(LIB).o src/$(LIB).c
	ar rcs src/$(LIBNAME) src/$(LIB).o
	ranlib src/$(LIBNAME)

debug/$(LIBNAME): src/$(LIB).o src/$(LIB).c
	$(CC) $(CFLAGS) $(CDEBUG) -c src/$(LIB).c $<
	ar rcs src/$(LIBNAME) $<
	ranlib src/$(LIBNAME)

install: src/$(LIBNAME)
	install -d $(DESTDIR)$(PREFIX)/lib/
	install -m 644 src/$(LIBNAME) $(DESTDIR)$(PREFIX)/lib
	install -d $(DESTDIR)$(PREFIX)/include/
	install -m 644 src/$(LIB).h $(DESTDIR)$(PREFIX)/include/

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/lib/$(LIBNAME)
	rm -f $(DESTDIR)$(PREFIX)/include/$(LIB).h
	-rmdir $(DESTDIR)$(PREFIX)/lib > /dev/null 2>&1       # remove if empty
	-rmdir $(DESTDIR)$(PREFIX)/include > /dev/null 2>&1

src/$(LIB).o: src/$(LIB).h src/$(LIB)def.h

clean/$(LIBNAME):
	-rm -fv src/*.o

build_msg:
	@printf "$(GREEN)#\n# [$(LIB)]: Building $(LIB)...\n#\n$(RESETCOLOR)"

.PHONY: build_msg
