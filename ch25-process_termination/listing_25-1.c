/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstrate the use of atexit() and on_exit().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// set to 0 or 1
#define USE_EXIT 0

#define ATEXIT(n) \
	static void \
	atexit_fn##n (void) \
	{ \
		printf ("%s called\n", __func__); \
	}

ATEXIT(1)
ATEXIT(2)

static void
on_exit_fn (int exitStatus, void *arg_p)
{
	printf ("%s called\n\texit status: %d\n\targ: %ld\n", __func__, exitStatus, (long)arg_p);
}

int
main (void)
{
	if (on_exit (on_exit_fn, (void*)10) != 0) {
		perror ("on_exit_fn");
		_exit (1);
	}
	if (atexit (atexit_fn1) != 0) {
		perror ("atexit_fn1");
		_exit (1);
	}
	if (atexit (atexit_fn2) != 0) {
		perror ("atexit_fn2");
		_exit (1);
	}
	if (on_exit (on_exit_fn, (void*)20) != 0) {
		perror ("on_exit_fn");
		_exit (1);
	}

#if (USE_EXIT == 1)
	exit (2);
#else
	return 3;
#endif
}
