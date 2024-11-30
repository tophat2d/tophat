.PHONY : all clean deepclean umka bulk

PLATFORM := $(shell uname -s)
SHORT_PLATFORM:= $(shell (X=`uname -s`; echo $${X:0:10}))

ifeq ($(PLATFORM), Linux)
	LDLIBS += -lm -lX11 -ldl -lGL -lpthread -fPIC -lXi -lXcursor
	CROSS_CC=x86_64-w64-mingw32-gcc
	CROSS_FLAGS = -lopengl32 -lgdi32 -lxinput -DNO_OPENGL_HEADERS
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

CFLAGS ?= -pipe -O3 -s
CFLAGS += -Ilib/stb/ \
	-Isrc \
	-Ilib/miniaudio/ \
	-Ilib/umprof/ \
	-Ilib/sokol/

# NOTE(skejeton): @vtereshkov here you might need to fix the flags try adding
# -mwin32 and -municode, or removing them in permutations for more info:
#  https://gcc.gnu.org/onlinedocs/gcc/x86-Windows-Options.html
ifeq ($(SHORT_PLATFORM), MINGW64_NT)
	CFLAGS += -mwindows
endif

DEFS += -DUMKA_STATIC -DUMKA_EXT_LIBS \
	-DTH_GITVER=\"$(shell git rev-parse --short HEAD)\" \
	-DTH_VERSION=\"$(shell cat version)\"
WARNS = -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare \
	-Wno-old-style-declaration -Wno-implicit-fallthrough -Wno-switch \
	-Wno-unused-function

CFLAGS += $(WARNS) $(DEFS)

all: $(UMKA_LIB) $(TARGET)

run: $(TARGET)
	./$(TARGET)

include cmd/Makeumka
include src/Makefile

deepclean:: clean

clean::
	$(RM) $(TARGET)
	$(RM) $(TARGET).exe

