## Makefile for libpidontrol

## Edit thise variables to configure installation path
# root to install to for `make install'
PREFIX := /usr/local
# if you use stow, root of your stow package directory
STOWBASE := /usr/local/stow
# if you use stow, a name for this stow package

## Mabe you care about these
CC := gcc
CXXFLAGS := -g -fPIC -I./include -Wall
OCXXFLAGS := -O3 -funroll-loops -I./include


## You probably don't care aboute these
VERSION := 0.0.1
PROJECT := libpidcontrol

STOWDIR := $(PROJECT)-$(VERSION)
STOWPREFIX := $(STOWBASE)/$(STOWDIR)

DISTPATH := $(HOME)/prism/tarballs
DOXPATH := $(HOME)/prism/public_html/dox

PLATFORM := $(shell uname -s)

## OS X uses a different library extension, play nice
ifeq ($(PLATFORM),Darwin)
PRODUCT := $(PROJECT).dylib
LDFLAGS := -lc
else
PRODUCT := $(PROJECT).so
endif

HEADERS := $(addprefix include/pidcontrol/,pidcontrol.h)



.PHONY: doc default clean stow


default: $(PRODUCT)

.c.o:
	$(CC) $(CXXFLAGS) -c $<

pid_test: pid_test.c
	$(CC) $(CXXFLAGS) -o $@ $< -lpidcontrol

$(PRODUCT): pidcontrol.o $(HEADERS)
	$(CC) -shared -Wl,-soname,$@ -o $@ $< $(LDFLAGS)

clean:
	rm -fv *.o *.so *.dylib pid_test

distclean: clean
	rm -rf doc

doc:
	doxygen

docul: doc
	cp -Tr doc/html $(DOXPATH)/$(PROJECT)

dist: distclean
	cd .. &&               \
	tar --exclude=.svn --lzma -cvf $(DISTPATH)/$(PROJECT)-$(VERSION).tar.lzma $(PROJECT)

stow: $(PRODUCT)
	mkdir -p $(STOWPREFIX)/include/ssdmu
	mkdir -p $(STOWPREFIX)/lib/
	install --mode 755 $(PRODUCT) $(STOWPREFIX)/lib
	install --mode 644 include/pidcontrol/*.h $(STOWPREFIX)/include/pidcontrol
	cd $(STOWBASE) && stow $(STOWDIR)


install: $(PRODUCT)
	mkdir -p $(PREFIX)/include/pidcontrol
	mkdir -p $(PREFIX)/lib/
	install --mode 755 $(PRODUCT) $(PREFIX)/lib
	install --mode 644 include/pidcontrol/*.h $(PREFIX)/include/pidcontrol
