/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * A simple program using Pthreads. This program creates another thread then
 * joins with it.
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>

static void *
thread_func (void *arg_p)
{
	char *ptr = (char *)arg_p;

	printf ("%s", ptr);
	return (void*)strlen (ptr);
}

int
main (void)
{
	pthread_t t1;
	void *res_p;
	int ret;

	ret = pthread_create (&t1, NULL, thread_func, (void*)"Hello, world!\n");
	if (ret != 0) {
		perror ("pthread_create()");
		return 1;
	}

	printf ("message from %s()\n", __func__);
	ret = pthread_join (t1, &res_p);
	if (ret != 0) {
		perror ("pthread_join()");
		return 1;
	}

	printf ("thread returned %ld\n", (long)res_p);
	return 0;
}
