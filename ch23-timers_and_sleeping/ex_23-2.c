/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Try running the program in Listing 23-3 in the background with a 60-second
 * sleep interval, while using the following command to send as many SIGINT
 * signals as possible to the background process:
 *
 *	$ while true; do kill -INT <pid>; done
 *
 * You should observe that the program sleeps rather longer than expected.
 * Replace the use of nanosleep() with the use of clock_gettime() (use a
 * CLOCK_REALTIME clock) and clock_nanosleep() with the TIMER_ABSTIME flag.
 * (This exercise requires Linux 2.6). Repeat the test with the modified
 * program and explain the difference.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

static void
signal_handler (__attribute__((unused)) int sig)
{
	return;
}

int
main (int argc, char *argv[])
{
	struct timespec start, request, now;
	struct sigaction sa;
	int rtn;

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <secs> <nanosecs>\n", argv[0]);
		return 0;
	}

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGINT, &sa, NULL);

	clock_gettime (CLOCK_REALTIME, &start);
	request.tv_sec = start.tv_sec + atol (argv[1]);
	request.tv_nsec = start.tv_nsec + atol (argv[2]);

	for (;;) {
		rtn = clock_nanosleep (CLOCK_REALTIME, TIMER_ABSTIME, &request, NULL);
		if (rtn == -1 && errno != EINTR) {
			perror ("clock_gettime()");
			return 1;
		}

		clock_gettime (CLOCK_REALTIME, &now);
		printf ("slept for: %9.6f secs\n", (now.tv_sec - start.tv_sec) + ((now.tv_nsec - start.tv_nsec) / 1000000000.0));

		if (rtn == 0)
			break;

		printf ("remaining: %9.6f secs\n", (request.tv_sec - now.tv_sec) + ((request.tv_nsec - now.tv_nsec) / 1000000000.0));
	}

	printf ("sleep complete\n");
	return 0;
}
