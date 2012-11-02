/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Interactions between fork(), stdio buffers, and _exit().
 */

#include <stdio.h>
#include <unistd.h>

int
main (void)
{
	printf ("hello, world\n");
	write (STDOUT_FILENO, "ciao\n", 5);

	if (fork () == -1) {
		perror ("fork()");
		return 1;
	}

	return 0;
}
