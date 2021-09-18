.PHONY : all clean deepclean umka bulk

PLATFORM := $(shell uname -s)
SHORT_PLATFORM:= $(shell (X=`uname -s`; echo $${X:0:10}))

ifeq ($(PLATFORM), Linux)
  LDLIBS += -lm -lX11 -ldl -lGL -lpthread
  CROSS_CC=x86_64-w64-mingw32-gcc
  CROSS_FLAGS = -lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread $(WARNS) $(DEFS) -DNO_OPENGL_HEADERS
  TARGET=tophat
else
ifeq ($(SHORT_PLATFORM), MINGW64_NT)
  LDLIBS += -lm -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread lib/rawdraw/chew.c
  TARGET=tophat.exe
  DEFS += -DNO_OPENGL_HEADERS
endif
endif

CFLAGS ?= -Ofast -pipe
CFLAGS += -Ilib/stb/ \
		  -Ilib/rawdraw/ \
		  -Ilib/miniaudio/

DEFS += -DCNFGOGL -DUMKA_STATIC -DRELEASE_BUILD
WARNS = \
		-Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
		-Wno-old-style-declaration -Wno-implicit-fallthrough

CFLAGS += $(WARNS) $(DEFS)

all: $(UMKA_LIB) $(TARGET)

run: $(TARGET)
	./$(TARGET)

include Makeumka
include src/Makefile

deepclean:: clean

install: $(TARGET) cross
	./package.sh
	cp cmd/tophat-cli /usr/bin
	./cmd/tophat-cli install tophat-linux.zip
	rm tophat-linux.zip tophat-win.zip

clean::
	$(RM) $(TARGET)
	$(RM) $(TARGET).exe


