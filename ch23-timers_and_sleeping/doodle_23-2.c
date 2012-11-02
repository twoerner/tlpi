/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * From man page for clock_getcpuclockid(3)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int
main (int argc, char *argv[])
{
	clockid_t clockId;
	struct timespec ts;

	if (argc != 2) {
		printf ("usage: %s <pid>\n", argv[0]);
		return 1;
	}

	if (clock_getcpuclockid (atoi (argv[1]), &clockId) != 0) {
		perror ("clock_getcpuclockid()");
		return 1;
	}

	if (clock_gettime (clockId, &ts) == -1) {
		perror ("clock_gettime()");
		return 1;
	}

	printf ("cpu-time clock for pid %s is %ld.%09ld [sec]\n", argv[1], (long)ts.tv_sec, (long)ts.tv_nsec);
	return 0;
}
