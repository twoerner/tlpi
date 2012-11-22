/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * RLIMIT_CPU: specifies the maximum number of seconds of CPU time (in both
 *             system and user mode) that can be used by a process. SUSv3
 *             requires that the SIGXCPU (24) signal be sent to the process
 *             when the soft limit is reached, but leaves other details
 *             unspecified.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/resource.h>
#include "print_rlimit.h"

volatile sig_atomic_t flag_G;
volatile sig_atomic_t rlimSet_G = 0;
struct rlimit rlim_G;
volatile sig_atomic_t resetLimits_G = 0;

static void
signal_handler (int sig)
{
	flag_G = 1;
	printf ("\nsignal %d (%s) caught\n", sig, strsignal (sig));
	print_rlimit (RLIMIT_CPU);

	if (resetLimits_G)
		if (rlimSet_G == 1)
			setrlimit (RLIMIT_CPU, &rlim_G);
}

int
main (int argc, __attribute__((unused)) char *argv[])
{
	int ret;
	struct sigaction sa;
	volatile unsigned i, j;

	// check cmdline args
	if (argc > 1)
		resetLimits_G = 1;

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
	print_rlimit (RLIMIT_CPU);

	rlim_G.rlim_cur = 2;
	rlim_G.rlim_max = 8;
	ret = setrlimit (RLIMIT_CPU, &rlim_G);
	if (ret == -1) {
		perror ("setrlimit()");
		return 1;
	}
	rlimSet_G = 1;

	printf ("after  ");
	print_rlimit (RLIMIT_CPU);

	// busy loop
	for (i=0; i<1000000; ++i) {
		fprintf (stderr, ".");
		for (j=0; j<1000000; ++j)
			if (flag_G) {
				flag_G = 0;
				sleep (1);
			}
	}

	return 0;
}
