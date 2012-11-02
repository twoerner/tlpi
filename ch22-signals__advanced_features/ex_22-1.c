/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Section 22.2 noted that if a stopped process that has established a handler
 * for and blocked SIGCONT is later resumed as a consequence of receiving a
 * SIGCONT, then the handler is invoked only when SIGCONT is unblocked. Write
 * a program to verify this. Recall that a process can be stopped by typing
 * the terminal suspend character (usually Ctrl-Z) and can be sent a SIGCONT
 * signal using either "kill -CONT" or using the shell "fg" command.
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static void
handler (int sig)
{
	printf ("caught signal %d\n", sig);
}

int
main (void)
{
	sigset_t block;
	struct sigaction sa;

	if (sigemptyset (&block) == -1) {
		perror ("sigemptyset()");
		return 1;
	}
	if (sigaddset (&block, SIGCONT) == -1) {
		perror ("sigaddset()");
		return 1;
	}
	if (sigprocmask (SIG_BLOCK, &block, NULL) == -1) {
		perror ("sigprocmask()");
		return 1;
	}

	if (sigemptyset (&sa.sa_mask) == -1) {
		perror ("sigemptyset (sa_mask)");
		return 1;
	}
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction (SIGCONT, &sa, NULL) == -1) {
		perror ("sigaction()");
		return 1;
	}

	fprintf (stderr, "sleeping... ");
	sleep (60);
	fprintf (stderr, "done\n");

	if (sigprocmask (SIG_UNBLOCK, &block, NULL) == -1) {
		perror ("sigprocmask (2)");
		return 1;
	}

	return 0;
}
