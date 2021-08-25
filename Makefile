# author: @prokoprandacek
.PHONY : all clean deepclean umka

CFLAGS ?= -O3 -pipe
LDLIBS += -lm -lX11 -ldl -lGL -lpthread

CFLAGS += -Ilib/stb/ \
		  -Ilib/rawdraw/ \
		  -Ilib/miniaudio/

DEFS = -DCNFGOGL -DUMKA_STATIC

WARNS = \
		-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
		-Wno-old-style-declaration -Wno-implicit-fallthrough

CFLAGS += $(WARNS) $(DEFS)

TARGET = tophat
VERSION = v0.3-$(shell git rev-parse --short HEAD)

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

include Makeumka
include src/Makefile

deepclean:: clean

clean::
	$(RM) $(TARGET)


