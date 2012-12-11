.PHONY: all clean
ALL = libx2.so
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings

all: $(ALL)

libx2.so: modx2.c
	gcc $(CFLAGS) -g -c -fPIC modx2.c
	gcc $(CFLAGS) -g -shared -o libx2.so modx2.o

clean::
	$(RM) $(ALL) *.o
