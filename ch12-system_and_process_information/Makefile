.PHONY: all clean
ALL = listing_12-1 listing_12-2 ex_12-1 ex_12-1a ex_12-1b single_pointer ex_12-2 ex_12-2a \
	ex_12-2b ex_12-3
CFLAGS = -Wall -Wextra -Wconversion

all: $(ALL)
ex_12-1: ex_12-1.c ch12_util.c
ex_12-2: ex_12-2.c ch12_util.c
ex_12-2a: ex_12-2a.c ch12_util.c
ex_12-2b: ex_12-2b.c ch12_util.c
ex_12-3: ex_12-3.c ch12_util.c

clean::
	$(RM) $(ALL)
