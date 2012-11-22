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

static void
signal_handler (int sig)
{
	flag_G = 1;
	printf ("signal %d (%s) caught\n", sig, strsignal (sig));
	print_rlimit (RLIMIT_CPU);
}

int
main (void)
{
	int ret;
	struct sigaction sa;
	unsigned sig;
	struct rlimit rlim;
	volatile unsigned i, j;

	// setup signal handler
	flag_G = 0;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	for (sig=1; sig<_NSIG; ++sig) {
		ret = sigaction (sig, &sa, NULL);
		if (ret == -1)
			printf ("can't sigaction signal %2d (%s)\n", sig, strsignal (sig));
	}
	printf ("\n");

	// set rlimits
	printf ("before ");
	print_rlimit (RLIMIT_CPU);

	rlim.rlim_cur = 2;
	rlim.rlim_max = 8;
	ret = setrlimit (RLIMIT_CPU, &rlim);
	if (ret == -1) {
		perror ("setrlimit()");
		return 1;
	}

	printf ("after  ");
	print_rlimit (RLIMIT_CPU);

	// busy loop
	for (i=0; i<1000000; ++i)
		for (j=0; j<1000000; ++j)
			if (flag_G) {
				flag_G = 0;
				sleep (1);
			}

	return 0;
}
