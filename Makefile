CC=gcc
CFLAGS=-pedantic -W -Wall
DEBUGFLAG=-g
LFLAGS=-static -lBMPmini
EXEC=example

all: src/libBMPmini.a $(EXEC).c
	$(CC) $(CFLAGS) -c $(EXEC).c
	$(CC) $(CFLAGS) $(EXEC).o -o $(EXEC) $(LFLAGS)

debug: libBMPmini_debug.a src/testeBMP.c
	$(CC) $(CFLAGS) $(DEBUGFLAG) -c $(EXEC).c
	$(CC) $(CFLAGS) $(LFLAGS) $(DEBUGFLAG) libBMPmini.a $(EXEC).o -o $(EXEC)

.PHONY: clean

clean:
	rm -v $(EXEC).o
	rm -v $(EXEC)
