/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the use of setitimer() and getitimer().
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

// set to 0 or 1
#define USE_SETITIMER 0		// setting this to 1 d/n work

static volatile sig_atomic_t gotAlarm_G = 0;

static void display_times (const char *msg_p, bool includeTimer);
static void signal_handler (int sig);

int
main (int argc, char *argv[])
{
	struct itimerval itv;
	clock_t prevClock;
	int maxSigs;
	int sigCnt;
	struct sigaction sa;
	int ret;

	if (argc > 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<secs> [<interval secs>]]\n", argv[0]);
		return 0;
	}

	sigCnt = 0;

	ret = sigemptyset (&sa.sa_mask);
	if (ret == -1) {
		perror ("sigemptyset()");
		return 1;
	}
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	ret = sigaction (SIGALRM, &sa, NULL);
	if (ret == -1) {
		perror ("sigaction()");
		return 1;
	}

	// set timer from cmdline
	itv.it_value.tv_sec = (argc > 1)? atol (argv[1]) : 2;
	itv.it_value.tv_usec = 0;
	itv.it_interval.tv_sec = (argc > 2)? atol (argv[2]) : 0;
	itv.it_interval.tv_usec = 0;

	maxSigs = (itv.it_interval.tv_sec == 0)? 1 : 3;
	display_times ("start:", false);

	ret = setitimer (ITIMER_REAL, &itv, NULL);
	if (ret == -1) {
		perror ("setitimer()");
		return 1;
	}

	prevClock = clock ();
	sigCnt = 0;

	for (;;) {
		while (((clock () - prevClock) * 10 / CLOCKS_PER_SEC) < 5) {
			if (gotAlarm_G) {
				gotAlarm_G = 0;
				display_times ("alarm:", true);

				++sigCnt;
				if (sigCnt >= maxSigs) {
					printf ("done\n");
					return 0;
				}
			}
		}

		prevClock = clock ();
		display_times ("main:", true);
	}
}

static void
display_times (const char *msg_p, bool includeTimer)
{
	struct itimerval itv;
	static struct timeval start;
	struct timeval curr;
	static int callNum = 0;

	if (callNum == 0) {
		if (gettimeofday (&start, NULL) == -1) {
			perror ("gettimeofday()");
			exit (1);
		}
	}

	if ((callNum % 20) == 0)
		printf ("\telapsed value interval\n");

	if (gettimeofday (&curr, NULL) == -1) {
		perror ("gettimeofday()");
		exit (1);
	}
	printf ("%-7s %6.2f", msg_p, curr.tv_sec - start.tv_sec + (curr.tv_usec - start.tv_usec) / 1000000.0);

	if (includeTimer) {
#if (USE_SETITIMER == 1)
		if (setitimer (ITIMER_REAL, NULL, &itv) == -1) {
			perror ("setitimer()");
#else
		if (getitimer (ITIMER_REAL, &itv) == -1) {
			perror ("getitimer()");
#endif
			exit (1);
		}
		printf ("\t%6.2f %6.2f",
				itv.it_value.tv_sec + itv.it_value.tv_usec / 1000000.0,
				itv.it_interval.tv_sec + itv.it_interval.tv_usec / 1000000.0);
	}

	printf ("\n");
	++callNum;
}

static void
signal_handler (__attribute__((unused)) int sig)
{
	gotAlarm_G = 1;
}
