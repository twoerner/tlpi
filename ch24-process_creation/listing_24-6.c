/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Avoiding a race condition after a fork() by synchronizing with a signal.
 */

// set to 0 or 1
#define USE_HANDLER 0

#include <stdio.h>
#if (USE_HANDLER == 1)
#include <string.h>
#endif
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#if (USE_HANDLER == 1)
static void
signal_handler (int sig)
{
	fprintf (stderr, "{got %s}\n", strsignal (sig));
}
#endif

int
main (void)
{
	setbuf (stdout, NULL);
	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			printf ("[%ld] child starting\n", (long)getpid ());
			sleep (2);
			printf ("[%ld] child signaling parent\n", (long)getpid ());
			kill (getppid (), SIGUSR1);
			_exit (0);

		default: {// parent
			sigset_t sigMask;
#if (USE_HANDLER == 1)
			struct sigaction sa;
#endif

			printf ("[%ld] parent starting\n", (long)getpid ());

			sigemptyset (&sigMask);
			sigaddset (&sigMask, SIGUSR1);
#if (USE_HANDLER == 1)
			sigemptyset (&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = signal_handler;
			sigaction (SIGUSR1, &sa, NULL);
#else
			sigprocmask (SIG_BLOCK, &sigMask, NULL);
#endif

			printf ("[%ld] parent waiting for signal\n", (long)getpid ());
			sigwaitinfo (&sigMask, NULL);
			printf ("[%ld] parent after signal\n", (long)getpid ());
		}
	}

	return 0;
}
