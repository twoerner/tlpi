/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * As noted in Section 20.3, sigaction() is more portable than signal() for
 * establishing a signal handler. Replace the use of signal() by sigaction()
 * in listing_20-7.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "listing_20-4.h"

// globals
static int sigCnt_G[NSIG];
static volatile sig_atomic_t gotSigInt_G = 0;

static void handler (int sig);

int
main (int argc, char *argv[])
{
	int n, numSecs;
	sigset_t pendingMask, blockingMask, emptyMask;
	struct sigaction sa;

	printf ("%s: PID is %ld\n", argv[0], (long)getpid ());

	memset (&sa, 0, sizeof (sa));
	sigemptyset (&sa.sa_mask);
	sa.sa_handler = handler;
	for (n=1; n<NSIG; ++n)
		sigaction (n, &sa, NULL);

	if (argc > 1) {
		numSecs = atoi (argv[1]);
		if (numSecs <= 0) {
			printf ("please specify a positive, non-zero optional first argument\n");
			return 1;
		}

		sigfillset (&blockingMask);
		if (sigprocmask (SIG_SETMASK, &blockingMask, NULL) == -1) {
			perror ("sigprocmask()");
			return 1;
		}

		printf ("%s: sleeping for %d seconds\n", argv[0], numSecs);
		sleep (numSecs);

		if (sigpending (&pendingMask) == -1) {
			perror ("sigpending()");
			return 1;
		}

		printf ("%s: pending signals are: \n", argv[0]);
		print_signal_set (NULL, "\t\t", &pendingMask);

		sigemptyset (&emptyMask);
		if (sigprocmask (SIG_SETMASK, &emptyMask, NULL) == -1) {
			perror ("sigprocmask()");
			return 1;
		}
	}

	while (!gotSigInt_G)
		continue;

	for (n=1; n<NSIG; ++n)
		if (sigCnt_G[n] != 0)
			printf ("%s: signal %d caught %d times\n", argv[0], n, sigCnt_G[n]);

	return 0;
}

static void
handler (int sig)
{
	if (sig == SIGINT)
		gotSigInt_G = 1;
	else
		++sigCnt_G[sig];
}
