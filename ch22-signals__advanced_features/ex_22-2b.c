/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * If both a realtime and a standard signal are pending for a process SUSv3
 * leaves it unspecified which is delivered first. Write a program to show
 * what Linux does in this case.
 *
 * Hints:
 *    - have the program setup a handler for all signals
 *    - block signals for a period of time so that you can send signals to it
 *    - then unblock all the signals
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define SLEEPSEC 30
#define UNUSED __attribute__((unused))

static void signal_handler (int sig, siginfo_t UNUSED *sigInfo_p, void UNUSED *uctx_p);

int
main (void)
{
	int sig;
	struct sigaction sa;
	sigset_t sigs;

	// setup signal handler for all signals
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_sigaction = signal_handler;
	for (sig=0; sig<NSIG; ++sig)
		sigaction (sig, &sa, NULL);

	// block all signals
	if (sigfillset (&sigs) == -1) {
		perror ("sigfillset()");
		exit (1);
	}
	if (sigprocmask (SIG_SETMASK, &sigs, NULL) == -1) {
		perror ("sigprocmask()");
		exit (1);
	}

	fprintf (stderr, "sleeping for %d seconds...\n", SLEEPSEC);
	sleep (SLEEPSEC);
	fprintf (stderr, "done\n");

	// unblock all signals
	if (sigprocmask (SIG_UNBLOCK, &sigs, NULL) == -1) {
		perror ("sigprocmask(2)");
		exit (1);
	}

	return 0;
}

static void
signal_handler (int sig, siginfo_t UNUSED *sigInfo_p, void UNUSED *uctx_p)
{
	fprintf (stderr, "handling %2d (%s)\n", sig, strsignal (sig)); // UNSAFE
}
