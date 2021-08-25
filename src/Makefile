# author: @prokoprandacek
# included from ../Makefile
_:
	@echo run make from repository root && false

SRCS = $(shell find src/ -name \*.c -print)
OBJS = $(sort $(SRCS:.c=.o))
DEPS = $(OBJS:.o=.d)


%.o: %.c
	@echo CC $@
	@$(CC) $(CFLAGS) -MM -MG -MF $(patsubst %.o,%.d,$@) -c $<
	@$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS) $(UMKA_LIBRARY)
	@echo LD $@
	@$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS) $(LDFLAGS)

clean::
	$(RM) $(OBJS)
	$(RM) $(DEPS)
	$(RM) umka_libs.h

-include $(DEPS)
