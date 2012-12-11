.PHONY: all clean
ALL = doodle_41-1
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings

all: $(ALL)

doodle_41-1: doodle_41-1.c d1/libx1.so
d1/libx1.so:
	$(MAKE) -C d1

clean::
	$(RM) $(ALL)
	$(MAKE) -C d1 clean
