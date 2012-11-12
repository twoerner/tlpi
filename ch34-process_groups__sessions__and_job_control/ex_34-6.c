/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program to verify that when a process in an orphaned process group
 * attempts to read() from the controlling terminal, the read() fails with
 * error EIO.
 */

#include <stdio.h>
#include <unistd.h>

int
main (void)
{
	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: { // child
			ssize_t rdCnt;
			char buf[256];

			sleep (5);
			rdCnt = read (STDIN_FILENO, buf, 256);
			if (rdCnt == -1)
				perror ("read()");
			_exit (0);
			break; }

		default:
			break;
	}

	sleep (2);
	printf ("parent exiting\n");

	return 0;
}
