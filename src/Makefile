#CC=gcc
CC=clang
# Development CFLAGS (clang)
CFLAGS=-D_GNU_SOURCE -std=c99 -ggdb3 -O0 -Weverything -Wno-padded -pedantic \
			 -Wno-format-nonliteral -pedantic-errors -c
# Development CFLAGS (gcc)
#CFLAGS=-D_GNU_SOURCE -std=c99 -ggdb3 -O0 -Werror -Wfatal-errors -Wall \
	-pedantic-errors -Wextra -Wformat=2 -Wswitch-default -Wswitch-enum \
	-Wunused-parameter -Wstrict-overflow=5 -Wfloat-equal -Wundef -Wshadow \
	-Wbad-function-cast -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion \
	-Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes \
	-Wmissing-declarations -Wmissing-noreturn -Wredundant-decls -Wnested-externs \
	-Wunreachable-code -Winline -Wstack-protector -fstrict-aliasing -fno-common \
	-ffloat-store -fno-omit-frame-pointer -c
# Release CFLAGS
#CFLAGS=-D_GNU_SOURCE -std=c99 -O2 -Wall -c
LDFLAGS=-lcurses -lm -lmenu
SRCFILES=$(wildcard *.c)
OBJFILES=$(patsubst %.c,%.o,$(SRCFILES))
PROGNAME=ascii-combat

.PHONY: clean

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) -o $(PROGNAME) $(OBJFILES) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(PROGNAME) $(OBJFILES)

install: $(PROGNAME)
	cp -v $(PROGNAME) /usr/bin/$(PROGNAME)

uninstall:
	rm -v /usr/bin/$(PROGNAME)
