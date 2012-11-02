/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstrates the establishment and use of an alternate signal stack. After
 * establishing an alternate signal stack and a handler for SIGSEGV, this
 * program calls a function that infinitely recurses so that the stack,
 * eventually, overflows and the process is sent SIGSEGV.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

/* UNSAFE: uses printf(), strsignal(), fflush() */
static void
segv_handler (int sig)
{
	int x;

	printf ("caught signal %d (%s)\n", sig, strsignal (sig));
	printf ("  top of handler stack near  %10p\n", (void*)&x);
	fflush (NULL);

	_exit (EXIT_FAILURE);
}

static void
overflow_stack (int callNum)
{
	char a[100000];

	printf ("call %4d - top of stack near %10p\n", callNum, a);
	overflow_stack (callNum + 1);
}

int
main (void)
{
	stack_t sigstack;
	struct sigaction sa;
	int i;

	printf ("top of standard stack near    %10p\n", (void*)&i);

	// allocate alternate stack
	sigstack.ss_sp = malloc (SIGSTKSZ);
	if (sigstack.ss_sp == NULL) {
		perror ("malloc()");
		return 1;
	}
	sigstack.ss_size = SIGSTKSZ;
	sigstack.ss_flags = 0;
	if (sigaltstack (&sigstack, NULL) == -1) {
		perror ("sigaltstack()");
		return 1;
	}
	printf ("alternate stack is at        %10p-%p\n", sigstack.ss_sp, (char*)sbrk (0) - 1);

	// setup handler for SIGSEGV
	sa.sa_handler = segv_handler;
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_ONSTACK;
	if (sigaction (SIGSEGV, &sa, NULL) == -1) {
		perror ("sigaction()");
		return 1;
	}

	overflow_stack (1);
	return 0;
}
