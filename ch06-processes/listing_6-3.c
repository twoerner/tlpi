/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program accesses 'environ' in order to list all the values
 * in the process environment.
 */

#include <stdio.h>

extern char **environ;

int
main (void)
{
	char **pptr;

	for (pptr = environ; *pptr!=NULL; ++pptr)
		puts (*pptr);

	return 0;
}
