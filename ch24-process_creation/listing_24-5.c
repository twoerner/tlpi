/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstrate who (the parent or the child) gets to run first
 * after a call to fork().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int
main (int argc, char *argv[])
{
	int numCh, i;
	pid_t chldPid;

	if (argc > 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<number of children>]\n", argv[0]);
		return 0;
	}

	numCh = (argc > 1)? atoi (argv[1]) : 1;
	if (numCh < 1) {
		printf ("please specify a positive, non-zero, number of children\n");
		return 1;
	}
	setbuf (stdout, NULL);

	for (i=0; i<numCh; ++i) {
		switch (chldPid = fork ()) {
			case -1:
				perror ("fork()");
				return 1;

			case 0: // child
				printf ("%d child\n", i);
				_exit (0);

			default: // parent
				printf ("%d parent\n", i);
				wait (NULL);
				break;
		}
	}

	return 0;
}
