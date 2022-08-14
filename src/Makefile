# author: @prokoprandacek
# included from ../Makefile
_:
	@echo run make from repository root && false

SRCS = $(shell find src/ -name \*.c -print) # lib/rawdraw/chew.c
OBJS = $(sort $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

HELPER_SCRIPT = cmd/buildhelper.um
EM_TARGET = src/staembed.c
UMKA_MODULES = umka/anim.um umka/audio.um umka/csv.um umka/ent.um umka/image.um umka/input.um umka/misc.um umka/canvas.um umka/ray.um umka/rect.um umka/tilemap.um umka/window.um umka/ui.um umka/std/std.um umka/particles.um umka/light.um umka/lerp.um umka/utf8.um umka/font.um umka/th.um umka/signal.um umka/atlas.um umka/ui/label.um umka/ui/grid.um umka/ui/imagebox.um umka/shader.um umka/color.um

$(EM_TARGET): $(UMKA_MODULES) LICENSE version
	@echo EM $@
	$(UMKA_BIN) $(HELPER_SCRIPT) em th_em_modulesrc $(UMKA_MODULES) > $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) argarr th_em_modulenames $(notdir $(UMKA_MODULES)) >> $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) em th_em_misc LICENSE version >> $(EM_TARGET)
	rm -f src/bindings.o

%.o: %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -MD -o $@ -c $<

$(TARGET): $(UMKA_LIBRARY) $(EM_TARGET) $(OBJS)
	@echo LD $@
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS)

UMKA_SRC=lib/umka/src/umka_common.c lib/umka/src/umka_const.c lib/umka/src/umka_expr.c lib/umka/src/umka_ident.c lib/umka/src/umka_runtime.c lib/umka/src/umka_types.c lib/umka/src/umka_api.c lib/umka/src/umka_compiler.c lib/umka/src/umka_decl.c lib/umka/src/umka_gen.c lib/umka/src/umka_lexer.c lib/umka/src/umka_stmt.c lib/umka/src/umka_vm.c
cross: $(UMKA_LIB)
	$(CROSS_CC) -o tophat.exe -static $(SRCS) $(UMKA_SRC) lib/rawdraw/chew.c $(CFLAGS) $(CROSS_FLAGS)

bulk: $(UMKA_LIB) $(UMKA_LIBRARY)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) $(LDFLAGS) $(LDLIBS)

clean::
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) umkalibs.h

-include $(DEPS)
