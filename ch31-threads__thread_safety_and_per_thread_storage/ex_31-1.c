/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Implement a function, one_time_init(control, init), that performs the
 * equivalent of pthread_once(). The control argument should be a pointer
 * to a statically allocated structure containing a boolean variable and a
 * mutex. The boolean indicates whether the function init has already been
 * called, and the mutex controls access to that variable. To keep the
 * implementation simple,  you can ignore possibilities such as init()
 * failing or being cancelled when first called from a thread (i.e. it is not
 * necessary to devise a scheme whereby if such an event occurs the next
 * thread to call one_time_init() reattempts the call to init()).
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

typedef struct {
	bool initialized;
	pthread_mutex_t mtx;
} Control_t;

static Control_t control_G = {
	false,
	PTHREAD_MUTEX_INITIALIZER,
};

static int
one_time_init (Control_t *control_p, void (*init_routine)(void))
{
	int ret;

	ret = pthread_mutex_lock (&(control_p->mtx));
	if (ret != 0) {
		perror ("pthread_mutex_lock(1)");
		exit (1);
	}

	if (!control_p->initialized) {
		control_p->initialized = true;
		init_routine ();
	}

	ret = pthread_mutex_unlock (&(control_p->mtx));
	if (ret != 0) {
		perror ("pthread_mutex_unlock(2)");
		exit (2);
	}

	return 0;
}

static void
my_init (void)
{
	fprintf (stderr, "hello\n");
}

static void *
thread_func (void *arg_p)
{
	uintptr_t td = (uintptr_t)arg_p;

	fprintf (stderr, "[%tu] thread starting\n", td);
	one_time_init (&control_G, my_init);
	return NULL;
}

int
main (void)
{
	int ret;
	uintptr_t i;
	pthread_t threads[5];

	for (i=0; i<5; ++i) {
		ret = pthread_create (&threads[i], NULL, thread_func, (void*)i);
		if (ret != 0) {
			perror ("pthread_create()");
			return i+1;
		}
	}
	for (i=0; i<5; ++i) {
		ret = pthread_join (threads[i], NULL);
		if (ret != 0) {
			perror ("pthread_join()");
			return i+1;
		}
	}

	return 0;
}
