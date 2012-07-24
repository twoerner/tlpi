.PHONY: all clean
ALL = listing_17-1 ex_17-1
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings -I../ch08-users_and_groups
LDFLAGS = -lacl

all: $(ALL)
listing_17-1: ../ch08-users_and_groups/libch08.o
ex_17-1: ../ch08-users_and_groups/libch08.o
clean::
	$(RM) $(ALL) ../ch08-users_and_groups/libch08.o
