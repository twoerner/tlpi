/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Thread-safe implementation of strerror () using thread-specific data.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

static pthread_once_t once_G = PTHREAD_ONCE_INIT;
static pthread_key_t strerrorKey_G;

#define MAX_ERROR_LEN 256

static void
destructor (void *buf_p)
{
	printf ("destructor: freeing buf (%p)\n", buf_p);
	free (buf_p);
}

static void
create_key (void)
{
	int ret;

	ret = pthread_key_create (&strerrorKey_G, destructor);
	if (ret != 0) {
		perror ("pthread_key_create()");
		exit (1);
	}
}

static char *
my_strerror (int err)
{
	int ret;
	char *buf_p;

	ret = pthread_once (&once_G, create_key);
	if (ret != 0) {
		perror ("pthread_once()");
		exit (1);
	}

	buf_p = pthread_getspecific (strerrorKey_G);
	if (buf_p == NULL) {
		buf_p = malloc (MAX_ERROR_LEN);
		if (buf_p == NULL) {
			perror ("malloc()");
			exit (1);
		}

		ret = pthread_setspecific (strerrorKey_G, buf_p);
		if (ret != 0) {
			perror ("pthread_setspecific()");
			exit (1);
		}
	}

	if (err < 0 || err >= _sys_nerr || _sys_errlist[err] == NULL)
		snprintf (buf_p, sizeof (buf_p), "unknown error %d", err);
	else {
		strncpy (buf_p, _sys_errlist[err], MAX_ERROR_LEN - 1);
		buf_p[MAX_ERROR_LEN - 1] = 0;
	}

	return buf_p;
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
