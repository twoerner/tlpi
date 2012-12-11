.PHONY: all clean
ALL = doodle_41-1
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings
PWD := $(shell pwd)

all: $(ALL)

doodle_41-1: doodle_41-1.c d1/libx1.so
	gcc $(CFLAGS) -g -o doodle_41-1 doodle_41-1.c -Wl,-rpath,$(PWD)/d1 -L$(PWD)/d1 -lx1

d1/libx1.so:
	$(MAKE) -C d1

clean::
	$(RM) $(ALL)
	$(MAKE) -C d1 clean
