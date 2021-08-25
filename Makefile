# author: @prokoprandacek
.PHONY : all clean deepclean umka

#CROSS_CC = x86_64-w64-mingw32-gcc

LDLIBS = -lm -lX11 -ldl -lGL -lpthread

CFLAGS ?= -O3 -pipe

CFLAGS += -Ilib/stb/ \
		  -Ilib/rawdraw/ \
		  -Ilib/miniaudio/

DEFS = -DCNFGOGL -DUMKA_STATIC

WARNS = \
		-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
		-Wno-old-style-declaration -Wno-implicit-fallthrough

CFLAGS += $(WARNS) $(DEFS)

TARGET = tophat

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

include Makeumka
include src/Makefile

deepclean:: clean

clean::
	$(RM) $(TARGET)


