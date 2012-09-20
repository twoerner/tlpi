/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * The program demonstrates using the same signal handler
 * to catch multiple signals.
 *
 * NOTE: this program's signal handler uses non-async-signal-safe
 *       functions e.g. printf(), exit()
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static void
signal_handler (int sig)
{
	static int count = 0;

	if (sig == SIGINT) {
		printf (": caught SIGINT (%d)\n", ++count);
		return;
	}

	printf (": caught SIGQUIT\n");
	exit (0);
}

int
main (void)
{
	if (signal (SIGINT, signal_handler) == SIG_ERR) {
		perror ("signal (SIGINT)");
		return 1;
	}
	if (signal (SIGQUIT, signal_handler) == SIG_ERR) {
		perror ("signal (SIGQUIT)");
		return 1;
	}

	for (;;)
		pause ();

	return 0;
}
