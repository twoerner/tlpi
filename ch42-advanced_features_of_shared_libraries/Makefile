.PHONY: all clean
ALL = listing_42-1 doodle_42-0 doodle_42-1 doodle_42-2 doodle_42-3
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings
LDFLAGS = -ldl
PWD := $(shell pwd)

all: $(ALL)

doodle_42-0: libv0.so
	gcc -g $(CFLAGS) -o doodle_42-0 doodle_42-0.c -Wl,-rpath,$(PWD) -L$(PWD) -lv0

doodle_42-1: libv1.so
	gcc -g $(CFLAGS) -o doodle_42-1 doodle_42-1.c -Wl,-rpath,$(PWD) -L$(PWD) -lv1

doodle_42-2: libv2.so
	gcc -g $(CFLAGS) -o doodle_42-2 doodle_42-2.c -Wl,-rpath,$(PWD) -L$(PWD) -lv2

doodle_42-3: libv2.so
	gcc -g $(CFLAGS) -o doodle_42-3 doodle_42-3.c -Wl,-rpath,$(PWD) -L$(PWD) -lv2

libv0.so: mylib_v0.c
	gcc -g -c -fPIC $(CFLAGS) mylib_v0.c
	gcc -g -shared -o libv0.so mylib_v0.o

libv1.so: mylib_v1.map mylib_v1.c
	gcc -g -c -fPIC $(CFLAGS) mylib_v1.c
	gcc -g -shared -o libv1.so mylib_v1.o -Wl,--version-script,mylib_v1.map

libv2.so: mylib_v2.map mylib_v2.c
	gcc -g -c -fPIC $(CFLAGS) mylib_v2.c
	gcc -g -shared -o libv2.so mylib_v2.o -Wl,--version-script,mylib_v2.map

clean::
	$(RM) $(ALL) *\.o *\.so
