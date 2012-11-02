/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * A main thread that can join with any terminated thread.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

static pthread_cond_t threadDied_G = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex_G = PTHREAD_MUTEX_INITIALIZER;

static unsigned threadCnt_G = 0;
static unsigned liveCnt_G = 0;
static unsigned unjoinedCnt_G = 0;

enum Tstate_e {
	TS_ALIVE,
	TS_TERMINATED,
	TS_JOINED,
};

typedef struct {
	pthread_t tid;
	enum Tstate_e state;
	int sleepTime;
} Thread_t;
static Thread_t *tArr_pG;

static void *
thread_func (void *arg_p)
{
	uintptr_t idx = (uintptr_t)arg_p;
	int ret;

	sleep (tArr_pG[idx].sleepTime);
	printf ("thread %tu terminating\n", idx);

	ret = pthread_mutex_lock (&threadMutex_G);
	if (ret != 0) {
		perror ("pthread_mutex_lock()");
		exit (1);
	}

	++unjoinedCnt_G;
	tArr_pG[idx].state = TS_TERMINATED;

	ret = pthread_mutex_unlock (&threadMutex_G);
	if (ret != 0) {
		perror ("pthread_mutex_unlock()");
		exit (2);
	}

	ret = pthread_cond_signal (&threadDied_G);
	if (ret != 0) {
		perror ("pthread_cond_signal()");
		exit (3);
	}

	return NULL;
}

int
main (int argc, char *argv[])
{
	uintptr_t idx;
	int ret;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <nsecs>...\n", argv[0]);
		return 1;
	}

	tArr_pG = calloc (argc - 1, sizeof (Thread_t));
	if (tArr_pG == NULL) {
		perror ("calloc()");
		return 1;
	}

	// create all threads
	for (idx=0; idx<(uintptr_t)(argc-1); ++idx) {
		tArr_pG[idx].sleepTime = atoi (argv[idx + 1]);
		if (tArr_pG[idx].sleepTime <= 0) {
			printf ("please specify positive sleep times %d[%tu]\n", tArr_pG[idx].sleepTime, idx);
			return 1;
		}
		tArr_pG[idx].state = TS_ALIVE;
		ret = pthread_create (&tArr_pG[idx].tid, NULL, thread_func, (void*)idx);
		if (ret != 0) {
			perror ("pthread_create()");
			return 1;
		}
	}

	threadCnt_G = argc - 1;
	liveCnt_G = threadCnt_G;

	// join with terminated threads
	while (liveCnt_G > 0) {
		ret = pthread_mutex_lock (&threadMutex_G);
		if (ret != 0) {
			perror ("ptrhead_mutex_lock()");
			return 1;
		}

		while (unjoinedCnt_G == 0) {
			ret = pthread_cond_wait (&threadDied_G, &threadMutex_G);
			if (ret != 0) {
				perror ("pthread_cond_wait()");
				return 1;
			}
		}

		for (idx=0; idx<threadCnt_G; ++idx) {
			if (tArr_pG[idx].state == TS_TERMINATED) {
				ret = pthread_join (tArr_pG[idx].tid, NULL);
				if (ret != 0) {
					perror ("pthread_join()");
					return 1;
				}

				tArr_pG[idx].state = TS_JOINED;
				--liveCnt_G;
				--unjoinedCnt_G;

				printf ("reaped thread %tu (liveCnt_G:%d)\n", idx, liveCnt_G);
			}
		}

		ret = pthread_mutex_unlock (&threadMutex_G);
		if (ret != 0) {
			perror ("pthread_mutex_unlock()");
			return 1;
		}
	}

	free (tArr_pG);
	return 0;
}
