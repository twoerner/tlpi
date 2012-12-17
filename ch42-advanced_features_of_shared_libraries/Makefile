.PHONY: all clean
ALL = listing_42-1 doodle_42-1
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings
LDFLAGS = -ldl
PWD := $(shell pwd)

all: $(ALL)

doodle_42-1: mylib_v1.so
	gcc -g $(CFLAGS) -o doodle_42-1 doodle_42-1.c -Wl,-rpath,$(PWD) -L$(PWD) -lv1

mylib_v1.so: mylib_v1.map mylib_v1.c
	gcc -g -c -fPIC $(CFLAGS) mylib_v1.c
	gcc -g -shared -o libv1.so mylib_v1.o -Wl,--version-script,mylib_v1.map

clean::
	$(RM) $(ALL) *o *.so
