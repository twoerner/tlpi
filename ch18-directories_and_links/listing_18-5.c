/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstrate usage of dirname(3) and basename(3).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

int
main (int argc, char *argv[])
{
	char *t1_p, *t2_p;
	int i;

	for (i=1; i<argc; ++i) {
		t1_p = strdup (argv[i]);
		if (t1_p == NULL) {
			perror ("strdup()");
			return 1;
		}

		t2_p = strdup (argv[i]);
		if (t2_p == NULL) {
			perror ("strdup()");
			return 1;
		}

		printf ("%s ==> %s + %s\n", argv[i], dirname (t1_p), basename (t2_p));

		free (t1_p);
		free (t2_p);
	}

	return 0;
}
