/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program uses sched_getscheduler() and sched_getparam() to retrieve
 * the policy and priority of all the processes whose PIDs are given as
 * cmdline args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <sys/types.h>

int
main (int argc, char *argv[])
{
	int i, policy;
	struct sched_param prio;
	pid_t pid;

	for (i=1; i<argc; ++i) {
		pid = (pid_t)atol (argv[i]);

		policy = sched_getscheduler (pid);
		if (policy == -1) {
			perror ("sched_getscheduler()");
			return 1;
		}

		if (sched_getparam (pid, &prio) == -1) {
			perror ("sched_getparam()");
			return 1;
		}

		printf ("%s: %-5s %2d\n", argv[i],
				(policy == SCHED_OTHER)? "OTHER" :
				(policy == SCHED_RR)? "RR" :
				(policy == SCHED_FIFO)? "FIFO" :
#ifdef SCHED_BATCH
				(policy == SCHED_BATCH)? "BATCH" :
#endif
#ifdef SCHED_IDLE
				(policy == SCHED_IDLE)? "IDLE" :
#endif
				"???", prio.sched_priority);
	}

	return 0;
}
