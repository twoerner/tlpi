/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program to show that if the evp argument to timer_create() is
 * specified as NULL, then this is equivalent to specifying evp as a pointer
 * to a sigevent structure with sigev_notify set to SIGEV_SIGNAL, sigev_signo
 * set to SIGALRM, and si_value.sival_int set to the timer ID.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

// UNSAFE: printf(), strsignal()
static void
signal_handler (int sig, siginfo_t *sigInfo_p, __attribute__((unused)) void *ctx_p)
{
	printf ("caught signal %d (%s)\n", sig, strsignal (sig));
	printf ("\tsi_value.sival_int: %ld\n", (long)sigInfo_p->si_value.sival_int);
}

int
main (void)
{
	int ret;
	timer_t tid;
	struct sigaction sa;
	struct itimerspec newVal;

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = signal_handler;
	sigaction (SIGALRM, &sa, NULL);

	ret = timer_create (CLOCK_REALTIME, NULL, &tid);
	if (ret == -1) {
		perror ("timer_create()");
		return 1;
	}
	printf ("main(): tid = %ld\n", (long)tid);

	memset (&newVal, 0, sizeof (newVal));
	newVal.it_value.tv_sec = 3;
	ret = timer_settime (tid, 0, &newVal, NULL);
	if (ret == -1) {
		perror ("timer_settime()");
		return 1;
	}

	pause ();

	ret = timer_delete (tid);
	if (ret == -1) {
		perror ("timer_delete()");
		return 1;
	}

	return 0;
}
