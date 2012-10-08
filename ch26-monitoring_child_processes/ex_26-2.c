/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Suppose that we have three processes related as grandparent, parent, and
 * child, and that the grandparent doesn't immediately perform a wait() after
 * the parents exits, so that the parent becomes a zombie. When do you expect
 * the grandchildren to be adopted by init() (so that getppid() in the
 * grandchild returns 1):
 *	1) after the parent terminates
 *	2) after the grandparent does a wait()?
 * Write a program to verify your answer.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int
main (void)
{
	switch (fork ()) {
		case 0: // parent
			switch (fork ()) {
				case 0: // grand-child
					sleep (4);
					printf ("parent %ld\n", (long)getppid ());

					sleep (4);
					printf ("parent %ld\n", (long)getppid ());

					_exit (0);

				default:
					_exit (0);
			}
			break;

		default: // grandparent
			sleep (6);
			wait (NULL);
			break;
	}

	return 0;
}
