/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program calls setrlimit() to set the soft and hard limits on the number
 * of processes a user may create (RLIMIT_NPROC), shows the limits both before
 * and after the change, and then creates as many processes as possible.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/resource.h>

static int print_rlimits (int resource);

int
main (int argc, char *argv[])
{
	struct rlimit rlim;
	int i;
	pid_t chldPid;
	unsigned long ultmp;
	long ltmp;
	int ret;

	if (argc < 2 || argc > 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <soft-limit> [<hard-limit>]\n", argv[0]);
		return 1;
	}

	printf ("RLIMIT_NPROC before: ");
	print_rlimits (RLIMIT_NPROC);
	memset (&rlim, 0, sizeof (rlim));

	// set soft limit
	if (argv[1][0] == 'i')
		ultmp = RLIM_INFINITY;
	else {
		ltmp = atol (argv[1]);
		if (ltmp < 0) {
			printf ("soft limit (%ld) must be .GT. zero\n", ltmp);
			return 1;
		}
		ultmp = (unsigned long)ltmp;
	}
	rlim.rlim_cur = ultmp;

	// set hard limit
	if (argc == 3) {
		if (argv[2][0] == 'i')
			ultmp = RLIM_INFINITY;
		else {
			ltmp = atol (argv[2]);
			if (ltmp < 0) {
				printf ("hard limit (%ld) must be .GT. zero\n", ltmp);
				return 1;
			}
			ultmp = (unsigned long)ltmp;
		}
		rlim.rlim_max = ultmp;
	}
	else
		rlim.rlim_max = rlim.rlim_cur;

	ret = setrlimit (RLIMIT_NPROC, &rlim);
	if (ret == -1) {
		perror ("setrlimit()");
		return 1;
	}

	printf ("RLIMIT_NPROC after:  ");
	print_rlimits (RLIMIT_NPROC);

	for (i=0; ; ++i) {
		switch ((chldPid = fork ())) {
			case -1:
				perror ("fork()");
				return 1;

			case 0: // child
				_exit (0);

			default: // parent
				printf ("child %3d (PID:%ld) started\n", i, (long)chldPid);
				break;
		}
	}

	return 0;
}

static int
print_rlimits (int resource)
{
	int ret;
	struct rlimit rlim;

	ret = getrlimit (resource, &rlim);
	if (ret == -1)
		return -1;

	printf ("soft:");
	if (rlim.rlim_cur == RLIM_INFINITY)
		printf ("infinite");
	else
		printf ("%lld", (long long)rlim.rlim_cur);

	printf ("; hard:");
	if (rlim.rlim_max == RLIM_INFINITY)
		printf ("infinite");
	else
		printf ("%lld\n", (long long)rlim.rlim_max);

	return 0;
}
