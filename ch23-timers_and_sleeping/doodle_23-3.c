/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Questions about clockid_t.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

int
main (void)
{
	clockid_t clockId;

	printf ("CLOCK_REALTIME............%ld\n", (long)CLOCK_REALTIME);
	printf ("CLOCK_MONOTONIC...........%ld\n", (long)CLOCK_MONOTONIC);
	printf ("CLOCK_PROCESS_CPUTIME_ID..%ld\n", (long)CLOCK_PROCESS_CPUTIME_ID);
	printf ("CLOCK_THREAD_CPUTIME_ID...%ld\n", (long)CLOCK_THREAD_CPUTIME_ID);

	clock_getcpuclockid (0, &clockId);
	printf ("clock ID of process 0.....%ld\n", (long)clockId);

	clock_getcpuclockid (getpid (), &clockId);
	printf ("clock ID of getpid()......%ld\n", (long)clockId);

	return 0;
}
