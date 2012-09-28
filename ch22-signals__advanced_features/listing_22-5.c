/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrates use of sigsuspend().
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "listing_20-4.h"

static volatile sig_atomic_t gotSigQuit_G = 0;

static void handler (int sig);

int
main (void)
{
	int loopNum;
	time_t startTime;
	sigset_t origMask, blockMask;
	struct sigaction sa;

	print_signal_mask (stdout, "initial signal mask:\n");

	sigemptyset (&blockMask);
	sigaddset (&blockMask, SIGINT);
	sigaddset (&blockMask, SIGQUIT);
	if (sigprocmask (SIG_BLOCK, &blockMask, &origMask) == -1) {
		perror ("sigprocmask()");
		return 1;
	}

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction (SIGINT, &sa, NULL) == -1) {
		perror ("sigaction (SIGINT)");
		return 1;
	}
	if (sigaction (SIGQUIT, &sa, NULL) == -1) {
		perror ("sigaction (SIGQUIT)");
		return 1;
	}

	for (loopNum=1; gotSigQuit_G==0; ++loopNum) {
		printf ("=== loop %d\n", loopNum);

		print_signal_mask (stdout, "starting critical section, mask:\n");
		for (startTime=time(NULL); time(NULL)<startTime+4; );
		print_pending_signals (stdout, "before sigsuspend:\n");

		if (sigsuspend (&origMask) == -1 && errno != EINTR) {
			perror ("sigsuspend()");
			return 1;
		}
	}

	if (sigprocmask (SIG_SETMASK, &origMask, NULL) == -1) {
		perror ("sigprocmask()");
		return 1;
	}
	print_signal_mask (stdout, "=== exited loop:\n");
	return 0;
}

static void
handler (int sig)
{
	printf ("caught %d (%s)\n", sig, strsignal (sig));
	if (sig == SIGQUIT)
		gotSigQuit_G = 1;
}
