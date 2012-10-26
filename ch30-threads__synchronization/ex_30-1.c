/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Incorrectly incrementing a global variable from two threads
 * (with updates as per exercise 30-1).
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static volatile int global_G = 0;
static int loops_G = 10000000;

static void *
thread_func (void *arg_p)
{
	int local, i;
	long identifier = (long)arg_p;

	for (i=0; i<loops_G; ++i) {
		printf ("[%ld] glob:%d\n", identifier, global_G);
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
	long identifier;

	if (argc > 1) {
		loops = atoi (argv[1]);
		if (loops < 1) {
			printf ("please specify a positive loop value (%d)\n", loops);
			return 1;
		}
		loops_G = loops;
	}

	identifier = 1;
	ret = pthread_create (&t1, NULL, thread_func, (void*)identifier);
	if (ret != 0) {
		perror ("pthread_create (t1)");
		return 1;
	}
	identifier = 2;
	ret = pthread_create (&t2, NULL, thread_func, (void*)identifier);
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
