.PHONY: all clean
ALL = listing_17-1
CFLAGS = -Wall -Wextra -Wshadow -Werror -Wwrite-strings -I../ch09-process_credentials
LDFLAGS = -lacl

all: $(ALL)
listing_17-1: ../ch09-process_credentials/libch09.o
clean::
	$(RM) $(ALL) ../ch09-process_credentials/libch09.o
