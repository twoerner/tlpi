/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates how to read and modify a /proc file.
 * This program reads and displays the contents of /proc/sys/kernel/pid_max.
 * If a cmdline arg is supplied, the program updates the file using that value.
 * This file (which is new in Linux 2.6) specifies an upper limit for proces IDs.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

#define MAX_LINE 100

int
main (int argc, char *argv[])
{
	int fd, ilen;
	char line[MAX_LINE];
	ssize_t n;
	size_t len;

	fd = open ("/proc/sys/kernel/pid_max", (argc > 1)? O_RDWR : O_RDONLY);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	n = read (fd, line, MAX_LINE);
	if (n == -1) {
		perror ("read()");
		return 1;
	}

	if (argc > 1)
		printf ("old value: ");
	printf ("%.*s", (int)n, line);

	if (argc > 1) {
		len = strlen (argv[1]);
		if (len < INT_MAX)
			ilen = (int)len;
		else {
			printf ("length of cmdline arg too long\n");
			return 1;
		}
		if (write (fd, argv[1], strlen (argv[1])) != ilen) {
			perror ("write()");
			return 1;
		}

		system ("echo /proc/sys/kernel/pid_max now contians `cat /proc/sys/kernel/pid_max`");
	}

	return 0;
}
