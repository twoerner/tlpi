.PHONY: all clean
ALL = listing_15-1 listing_15-2 listing_15-5 ex_15-3 ex_15-4
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings

all: $(ALL)
listing_15-1: ch15_util.c
listing_15-5: ch15_util.c
ex_15-3: ch15_util.c
ex_15-4: ex_15-4.c
	$(CC) $(CFLAGS) -o $@ $^
	chmod u+s $@
clean::
	$(RM) $(ALL)
