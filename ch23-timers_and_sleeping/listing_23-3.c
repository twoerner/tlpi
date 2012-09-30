/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrate nanosleep().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/time.h>

static void
signal_handler (__attribute__((unused)) int sig)
{
	return;
}

int
main (int argc, char *argv[])
{
	struct timeval start, finish;
	struct timespec request, remain;
	struct sigaction sa;
	int rtn;

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <secs> <nanosecs>\n", argv[0]);
		return 0;
	}

	request.tv_sec = atol (argv[1]);
	request.tv_nsec = atol (argv[2]);

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGINT, &sa, NULL);

	gettimeofday (&start, NULL);

	for (;;) {
		rtn = nanosleep (&request, &remain);
		if (rtn == -1 && errno != EINTR) {
			perror ("nanosleep()");
			return 1;
		}

		gettimeofday (&finish, NULL);
		printf ("slept for: %9.6f secs\n", finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec) / 1000000.0);

		if (rtn == 0)
			break;

		printf ("remaining: %2ld.%09ld\n", (long)remain.tv_sec, remain.tv_nsec);
		request = remain;
	}

	printf ("sleep complete\n");
	return 0;
}
