/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Non-thread-safe implementation of strerror ().
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#define MAX_ERROR_LEN 256
static __thread char buf_G[MAX_ERROR_LEN];

static char *
my_strerror (int err)
{
	if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL)
		snprintf (buf_G, sizeof (buf_G), "unknown error %d", err);
	else {
		strncpy (buf_G, _sys_errlist[err], MAX_ERROR_LEN - 1);
		buf_G[MAX_ERROR_LEN - 1] = 0;
	}

	return buf_G;
}

static void *
thread_func (__attribute__((unused)) void *arg)
{
	char *str_p;

	printf ("other thread about to call strerror()\n");
	str_p = my_strerror (EPERM);
	printf ("other thread: str (%p) = %s\n", str_p, str_p);

	return NULL;
}

int
main (void)
{
	pthread_t t;
	int ret;
	char *str_p;

	str_p = my_strerror (EINVAL);
	printf ("main thread has called strerror()\n");

	ret = pthread_create (&t, NULL, thread_func, NULL);
	if (ret != 0) {
		perror ("pthread_create()");
		return 1;
	}

	ret = pthread_join (t, NULL);
	if (ret != 0) {
		perror ("pthread_join()");
		return 1;
	}

	printf ("main thread: str (%p) = %s\n", str_p, str_p);
	return 0;
}
