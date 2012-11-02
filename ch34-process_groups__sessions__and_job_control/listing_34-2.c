/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the use of setsid() to create a new session.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main (void)
{
	if (fork () != 0)
		_exit (0);

	// child only
	if (setsid () == -1) {
		perror ("setsid()");
		return 1;
	}

	printf ("PID:%ld PGID:%ld SID:%ld\n", (long)getpid (), (long)getpgrp (), (long)getsid (0));

	if (open ("/dev/tty", O_RDWR) == -1)
		perror ("open()");
	return 0;
}
