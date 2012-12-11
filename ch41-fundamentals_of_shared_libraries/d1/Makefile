.PHONY: all clean
ALL = libx1.so
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings
PWD := $(shell pwd)

all: $(ALL)

libx1.so: modx1.c d2/libx2.so
	gcc $(CFLAGS) -g -c -fPIC modx1.c
	gcc $(CFLAGS) -g -shared -o libx1.so modx1.o -Wl,-rpath,$(PWD)/d2 -L$(PWD)/d2 -lx2

d2/libx2.so:
	$(MAKE) -C d2

clean::
	$(RM) $(ALL) *.o
	$(MAKE) -C d2 clean
