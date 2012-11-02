/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program to verify that when a child's parent terminates, a call to
 * getpid() returns 1 (i.e. the child process was orphaned and adopted by the
 * init process).
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

		case 0: // child
			sleep (5);
			printf ("parent: %ld\n", (long)getppid ());
			_exit (0);

		default:
			break;
	}

	return 0;
}
