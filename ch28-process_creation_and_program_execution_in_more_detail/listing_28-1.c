/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program uses acct() to switch process accounting on and off.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	if (argc > 2 || (argc > 1 && strcmp (argv[1], "--help") == 0)) {
		printf ("usage: %s [<file>]\n", argv[0]);
		return 1;
	}

	if (acct (argv[1]) == -1) {
		perror ("acct()");
		return 1;
	}

	printf ("process accounting %sabled\n", (argv[1] == NULL)? "dis" : "en");
	return 0;
}
