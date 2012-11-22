/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * RLIMIT_RTTIME: (since Linux 2.6.25) specifies the maximum amount of CPU time
 *                in microseconds that a process running under a realtime
 *                scheduling policy may consume without sleeping (i.e.
 *                performing a blocking system call). The behaviour if this
 *                limit is reached is the same as for RLIMIT_CPU: if the
 *                process reaches the soft limit, then SIGXCPU is sent to the
 *                process, and further SIGXCPU signals are sent for each
 *                additional second of CPU time consumed. On reaching the hard
 *                limit, SIGKILL is sent.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#include <sys/resource.h>
#include "print_rlimit.h"

volatile sig_atomic_t flag_G;

static void
signal_handler (int sig)
{
	flag_G = 1;
	printf ("signal %d (%s) caught\n", sig, strsignal (sig));
	print_rlimit (RLIMIT_RTTIME);
}

int
main (void)
{
	int ret;
	struct sigaction sa;
	struct rlimit rlim;
	struct sched_param prio;
	volatile unsigned i, j;

	// setup signal handler
	flag_G = 0;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	ret = sigaction (SIGXCPU, &sa, NULL);
	if (ret == -1) {
		printf ("can't sigaction signal %2d (%s)\n", SIGXCPU, strsignal (SIGXCPU));
		perror ("sigaction()");
		return 1;
	}

	// set rlimits
	printf ("before ");
	print_rlimit (RLIMIT_RTTIME);

	rlim.rlim_cur = 2 * 1000000;
	rlim.rlim_max = 8 * 1000000;
	ret = setrlimit (RLIMIT_RTTIME, &rlim);
	if (ret == -1) {
		perror ("setrlimit()");
		return 1;
	}

	printf ("after  ");
	print_rlimit (RLIMIT_RTTIME);

	// set -rt priority
	prio.sched_priority = 1;
	ret = sched_setscheduler (0, SCHED_FIFO, &prio);
	if (ret == -1) {
		perror ("sched_setscheduler()");
		return 1;
	}

	// busy loop
	for (i=0; i<1000000; ++i)
		for (j=0; j<1000000; ++j)
			if (flag_G) {
				flag_G = 0;
				fprintf (stderr, "starting sleep...");
				sleep (3);
				fprintf (stderr, "done\n");
			}

	return 0;
}
