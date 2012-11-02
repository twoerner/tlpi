/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program provides a simple example of the use of a cleanup handler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>

static pthread_cond_t cond_G = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx_G = PTHREAD_MUTEX_INITIALIZER;
static bool global_G = false;

static void
cleanup (void *arg_p)
{
	int ret;

	printf ("%s: freeling %p\n", __func__, arg_p);
	free (arg_p);

	printf ("%s: unlocking mutex\n", __func__);
	ret = pthread_mutex_unlock (&mtx_G);
	if (ret != 0) {
		perror ("pthread_mutex_unlock()");
		exit (1);
	}
}

static void *
thread_func (__attribute__((unused)) void *arg_p)
{
	int ret;
	void *buf_p = NULL;

	buf_p = malloc (0x10000);
	if (buf_p == NULL) {
		perror ("malloc()");
		exit (1);
	}
	printf ("%s: allocated memory %p\n", __func__, buf_p);

	ret = pthread_mutex_lock (&mtx_G);
	if (ret != 0) {
		perror ("pthread_mutex_lock()");
		exit (1);
	}

	pthread_cleanup_push (cleanup, buf_p);

	while (!global_G) {
		ret = pthread_cond_wait (&cond_G, &mtx_G);
		if (ret != 0) {
			perror ("pthread_cond_wait()");
			exit (1);
		}
	}

	printf ("%s: condition loop wait completed\n", __func__);
	pthread_cleanup_pop (1);
	return NULL;
}

int
main (int argc, __attribute__((unused)) char *argv[])
{
	pthread_t td;
	void *result_p;
	int ret;

	ret = pthread_create (&td, NULL, thread_func, NULL);
	if (ret != 0) {
		perror ("pthread_create()");
		return 1;
	}

	sleep (2);

	if (argc == 1) {
		printf ("%s: about to cancel\n", __func__);
		ret = pthread_cancel (td);
		if (ret != 0) {
			perror ("pthread_cancel()");
			return 1;
		}
	}
	else {
		printf ("%s: about to signal cond variable\n", __func__);
		global_G = true;
		ret = pthread_cond_signal (&cond_G);
		if (ret != 0) {
			perror ("pthread_cond_signal()");
			return 1;
		}
	}

	ret = pthread_join (td, &result_p);
	if (ret != 0) {
		perror ("pthread_join()");
		return 1;
	}
	if (result_p == PTHREAD_CANCELED)
		printf ("%s: thread cancelled\n", __func__);
	else
		printf ("%s: thread terminated normally\n", __func__);

	return 0;
}
