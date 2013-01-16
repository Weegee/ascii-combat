SRCDIR=src/

all: ascii-combat

ascii-combat:
	$(MAKE) -C $(SRCDIR)

clean:
	$(MAKE) clean -C $(SRCDIR)

install:
	$(MAKE) install -C $(SRCDIR)

uninstall:
	$(MAKE) uninstall -C $(SRCDIR)
