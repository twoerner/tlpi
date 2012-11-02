/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program retrieves and displays the names and values of all EAs of the
 * files listed on its cmdline. By default attribute values are displayed as
 * plain text. If the '-x' option is supplied, the attribute values are
 * displayed as hexadecimal strings. E.g.:
 *
 *	$ touch tfile
 *	$ setfattr -n user.x -v "The past is not dead." tfile
 *	$ setfattr -n user.y -v "In fact, it's not even past." tfile
 *	$ ./listing_16-1 tfile
 *	tfile:
 *		name=user.x; value=The past is not dead.
 *		name=user.y; value=In fact, it's not even past.
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/xattr.h>

#define XATTR_SIZE 10000

static void usage (const char *pgm_p);

int
main (int argc, char *argv[])
{
	char list[XATTR_SIZE], value[XATTR_SIZE];
	ssize_t listLen, valueLen;
	int ns, i, j, opt;
	bool hexDisplay;

	hexDisplay = false;
	while ((opt = getopt (argc, argv, "x")) != -1) {
		switch (opt) {
			case 'x':
				hexDisplay = true;
				break;

			case '?':
				usage (argv[0]);
				return 1;
		}
	}

	if (optind >= (argc + 2)) {
		usage (argv[0]);
		return 1;
	}

	for (i=optind; i<argc; ++i) {
		listLen = listxattr (argv[i], list, XATTR_SIZE);
		if (listLen == -1) {
			perror ("listxattr()");
			return 1;
		}

		printf ("%s:\n", argv[i]);

		for (ns=0; ns<listLen; ns+=strlen(&list[ns]) + 1) {
			printf ("\tname=%s ", &list[ns]);

			valueLen = getxattr (argv[i], &list[ns], value, XATTR_SIZE);
			if (valueLen == -1)
				printf ("(couldn't get value)");
			else if (!hexDisplay)
				printf ("value=%.*s", (int)valueLen, value);
			else {
				printf ("value=");
				for (j=0; j<valueLen; ++j)
					printf ("%02x ", (unsigned)value[j]);
			}
			printf ("\n");
		}
		printf ("\n");
	}

	return 0;
}

static void
usage (const char *cmd_p)
{
	printf ("usage: %s [-x] <file(s)...>\n", cmd_p);
	printf ("  -x    display value in hex\n");
}
