/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Passing the caller's environment to the new program using execl().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int
main (void)
{
	printf ("init value of USER: %s\n", getenv ("USER"));
	if (putenv ("USER=britta") != 0) {
		perror ("putenv()");
		return 1;
	}

	execl ("/usr/bin/printenv", "printenv", "USER", "SHELL", NULL);
	perror ("execl()");
	return 1;
}
