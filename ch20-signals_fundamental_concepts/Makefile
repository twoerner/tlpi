.PHONY: all clean
ALL = listing_20-1 listing_20-2 listing_20-3 listing_20-4.o listing_20-6 listing_20-7 \
	ex_20-1 ex_20-2
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings

all: $(ALL)
listing_20-7: listing_20-4.o
ex_20-1: listing_20-4.o
clean::
	$(RM) $(ALL)
