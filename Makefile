# author: @prokoprandacek
.PHONY : all clean deepclean umka bulk

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

CROSS_CC = x86_64-w64-mingw32-gcc
CROSS_FLAGS = -lm -Ldl -Ilib/rawdraw -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread $(WARNS) $(DEFS) -DNO_OPENGL_HEADERS

TARGET = tophat

all: $(TARGET)

run: $(TARGET)
	./$(TARGET)

include Makeumka
include src/Makefile

deepclean:: clean

clean::
	$(RM) $(TARGET)
	$(RM) $(TARGET).exe


