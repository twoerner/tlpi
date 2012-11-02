/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Using execlp() to search for a filename in a PATH.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	if (argc != 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pathname>\n", argv[0]);
		return 1;
	}

	execlp (argv[1], argv[1], "hello world", NULL);
	perror ("execlp()");
	return 1;
}
