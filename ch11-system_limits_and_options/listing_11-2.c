/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program shows the use of fpathconf() to retrieve various
 * system limits for the file referred to by its standard input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static void
fpathconf_print (const char *msg_p, int fd, int name)
{
	long lim;

	errno = 0;
	lim = fpathconf (fd, name);
	if (lim != -1)
		printf ("%s %ld\n", msg_p, lim);
	else {
		if (errno == 0)
			printf ("%s (indeterminate)\n", msg_p);
		else {
			perror ("fpathconf()");
			exit (1);
		}
	}
}

int
main (void)
{
	fpathconf_print ("_PC_NAME_MAX:  ", STDIN_FILENO, _PC_NAME_MAX);
	fpathconf_print ("_PC_PATH_MAX:  ", STDIN_FILENO, _PC_PATH_MAX);
	fpathconf_print ("_PC_PIPE_BUF:  ", STDIN_FILENO, _PC_PIPE_BUF);

	return 0;
}
