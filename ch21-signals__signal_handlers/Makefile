.PHONY: all clean
ALL = listing_21-1 listing_21-2a listing_21-2b listing_21-3
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings -I../ch20-signals_fundamental_concepts
LDFLAGS = -lcrypt

all: $(ALL)
listing_21-2a: ../ch20-signals_fundamental_concepts/listing_20-4.o
listing_21-2b: ../ch20-signals_fundamental_concepts/listing_20-4.o
clean::
	$(RM) $(ALL)
