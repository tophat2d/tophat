.PHONY : all clean deepclean umka bulk

PLATFORM := $(shell uname -s)
SHORT_PLATFORM:= $(shell (X=`uname -s`; echo $${X:0:10}))

ifeq ($(PLATFORM), Linux)
	LDLIBS += -lm -lX11 -ldl -lGL -lpthread -fPIC
	CROSS_CC=x86_64-w64-mingw32-gcc
	CROSS_FLAGS = -lopengl32 -lgdi32 -DNO_OPENGL_HEADERS
	TARGET=tophat
	UMKA_BIN = ./lib/umka/build/umka
else
	ifeq ($(SHORT_PLATFORM), MINGW64_NT)
	LDLIBS += -lm -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread
	TARGET=tophat.exe
	DEFS += -DNO_OPENGL_HEADERS
	UMKA_BIN = ./lib/umka/build/umka.exe
endif
endif

CFLAGS ?= -s -O3 -pipe
CFLAGS += -Ilib/stb/ \
	  -Ilib/rawdraw/ \
	  -Ilib/miniaudio/ \
	  -Ilib/umprof/

DEFS += -DUMKA_STATIC -DUMKA_EXT_LIBS
WARNS = -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
	-Wno-old-style-declaration -Wno-implicit-fallthrough

CFLAGS += $(WARNS) $(DEFS)

all: $(UMKA_LIB) $(TARGET)

run: $(TARGET)
	./$(TARGET)

include Makeumka
include src/Makefile

deepclean:: clean

clean::
	$(RM) $(TARGET)
	$(RM) $(TARGET).exe

