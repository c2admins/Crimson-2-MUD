# Makefile for Crimson 2 MUD
#
# Written by Relic (much cleaner)


# C compiler
CC = /usr/bin/gcc

# SVN version header
VERSIONHEADER = repversion.h

# Prog name
PROGNAME = rebuildplr

# Compiler flags:
CFLAGS =  -Wall -g -Wformat=0 -w

# Linker flags.  (-lcrypt isn't needed on OS X)
LFLAGS = -lcrypt

OBJFILES = rebuildplr.o


default: local

clean:
	rm -f *.o $(VERSIONHEADER) depend $(PROGNAME)

program: $(OBJFILES)
	$(CC) -o $(PROGNAME) $(OBJFILES) $(LFLAGS)

prod: local
	cp ./$(PROGNAME) ~/c2/prod/bin/$(PROGNAME)

dev: local
	cp ./$(PROGNAME) ~/c2/dev/bin/$(PROGNAME)

hell: local
	cp ./$(PROGNAME) ~/c2/hell/bin/$(PROGNAME)

test: local
	cp ./$(PROGNAME) ~/c2/test/bin/$(PROGNAME)

local:
	echo "#define C2_REBUILDPLR \"100\"" > $(VERSIONHEADER)
	$(MAKE) program

all:
	$(MAKE) prod
	$(MAKE) dev
	$(MAKE) hell
	$(MAKE) test

depend:
	echo "#define C2_REBUILDPLR \"100\"" > $(VERSIONHEADER)
	$(CC) -MM *.c /home/ubuntu/c2/src/*.c > depend

-include depend
