/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LINE 100

int
main (int argc, char *argv[])
{
	int fd;
	char line[MAX_LINE];
	ssize_t n;

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
		if (write (fd, argv[1], strlen (argv[1])) != strlen (argv[1])) {
			perror ("write()");
			return 1;
		}

		system ("echo /proc/sys/kernel/pid_max now contians `cat /proc/sys/kernel/pid_max`");
	}

	return 0;
}
