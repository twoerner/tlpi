/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Cancelling a thread with pthread_cancel().
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

static void *
thread_func (__attribute__((unused)) void *arg_p)
{
	unsigned i = 0;

	printf ("new thread\n");
	while (1) {
		printf ("loop %u\n", i++);
		sleep (1);
	}

	return NULL;
}

int
main (void)
{
	pthread_t td;
	int ret;
	void *result_p;

	ret = pthread_create (&td, NULL, thread_func, NULL);
	if (ret != 0) {
		perror ("pthread_create()");
		return 1;
	}

	sleep (3);

	ret = pthread_cancel (td);
	if (ret != 0) {
		perror ("pthread_cancel()");
		return 1;
	}

	ret = pthread_join (td, &result_p);
	if (ret != 0) {
		perror ("pthread_join()");
		return 1;
	}

	if (result_p == PTHREAD_CANCELED)
		printf ("thread cancelled\n");
	else
		printf ("how'd we get here?!\n");

	return 0;
}
