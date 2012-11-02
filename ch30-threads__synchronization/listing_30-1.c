/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Incorrectly incrementing a global variable from two threads.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static volatile int global_G = 0;

static void *
thread_func (void *arg_p)
{
	int loops = *((int*)arg_p);
	int local, i;

	for (i=0; i<loops; ++i) {
		local = global_G;
		++local;
		global_G = local;
	}

	return NULL;
}

int
main (int argc, char *argv[])
{
	pthread_t t1, t2;
	int loops, ret;

	loops = (argc > 1)? atoi (argv[1]) : 10000000;
	if (loops < 1) {
		printf ("please specify a positive loop value\n");
		return 1;
	}

	ret = pthread_create (&t1, NULL, thread_func, &loops);
	if (ret != 0) {
		perror ("pthread_create (t1)");
		return 1;
	}
	ret = pthread_create (&t2, NULL, thread_func, &loops);
	if (ret != 0) {
		perror ("pthread_create (t2)");
		return 1;
	}

	ret = pthread_join (t1, NULL);
	if (ret != 0) {
		perror ("pthread_join (t1)");
		return 1;
	}
	ret = pthread_join (t2, NULL);
	if (ret != 0) {
		perror ("pthread_join (t2)");
		return 1;
	}

	printf ("global_G: %d\n", global_G);
	return 0;
}
