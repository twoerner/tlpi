/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Other half of Listing 27-2.
 */

#include <stdio.h>

extern char **environ;

int
main (int argc, char *argv[])
{
	int i;
	char **ep;

	for (i=0; i<argc; ++i)
		printf ("argv[%d] = %s\n", i, argv[i]);
	for (ep=environ; *ep!=NULL; ++ep)
		printf ("environ: %s\n", *ep);

	return 0;
}
