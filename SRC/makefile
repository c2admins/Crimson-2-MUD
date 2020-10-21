# Makefile for Crimson 2 MUD
#
# Written by Relic (much cleaner)


# C compiler
CC = /usr/bin/gcc

# SVN version header
VERSIONHEADER = repversion.h

# Prog name
PROGNAME = crimsond

# Compiler flags:
CFLAGS =  -Wall -g -Wformat=0 -w

# Linker flags.  (-lcrypt isn't needed on OS X)
LFLAGS = -lcrypt

OBJFILES = admin.o alias.o assign.o attack.o auction.o ban.o boards.o \
	channel.o comm.o constants.o create.o citadel.o clans.o bounty.o \
	db.o fight.o find.o forbidden.o handler.o history.o hunt.o \
	info.o limits.o magic1.o magic2.o magic3.o magic4.o mobact.o \
	modify.o move.o obj1.o obj2.o objact.o other.o parser.o \
	pictures.o proc1.o proc2.o quest.o rent.o skills.o mail.o \
	security.o shop.o signals.o silver.o smobs.o social.o special.o \
	spells.o spells1.o spells2.o spells3.o spells4.o vehicle.o utility.o \
	weather.o wiz1.o wiz2.o xanth.o logview.o halfling.o swamp.o qelves.o

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
	echo "#define C2_VERSION \"180\"" > $(VERSIONHEADER)
	$(MAKE) program

all:
	$(MAKE) prod
	$(MAKE) dev
	$(MAKE) hell
	$(MAKE) test

depend:
	echo "#define C2_VERSION \"180\"" > $(VERSIONHEADER)
	$(CC) -MM *.c > depend

-include depend
