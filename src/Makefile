CC=gcc
#CFLAGS=-g -O2 -Wall -pedantic -std=c99 -c
CFLAGS=-D_GNU_SOURCE -Wall -Wextra -Wformat=2 -Wswitch-default -Wcast-align \
				-Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Winline \
				-Wundef -Wnested-externs -Wcast-qual -Wshadow -Wwrite-strings \
				-Wconversion -Wunreachable-code -Wstrict-aliasing=2 -ffloat-store \
				-fno-common -fstrict-aliasing -std=c99 -pedantic -O0 -ggdb3 -pg -c
LDFLAGS=-L/usr/lib -lcurses -lm -lmenu
SRCFILES=$(wildcard *.c)
OBJFILES=$(patsubst %.c,%.o,$(SRCFILES))
PROGNAME=ascii-combat

.PHONY: clean

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(LDFLAGS) -o $(PROGNAME) $(OBJFILES)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROGNAME) $(OBJFILES)

install: $(PROGNAME)
	cp -v $(PROGNAME) /usr/bin/$(PROGNAME)

uninstall:
	rm -v /usr/bin/$(PROGNAME)
