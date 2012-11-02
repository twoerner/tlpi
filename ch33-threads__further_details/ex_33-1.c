/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program to demonstrate that different threads in the same process
 * can have different sets of pending signals, as returned by sigpending().
 * You can do this by using pthread_kill() to send different signals to two
 * different threads that have blocked these signals, and then have each of
 * the threads call sigpending().
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>

static int
print_pending_signals (uintptr_t idx)
{
	int sig, cnt;
	sigset_t pendingSignals;

	if (sigpending (&pendingSignals) == -1)
		return -1;

	cnt = 0;
	for (sig=1; sig<NSIG; ++sig) {
		if (sigismember (&pendingSignals, sig)) {
			++cnt;
			printf ("\t[%tu] %d (%s)\n", idx, sig, strsignal (sig));
		}
	}

	if (cnt == 0)
		printf ("[%tu] <empty signal set>\n", idx);

	return 0;
}

static void *
thread_func (void *arg_p)
{
	int ret;
	uintptr_t idx = (uintptr_t)arg_p;
	sigset_t newSet;

	switch (idx) {
		case 0:
			printf ("[%tu] blocking SIGINT\n", idx);
			ret = sigemptyset (&newSet);
			if (ret != 0) {
				perror ("sigemptyset()");
				exit (1);
			}
			ret = sigaddset (&newSet, SIGINT);
			if (ret != 0) {
				perror ("sigaddset()");
				exit (1);
			}
			ret = pthread_sigmask (SIG_BLOCK, &newSet, NULL);
			if (ret != 0) {
				perror ("pthread_sigmask()");
				exit (1);
			}
			break;

		case 1:
			printf ("[%tu] blocking SIGQUIT\n", idx);
			ret = sigemptyset (&newSet);
			if (ret != 0) {
				perror ("sigemptyset()");
				exit (1);
			}
			ret = sigaddset (&newSet, SIGQUIT);
			if (ret != 0) {
				perror ("sigaddset()");
				exit (1);
			}
			ret = pthread_sigmask (SIG_BLOCK, &newSet, NULL);
			if (ret != 0) {
				perror ("pthread_sigmask()");
				exit (1);
			}
			break;

		default:
			printf ("only two threads are supported (%tu)\n", idx);
			break;
	}

	sleep (3);
	print_pending_signals (idx);
	return NULL;
}

int
main (void)
{
	pthread_t td0, td1;
	int ret;

	ret = pthread_create (&td0, NULL, thread_func, (void*)0);
	if (ret != 0) {
		perror ("pthread_create(0)");
		return 1;
	}
	ret = pthread_create (&td1, NULL, thread_func, (void*)1);
	if (ret != 0) {
		perror ("pthread_create(1)");
		return 1;
	}

	sleep (1);

	ret = pthread_kill (td0, SIGINT);
	if (ret != 0) {
		perror ("pthread_kill(0)");
		return 1;
	}
	ret = pthread_kill (td1, SIGQUIT);
	if (ret != 0) {
		perror ("pthread_kill(1)");
		return 1;
	}

	ret = pthread_join (td0, NULL);
	if (ret != 0) {
		perror ("pthread_join(0)");
		return 1;
	}
	ret = pthread_join (td1, NULL);
	if (ret != 0) {
		perror ("pthread_join(1)");
		return 1;
	}

	return 0;
}
