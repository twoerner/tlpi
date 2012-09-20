/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program shows a simple example of a signal handler function.
 */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void
sigHandler (int __attribute__ ((unused)) sig)
{
	printf ("ouch!\n");
}

int
main (void)
{
	int i;

	if (signal (SIGINT, sigHandler) == SIG_ERR) {
		perror ("signal()");
		return 1;
	}

	for (i=0; i<20; ++i) {
		printf ("%d\n", i);
		sleep (3);
	}

	return 0;
}
