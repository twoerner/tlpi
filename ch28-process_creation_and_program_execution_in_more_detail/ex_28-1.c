/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program to see how fast the fork() and vfork() system calls are on
 * your system. Each child process should immediately exit, and the parent
 * should wait() on each child before creating the next.
 *
 * fork():
 *	$ time ./ex_28-1
 *
 *	real    1m28.797s
 *	user    0m1.441s
 *	sys     0m39.347s
 *
 * vfork():
 *	$ time ./ex_28-1
 *	
 *	real    0m11.347s
 *	user    0m0.181s
 *	sys     0m7.709s
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define USE_FORK 1

int
main (void)
{
	unsigned i;

	for (i=0; i<500000; ++i) {
#if (USE_FORK == 1)
		switch (fork ()) {
#else
		switch (vfork ()) {
#endif
			case -1:
				return 1;

			case 0: // child
				_exit (0);

			default: // parent
				wait (NULL);
		}
	}

	return 0;
}
