/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * ch 35:
 * This program uses sched_setscheduler() to set the policy and prioity of the
 * process specified on the cmdline.
 *
 * ch 39:
 * Modify this program to use file capabilities so that it can be used by an
 * unprivileged user.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include "ch39lib.h"

int
main (int argc, char *argv[])
{
	int i, policy, ret;
	struct sched_param prio;

	ret = raise_cap (CAP_SYS_NICE);
	if (ret == -1) {
		perror ("raise_cap()");
		return 1;
	}

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
