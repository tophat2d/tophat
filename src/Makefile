# included from ../Makefile
_:
	@echo run make from repository root && false

SRCS = $(shell find src/ -name \*.c -print) # lib/chew.c
OBJS = $(sort $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)

HELPER_SCRIPT = cmd/buildhelper.um
EM_TARGET = src/staembed.c
UMKA_MODULES = \
	umka/anim.um \
	umka/audio.um \
	umka/csv.um \
	umka/ent.um \
	umka/image.um \
	umka/input.um \
	umka/misc.um \
	umka/canvas.um \
	umka/ray.um \
	umka/rect.um \
	umka/tilemap.um \
	umka/window.um \
	umka/particles.um \
	umka/lerp.um \
	umka/font.um \
	umka/th.um \
	umka/signal.um \
	umka/atlas.um \
	umka/shader.um \
	umka/color.um \
	umka/coll.um \
	umka/placeholders.um \
	umka/nav.um \
	umka/ui.um
UMKA_DOCS=$(UMKA_MODULES:.um=.md)
MMDOC=$(UMKA_BIN) cmd/mmdoc/mmdoc.um
IMAGES_MOD = umka/placeholders.um
IMAGES = etc/test.ff etc/icon.ff etc/button.ff

%.md: %.um
	@echo BU $@
	@$(MMDOC) $< > $@

$(EM_TARGET): $(UMKA_MODULES) $(UMKA_DOCS) $(IMAGES_MOD) LICENSE version $(HELPER_SCRIPT) $(UMKA_BIN) $(IMAGES)
	@echo EM $@
	@echo '#include "tophat.h"' > $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) em th_em_modulesrc $(UMKA_MODULES) >> $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) argarr th_em_modulenames $(UMKA_MODULES:umka/%=%) >> $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) em th_em_misc LICENSE version >> $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) em th_em_moduledocs $(UMKA_DOCS) >> $(EM_TARGET)
	@$(UMKA_BIN) $(HELPER_SCRIPT) placeholders $(IMAGES) >> $(EM_TARGET)

%.o: %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -MD -o $@ -c $<

$(TARGET): $(UMKA_LIBRARY) $(EM_TARGET) $(OBJS)
	@echo LD $@
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS)

src/thextdata.h:
	@echo

UMKA_SRC=lib/umka/src/umka_common.c lib/umka/src/umka_const.c lib/umka/src/umka_expr.c lib/umka/src/umka_ident.c lib/umka/src/umka_runtime.c lib/umka/src/umka_types.c lib/umka/src/umka_api.c lib/umka/src/umka_compiler.c lib/umka/src/umka_decl.c lib/umka/src/umka_gen.c lib/umka/src/umka_lexer.c lib/umka/src/umka_stmt.c lib/umka/src/umka_vm.c
cross: $(UMKA_RUNTIME_SRC)
	$(CROSS_CC) -o tophat.exe -static $(SRCS) $(UMKA_SRC) $(CFLAGS) $(CROSS_FLAGS)

bulk: $(UMKA_LIB) $(UMKA_LIBRARY)
	$(CC) -o $(TARGET) $(CFLAGS) $(SRCS) $(LDFLAGS) $(LDLIBS)

clean::
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) umkalibs.h

-include $(DEPS)

