#
#  Makefile
#
#  Copyright (c) 2021 by Daniel Kelley
#

DEBUG ?= -g -O0

PREFIX ?= /usr/local

# address thread undefined etc.
ifneq ($(SANITIZE),)
DEBUG += -fsanitize=$(SANITIZE)
endif

INC := -I$(PREFIX)/include
CPPFLAGS := $(INC) -MP -MMD

WARN := -Wall
WARN += -Wextra
WARN += -Wdeclaration-after-statement
WARN += -Werror
CFLAGS := $(WARN) $(DEBUG) -fPIC

LDFLAGS := $(DEBUG) -L. -L$(PREFIX)/lib
LDLIBS := -lpvocf -lm

PVOCTOOL_SRC := pvoctool.c
PVOCTOOL_OBJ := $(PVOCTOOL_SRC:%.c=%.o)
PVOCTOOL_DEP := $(PVOCTOOL_SRC:%.c=%.d)
OBJ := $(PVOCTOOL_OBJ)
DEP := $(PVOCTOOL_DEP)

PROG := pvoctool

.PHONY: all install uninstall clean check

all: $(PROG)

pvoctool: $(PVOCTOOL_SRC)

install: $(PROG)
	install -p -m 755 $(PROG) $(PREFIX)/bin

uninstall:
	-rm -f $(PREFIX)/bin/$(PROG)

chirp.wav: test/chirp.csd
	csound $<

chirp.pvx: chirp.wav
	csound -U pvanal -n 4096 -h 1024 $< $@

check: $(PROG) chirp.pvx
	./$+

clean:
	-rm -f $(PROG)	$(OBJ) $(DEP) \
		chirp.wav chirp.pvx


-include $(DEP)
