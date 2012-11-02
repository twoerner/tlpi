/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Using a mutex to protect access to a global variable.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static volatile int global_G = 0;
static pthread_mutex_t mtx_G = PTHREAD_MUTEX_INITIALIZER;

static void *
thread_func (void *arg_p)
{
	int loops = *((int*)arg_p);
	int local, i, ret;

	for (i=0; i<loops; ++i) {
		ret = pthread_mutex_lock (&mtx_G);
		if (ret != 0) {
			perror ("pthread_mutex_lock()");
			exit (1);
		}

		local = global_G;
		++local;
		global_G = local;

		ret = pthread_mutex_unlock (&mtx_G);
		if (ret != 0) {
			perror ("pthread_mutex_unlock()");
			exit (1);
		}
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
