/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write programs that verify the effect of the SA_RESETHAND and SA_NOREFER
 * flags when establishing a signal handler with sigaction().
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static void handler (int);

int
main (void)
{
	int ret;
	struct sigaction sa;

	memset (&sa, 0, sizeof (sa));
	ret = sigemptyset (&sa.sa_mask);
	if (ret != 0) {
		perror ("sigemptyset()");
		return 1;
	}

	sa.sa_handler = handler;
	sa.sa_flags = SA_RESETHAND;
	ret = sigaction (SIGINT, &sa, NULL);
	if (ret != 0) {
		perror ("sigaction()");
		return 1;
	}

	while (1)
		pause ();

	return 0;
}

static void
handler (int __attribute__((unused)) sig)
{
	printf ("in handler\n"); // not 100% async safe!
}
