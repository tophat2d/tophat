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
	@./lib/umka/build/umka cmd/embedder.um umka/animation.um umka/audio.um umka/csv.um umka/entity.um umka/image.um umka/input.um umka/misc.um umka/polygon.um umka/rawdraw.um umka/raycast.um umka/rectangle.um umka/tilemap.um umka/tophat.um umka/ui.um umka/vec.um umka/std/std.um umka/std/std.um >> src/umkalibs.h

%.o: %.c $(UMKA_LIB)
	@echo CC $@
	@$(CC) $(CFLAGS) -MM -MG -MF $(patsubst %.o,%.d,$@) -c $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS) $(UMKA_LIBRARY)
	@echo LD $@
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS)

UMKA_SRC=lib/umka/src/umka_common.c lib/umka/src/umka_const.c lib/umka/src/umka_expr.c lib/umka/src/umka_ident.c lib/umka/src/umka_runtime.c lib/umka/src/umka_types.c lib/umka/src/umka_api.c lib/umka/src/umka_compiler.c lib/umka/src/umka_decl.c lib/umka/src/umka_gen.c lib/umka/src/umka_lexer.c lib/umka/src/umka_stmt.c lib/umka/src/umka_vm.c
cross: $(UMKA_LIB)
	$(CROSS_CC) $(CFLAGS) -o tophat.exe $(SRCS) $(UMKA_SRC) $(CROSS_FLAGS) lib/rawdraw/chew.c -s -Os

bulk: $(UMKA_LIB) $(UMKA_LIBRARY)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) $(LDFLAGS) $(LDLIBS)

clean::
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) umkalibs.h

-include $(DEPS)
