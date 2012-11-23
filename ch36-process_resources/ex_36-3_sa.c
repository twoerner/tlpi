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
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sched.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/resource.h>

// function declarations
static void signal_handler (int sig);
static void print_rlimit (int resource);
static void process_cmdline_args (int argc, char *argv[]);
static void usage (const char *pgm_p);

// global variables
static volatile sig_atomic_t flag_G;
static volatile sig_atomic_t rlimSet_G = 0;
static volatile sig_atomic_t resetLimits_G = 0;
static volatile sig_atomic_t runBlockingCall_G = 0;
static struct rlimit rlim_G;
static int limit_G;
static const char *limStr_pG = NULL;

// functions
int
main (int argc, char *argv[])
{
	int ret;
	struct sigaction sa;
	struct sched_param prio;
	volatile unsigned i, j;

	process_cmdline_args (argc, argv);

	// setup signal handler
	if (runBlockingCall_G)
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
	print_rlimit (limit_G);

	if (limit_G == RLIMIT_CPU) {
		rlim_G.rlim_cur = 2;
		rlim_G.rlim_max = 8;
	}
	else {
		rlim_G.rlim_cur = 2 * 1000000;
		rlim_G.rlim_max = 8 * 1000000;
	}
	ret = setrlimit (limit_G, &rlim_G);
	if (ret == -1) {
		perror ("setrlimit()");
		return 1;
	}
	rlimSet_G = 1;

	printf ("after  ");
	print_rlimit (limit_G);

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
			if (runBlockingCall_G == 1) {
				if (flag_G) {
					flag_G = 0;
					usleep (1);
				}
			}
			else {
				;
			}

	return 0;
}

// UNSAFE
static void
signal_handler (int sig)
{
	if (runBlockingCall_G)
		flag_G = 1;

	printf ("\n\nsignal %d (%s) caught\n", sig, strsignal (sig));
	print_rlimit (limit_G);

	if (resetLimits_G == 1)
		if (rlimSet_G == 1) {
			fprintf (stderr, "resetting %s\n", limStr_pG);
			setrlimit (limit_G, &rlim_G);
			print_rlimit (limit_G);
		}
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

static void
process_cmdline_args (int argc, char *argv[])
{
	int c, idx;
	struct option opts[] = {
		{"reset-limits", no_argument, 0, 0},
		{"run-blocking-call", no_argument, 0, 0},
		{NULL, 0, 0, 0},
	};

	while (1) {
		c = getopt_long (argc, argv, "lb", opts, &idx);
		if (c == -1)
			break;
		if (c != 0) {
			printf ("\n");
			usage (argv[0]);
			exit (1);
		}
		switch (idx) {
			case 0:
				resetLimits_G = 1;
				break;

			case 1:
				runBlockingCall_G = 1;
				break;

			default:
				printf ("\n");
				usage (argv[0]);
				exit (1);
				break;
		}
	}

	// required cmdline arg
	if ((optind + 1) == argc) {
		if (strcmp (argv[optind], "RLIMIT_RTTIME") == 0) {
			limStr_pG = "RLIMIT_RTTIME";
			limit_G = RLIMIT_RTTIME;
		}
		if (strcmp (argv[optind], "RLIMIT_CPU") == 0) {
			limStr_pG = "RLIMIT_CPU";
			limit_G = RLIMIT_CPU;
		}
	}
	else {
		printf ("incorrect number of cmdline args\n\n");
		usage (argv[0]);
		exit (1);
	}

	if (limStr_pG == NULL) {
		printf ("required cmdline arg not one of 'RLIMIT_RTTIME' or 'RLIMIT_CPU'\n\n");
		usage (argv[0]);
		exit (1);
	}
}

static void
usage (const char *pgm_p)
{
	printf ("usage: %s [OPTIONS] <limit>\n", pgm_p);
	printf ("  where:\n");
	printf ("    OPTIONS:\n");
	printf ("      --reset-limits        reset the soft limit during the\n");
	printf ("                            interrupt handler\n");
	printf ("      --run-blocking-call   when the interrupt handler is called,\n");
	printf ("                            call a blocking system function\n");
	printf ("    <limit> is one of:\n");
	printf ("      RLIMIT_CPU            limit the amount of CPU time\n");
	printf ("      RLIMIT_RTTIME         limit the amount of non-sleeping CPU time\n");
}
