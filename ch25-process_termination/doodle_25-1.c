/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Interactions between fork(), stdio buffers, exec(), and _exit().
 *
 * Compare/contrast the output from Listing 25-2 versus the output from this
 * doodle when both are run with and without redirection.
 *
 *	$ ./listing_25-2
 *	hello, world
 *	ciao
 *
 *	$ ./listing_25-2 | more
 *	ciao
 *	hello, world
 *	hello, world
 *
 *	$ ./doodle_25-1
 *	hello, world
 *	ciao
 *	Makefile     doodle_25-1.c  listing_25-1.c  listing_25-2.c
 *	doodle_25-1  listing_25-1   listing_25-2
 *	
 *
 *	$ ./doodle_25-1 | more
 *	ciao
 *	hello, world
 *	Makefile
 *	doodle_25-1
 *	doodle_25-1.c
 *	listing_25-1
 *	listing_25-1.c
 *	listing_25-2
 *	listing_25-2.c
 */

#include <stdio.h>
#include <unistd.h>

int
main (void)
{
	printf ("hello, world\n");
	write (STDOUT_FILENO, "ciao\n", 5);

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			execlp ("ls", "ls", NULL);
			break;

		default: // parent
			break;
	}

	return 0;
}
