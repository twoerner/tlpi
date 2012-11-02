/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstration of vfork().
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int
main (void)
{
	int istack = 222;

	switch (vfork ()) {
		case -1:
			perror ("vfork()");
			return 1;

		case 0: // child
			sleep (3);

			printf ("child\n");
			istack *= 3;
			_exit (0);

		default:
			printf ("parent\n");
			printf ("\tistack: %d\n", istack);
	}

	return 0;
}
