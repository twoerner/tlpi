/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program uses sched_setscheduler() to set the policy and prioity of the
 * process specified on the cmdline.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

int
main (int argc, char *argv[])
{
	int i, policy;
	struct sched_param prio;

	if (argc < 3 || strchr ("rfo", argv[1][0]) == NULL) {
		printf ("usage: %s <policy> <priority> [<pid>...]\n", argv[0]);
		printf ("  where:\n");
		printf ("    <policy>   is one of 'r' (RR), 'f' (FIFO), \n");
		printf ("               "
#ifdef SCHED_BATCH
				        "'b' (BATCH), "
#endif
#ifdef SCHED_IDLE
					"'i' (IDLE), "
#endif
					"or 'o' (OTHER)\n");
		return 1;
	}

	policy = (argv[1][0] == 'r')? SCHED_RR :
		(argv[1][0] == 'f')? SCHED_FIFO :
#ifdef SCHED_BATCH
		(argv[1][0] == 'b')? SCHED_BATCH :
#endif
#ifdef SCHED_IDLE
		(argv[1][0] == 'i')? SCHED_IDLE :
#endif
		SCHED_OTHER;

	prio.sched_priority = atoi (argv[2]);

	for (i=3; i<argc; ++i)
		if (sched_setscheduler (atol (argv[3]), policy, &prio) == -1) {
			perror ("sched_setscheduler()");
			return 1;
		}

	return 0;
}
