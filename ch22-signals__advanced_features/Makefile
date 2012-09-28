.PHONY: all clean
ALL = listing_22-2 listing_22-3 listing_22-5 listing_22-6 listing_22-7 \
	ex_22-1 ex_22-2 ex_22-2b ex_22-3a ex_22-3b
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings -I../ch20-signals_fundamental_concepts

all: $(ALL)
listing_22-5: ../ch20-signals_fundamental_concepts/listing_20-4.o
clean::
	$(RM) $(ALL)
