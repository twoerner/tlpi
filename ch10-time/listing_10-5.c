/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the usage of clock() and times().
 * The display_process_times() function prints the supplied message and then uses
 * clock() and times() to retrieve and display process times. The main program
 * makes an initial call to display_process_times() and then executes a loop
 * that consumes some CPU time by calling getppid() repeatedly before again calling
 * display_process_times() to show how much CPU time has elapsed. The user can
 * optionally specify the number of iterations to perform.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

static void
display_process_times (const char *msg_p)
{
	struct tms t;
	clock_t clockTime;
	static long clockTicks = 0;

	if (msg_p != NULL)
		printf ("%s", msg_p);

	if (clockTicks == 0) {
		clockTicks = sysconf (_SC_CLK_TCK);
		if (clockTicks == -1) {
			perror ("sysconf (_SC_CLK_TCK)");
			exit (1);
		}
	}

	clockTime = clock ();
	if (clockTime == -1) {
		perror ("clock()");
		exit (1);
	}

	printf ("\tclock() returns: %ld clocks-per-sec (%.2f secs)\n",
			(long)clockTime, (double)clockTime / CLOCKS_PER_SEC);

	if (times (&t) == -1) {
		perror ("times()");
		exit (1);
	}
	printf ("\ttimes() yields: user CPU=%.2f; system CPU:%.2f\n",
			(double)t.tms_utime / clockTicks,
			(double)t.tms_stime / clockTicks);
}

int
main (int argc, char *argv[])
{
	int numCalls, i;

	printf ("CLOCK_PER_SEC=%ld sysconf(_SC_CLK_TCK)=%ld\n\n",
			(long)CLOCKS_PER_SEC, sysconf (_SC_CLK_TCK));
	display_process_times ("start:\n");
	numCalls = argc > 1? atoi (argv[1]) : 100000000;
	for (i=0; i<numCalls; ++i)
		getppid ();
	display_process_times ("after loop:\n");

	return 0;
}
