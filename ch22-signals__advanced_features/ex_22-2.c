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

// set to 0 or 1
#define USE_SIGQUEUE 0
// set to 0 or 1
#define USE_SIGINFO 0
// set to 0 or 1
#define EMPTY_MASK 1

#if (USE_SIGINFO == 1)
#define UNUSED __attribute__((unused))
static void signal_handler (int sig, UNUSED siginfo_t*, UNUSED void*);
#else
static void signal_handler (int sig);
#endif
static void block_and_raise (unsigned cnt, ...);

int
main (void)
{
	int sig;
	struct sigaction sa;

	// setup signal handler for all signals
#if (EMPTY_MASK == 1)
	sigemptyset (&sa.sa_mask);
#else
	sigfillset (&sa.sa_mask);
#endif
	sa.sa_flags = 0;
#if (USE_SIGINFO == 1)
	sa.sa_sigaction = signal_handler;
#else
	sa.sa_handler = signal_handler;
#endif
	for (sig=0; sig<NSIG; ++sig)
		sigaction (sig, &sa, NULL);

	block_and_raise (4, 2, 10, 44, 42);
	block_and_raise (4, 42, 44, 10, 2);
	block_and_raise (4, 42, 2, 44, 10);
	block_and_raise (2, 44, 42);
	block_and_raise (2, 42, 44);
	block_and_raise (2, 2, 10);
	block_and_raise (2, 10, 2);
	block_and_raise (2, 54, 40);
	block_and_raise (2, 40, 54);

	return 0;
}

static void
block_and_raise (unsigned cnt, ...)
{
	va_list ap;
	unsigned i;
	int sig;
	sigset_t sigs;
#if (USE_SIGQUEUE == 1)
	pid_t pid;
	union sigval inVal;

	pid = getpid ();
	inVal.sival_int = 0;
#endif

	// block all signals
	if (sigfillset (&sigs) == -1) {
		perror ("sigfillset()");
		exit (1);
	}
	if (sigprocmask (SIG_SETMASK, &sigs, NULL) == -1) {
		perror ("sigprocmask()");
		exit (1);
	}

	// raise signals
	va_start (ap, cnt);
	for (i=0; i<cnt; ++i) {
		sig = va_arg (ap, int);
#if (USE_SIGQUEUE == 1)
		fprintf (stderr, "\tsigqueueing signal %d\n", sig);
		if (sigqueue (pid, sig, inVal) == -1) {
			perror ("sigqueue()");
			exit (1);
		}
#else
		fprintf (stderr, "\traising signal %d\n", sig);
		raise (sig);
#endif
	}

	// unblock all signals
	if (sigprocmask (SIG_UNBLOCK, &sigs, NULL) == -1) {
		perror ("sigprocmask(2)");
		exit (1);
	}

	fprintf (stderr, "\n");
}

static void
#if (USE_SIGINFO == 1)
signal_handler (int sig, UNUSED siginfo_t *a, UNUSED void *b)
#else
signal_handler (int sig)
#endif
{
	fprintf (stderr, "handling %2d (%s)\n", sig, strsignal (sig)); // UNSAFE
}
