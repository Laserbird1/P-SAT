
CC=gcc
LIBDIR=/usr/local/lib
INCDIR=/usr/local/sgb/include
LDFLAGS=-L$(LIBDIR) -lgb
CCFLAGS=-g -Wall -I$(INCDIR)

DESTDIR=../bin
SRCDIR=.
FILE=$(wildcard $(SRCDIR)/*.w)

EXEC=$(FILE:$(SRCDIR)/%.w=$(DESTDIR)/%)
SRC=$(FILE:%.w=%.c)

.PHONY: all test

all: $(EXEC)

$(DESTDIR)/%: $(SRCDIR)/%.c
	$(CC) $(CCFLAGS) $(LDFLAGS) $< -o $@

$(SRCDIR)/%.c:$(SRCDIR)/%.w
	ctangle $(CWEBFILE)

clean:
	rm -f $(EXEC) $(SRC)

install: install_cweb install_sgb

install_sgb:
	@echo "-- Fetching file from stanford website in order to make a c library widely used by knuth" 
	curl -o sgb.tar.gz ftp://ftp.cs.stanford.edu/pub/sgb/sgb.tar.gz
	mkdir sgb
	tar -x -C sgb -f sgb.tar.gz
	@echo
	@echo "-- Now follow sgb installation guide"


install_cweb:
	@echo "-- Fetching cweb utilities"
	curl cweb.tar.gz ftp://ftp.cs.stanford.edu/pub/cweb/cweb.tar.gz
	mkdir cweb
	tar -x -C cweb -f cweb.tar.gz
	@echo
	@echo "-- Now follow cweb installation guide"

test:
	@echo FILES := $(FILE)
	@echo
	@echo SRC := $(SRC)
	@echo
	@echo EXEC := $(EXEC)
