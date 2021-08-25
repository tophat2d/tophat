# author: @prokoprandacek
# included from ../Makefile
_:
	@echo run make from repository root && false

SRCS = $(shell find src/ -name \*.c -print)
OBJS = $(sort $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

UMKA_LIB = src/umkalibs.h

$(UMKA_LIB): $(UMKA_LIBRARY)
	@echo EM $@
	@echo "#ifndef UMKALIBS_H" > src/umkalibs.h
	@echo "#define UMKALIBS_H" >> src/umkalibs.h
	@echo "const char *libs[] = {" >> src/umkalibs.h
	@./lib/umka/build/umka cmd/embedder.um umka/animation.um umka/audio.um umka/csv.um umka/entity.um umka/image.um umka/input.um umka/misc.um umka/polygon.um umka/rawdraw.um umka/raycast.um umka/rectangle.um umka/tilemap.um umka/tophat.um umka/ui.um umka/vec.um umka/std/std.um umka/std/std.um >> src/umkalibs.h
	@echo "};" >> src/umkalibs.h
	@echo "#endif" >> src/umkalibs.h

%.o: %.c $(UMKA_LIB)
	@echo CC $@
	@$(CC) $(CFLAGS) -MM -MG -MF $(patsubst %.o,%.d,$@) -c $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS) $(UMKA_LIBRARY)
	@echo LD $@
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS)

#win: $(UMKA_LIB)
#	$(CROSS_CC) -o tophat.exe $(SRCS) $(UMKA_LIBRARY) $(CFLAGS) -lm -Ldl -lopengl32 -lgdi32 -Wl,-Bstatic -lpthread $(LDFLAGS) -DNO_OPENGL_HEADERS -DRELEASE_BUILD

clean::
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) umkalibs.h

-include $(DEPS)
