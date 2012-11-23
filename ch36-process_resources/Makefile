.PHONY: all clean
ALL = listing_36-3 \
	doodle_36-1 \
	ex_36-1 ex_36-2 ex_36-3a ex_36-3b ex_36-3_sa
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings

all: $(ALL)

listing_36-3: print_rlimit.c print_rlimit.h
doodle_36-1: print_rlimit.c print_rlimit.h
ex_36-3a: print_rlimit.c print_rlimit.h
ex_36-3b: print_rlimit.c print_rlimit.h

all: $(ALL)
clean::
	$(RM) $(ALL)
