.PHONY: all clean
ALL = listing_36-3 \
	doodle_36-1
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings

all: $(ALL)

listing_36-3: print_rlimit.o
doodle_36-1: print_rlimit.o

all: $(ALL)
clean::
	$(RM) $(ALL) *\.o
