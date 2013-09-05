.PHONY: all clean
ALL = doodle_41-1 soname nosoname
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings
PWD := $(shell pwd)

all: $(ALL)

doodle_41-1: doodle_41-1.c d1/libx1.so
	gcc $(CFLAGS) -g -o doodle_41-1 doodle_41-1.c -Wl,-rpath,$(PWD)/d1 -L$(PWD)/d1 -lx1

d1/libx1.so:
	$(MAKE) -C d1

soname: soname.c libfoo.so libbar.so
	gcc $(CFLAGS) -g -o soname soname.c -Wl,-rpath,$(PWD) -L$(PWD) -lfoo
libfoo.so: libfoo.c
	gcc $(CFLAGS) -g -fPIC -shared -Wl,-soname,libbar.so -o libfoo.so libfoo.c
libbar.so: libbar.c
	gcc $(CFLAGS) -g -fPIC -shared -o libbar.so libbar.c

nosoname: nosoname.c libnsn.so.2.0.6
	gcc $(CFLAGS) -g -o nosoname nosoname.c -Wl,-rpath,$(PWD) libnsn.so.2.0.6
libnsn.so.2.0.6: libnsn.c
	gcc $(CFLAGS) -g -fPIC -shared -o libnsn.so.2.0.6 libnsn.c

clean::
	$(RM) $(ALL) libfoo.so libbar.so libnsn.so.2.0.6
	$(MAKE) -C d1 clean
