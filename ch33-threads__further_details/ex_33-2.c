/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Suppose that a thread creates a child using fork(). When the child
 * terminates, is it guarenteed that the resulting SIGCHLD signal will be
 * delivered to the thread that called fork() (as opposed to some other thread
 * in the process)?
 */

#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/syscall.h>

#define UNUSED __attribute__((unused))

static void
signal_handler (UNUSED int sig)
{
	printf ("SIGCHLD in %ld\n", (long)syscall (SYS_gettid)); // UNSAFE
}

static void *
thread_func (void *arg_p)
{
	uintptr_t idx = (uintptr_t)arg_p;

	printf ("thread:%tu ttid:%ld\n", idx, (long)syscall (SYS_gettid));
	if (idx == 2)
		if (fork() == 0) {
			sleep (1);
			_exit (0);
		}

	while (1)
		pause ();
}

int
main (void)
{
	uintptr_t idx;
	pthread_t threads[5];
	struct sigaction sa;

	// set signal disposition for SIGCHLD
	// i.e. all threads potentially handle signal delivery
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGCHLD, &sa, NULL);

	for (idx=0; idx<5; ++idx)
		pthread_create (&threads[idx], NULL, thread_func, (void*)idx);

	while (1)
		pause ();

	return 0;
}
