/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the user of all the functions
 * discussed on section 6.7.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

extern char **environ;

int
main (int argc, char *argv[])
{
	int j;
	char **pptr;

	clearenv ();

	for (j=1; j<argc; ++j)
		if (putenv (argv[j]) != 0) {
			perror ("putenv()");
			return 1;
		}

	if (setenv ("GREET", "Hello world", 0) == -1) {
		perror ("setenv()");
		return 1;
	}

	unsetenv ("BYE");

	for (pptr=environ; *pptr!=NULL; ++pptr)
		puts (*pptr);

	return 0;
}
