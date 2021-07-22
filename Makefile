#
#  Makefile
#
#  Copyright (c) 2021 by Daniel Kelley
#

CC = h5cc

DEBUG ?= -g

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
LDLIBS := -lpvocf -lriffr -lhdf5_hl -lhdf5 -lm

PVOCTOOL_SRC := pvoctool.c
PVOCTOOL_SRC += pvoctool_get_data.c
PVOCTOOL_SRC += pvoctool_info.c
PVOCTOOL_SRC += pvoctool_hdf5.c
PVOCTOOL_OBJ := $(PVOCTOOL_SRC:%.c=%.o)
PVOCTOOL_DEP := $(PVOCTOOL_SRC:%.c=%.d)
OBJ := $(PVOCTOOL_OBJ)
DEP := $(PVOCTOOL_DEP)

PROG := pvoctool

.PHONY: all install uninstall clean check

all: $(PROG)

pvoctool: $(PVOCTOOL_OBJ)

install: $(PROG)
	install -p -m 755 $(PROG) $(PREFIX)/bin

uninstall:
	-rm -f $(PREFIX)/bin/$(PROG)

chirp.wav: test/chirp.csd
	csound $<

chirp.pvx: chirp.wav
	csound -U pvanal -n 4096 -h 1024 $< $@

check: $(PROG) chirp.pvx
	./$(PROG) info chirp.pvx
	./$(PROG) hdf5 chirp.pvx chirp.h5
	h5dump chirp.h5 > chirp.h5.txt
clean:
	-rm -f $(PROG)	$(OBJ) $(DEP) \
		chirp.wav chirp.pvx chirp.h5 chirp.h5.txt

-include $(DEP)
