/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * RLIMIT_RTTIME:
 *	(since Linux 2.6.25) specifies the maximum amount of CPU time
 *      in microseconds that a process running under a realtime
 *      scheduling policy may consume without sleeping (i.e.
 *      performing a blocking system call). The behaviour if this
 *      limit is reached is the same as for RLIMIT_CPU: if the
 *      process reaches the soft limit, then SIGXCPU is sent to the
 *      process, and further SIGXCPU signals are sent for each
 *      additional second of CPU time consumed. On reaching the hard
 *      limit, SIGKILL is sent.
 *
 * RLIMIT_CPU:
 *	specifies the maximum number of seconds of CPU time (in both
 *      system and user mode) that can be used by a process. SUSv3
 *      requires that the SIGXCPU (24) signal be sent to the process
 *      when the soft limit is reached, but leaves other details
 *      unspecified.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#include <sys/resource.h>

// set to 0 to indicate RLIMIT_CPU
// set to 15 to indicate RLIMIT_RTTIME
#define WHICH_LIMIT 15

static void print_rlimit (int resource);

static volatile sig_atomic_t rlimSet_G = 0;
static volatile sig_atomic_t resetLimits_G = 0;
static struct rlimit rlim_G;

#if (WHICH_LIMIT == 0)
const char *limStr_pG = "RLIMIT_CPU";
#else
const char *limStr_pG = "RLIMIT_RTTIME";
#endif

// UNSAFE
static void
signal_handler (int sig)
{
	printf ("\n\nsignal %d (%s) caught\n", sig, strsignal (sig));
	print_rlimit (WHICH_LIMIT);

	if (resetLimits_G == 1)
		if (rlimSet_G == 1) {
			fprintf (stderr, "resetting %s\n", limStr_pG);
			setrlimit (WHICH_LIMIT, &rlim_G);
			print_rlimit (WHICH_LIMIT);
		}
}

int
main (int argc, __attribute__((unused)) char *argv[])
{
	int ret;
	struct sigaction sa;
	struct sched_param prio;
	volatile unsigned i, j;

	// check cmdline args
	if (argc > 1)
		resetLimits_G = 1;

	// setup signal handler
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
	print_rlimit (WHICH_LIMIT);

#if (WHICH_LIMIT == 0)
	rlim_G.rlim_cur = 2;
	rlim_G.rlim_max = 8;
#else
	rlim_G.rlim_cur = 2 * 1000000;
	rlim_G.rlim_max = 8 * 1000000;
#endif
	ret = setrlimit (WHICH_LIMIT, &rlim_G);
	if (ret == -1) {
		perror ("setrlimit()");
		return 1;
	}
	rlimSet_G = 1;

	printf ("after  ");
	print_rlimit (WHICH_LIMIT);

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
			;

	return 0;
}

static void
print_rlimit (int resource)
{
	int ret;
	struct rlimit rlim;
	static struct {
		int resource;
		const char *str_p;
	} LimitTable[] = {
		{RLIMIT_AS, "RLIMIT_AS"},
		{RLIMIT_CORE, "RLIMIT_CORE"},
		{RLIMIT_CPU, "RLIMIT_CPU"},
		{RLIMIT_DATA, "RLIMIT_DATA"},
		{RLIMIT_FSIZE, "RLIMIT_FSIZE"},
		{RLIMIT_LOCKS, "RLIMIT_LOCKS"},
		{RLIMIT_MEMLOCK, "RLIMIT_MEMLOCK"},
		{RLIMIT_MSGQUEUE, "RLIMIT_MSGQUEUE"},
		{RLIMIT_NICE, "RLIMIT_NICE"},
		{RLIMIT_NOFILE, "RLIMIT_NOFILE"},
		{RLIMIT_NPROC, "RLIMIT_NPROC"},
		{RLIMIT_RSS, "RLIMIT_RSS"},
		{RLIMIT_RTPRIO, "RLIMIT_RTPRIO"},
		{RLIMIT_RTTIME, "RLIMIT_RTTIME"},
		{RLIMIT_SIGPENDING, "RLIMIT_SIGPENDING"},
		{RLIMIT_STACK, "RLIMIT_STACK"},
	};
	size_t tblSz = sizeof (LimitTable) / sizeof (LimitTable[0]);
	size_t i;

	for (i=0; i<tblSz; ++i) {
		if (LimitTable[i].resource == resource) {
			printf ("%s: ", LimitTable[i].str_p);
			break;
		}
	}
	if (i >= tblSz)
		printf ("resource %d (unknown): ", resource);

	// get limit
	ret = getrlimit (resource, &rlim);
	if (ret == -1) {
		perror ("getrlimit()");
		return;
	}

	// soft
	printf ("soft=");
	if (rlim.rlim_cur == RLIM_INFINITY)
		printf ("infinite");
	else
		printf ("%lld", (long long)rlim.rlim_cur);
	printf (" ");

	printf ("hard=");
	if (rlim.rlim_max == RLIM_INFINITY)
		printf ("infinite");
	else
		printf ("%lld", (long long)rlim.rlim_max);
	printf ("\n");
}
