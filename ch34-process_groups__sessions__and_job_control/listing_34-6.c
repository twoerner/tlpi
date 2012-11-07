/*
 * Copyright (C) 2012  Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates a better way to handle SIGTSTP.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

static void
signal_handler (int sig)
{
	sigset_t newMask, oldMask;
	int savedErrno;
	struct sigaction sa;

	savedErrno = errno;

	printf ("caught %s (%d)\n", strsignal (sig), sig); // UNSAFE

	// set the disposition to default and raise
	// SIGTSTP so the default action (stop) can happen (eventually)
	if (signal (SIGTSTP, SIG_DFL) == SIG_ERR) {
		perror ("signal()");
		exit (1);
	}
	raise (SIGTSTP);

	// unblock SIGTSTP (which was blocked by default on entry)
	// the above pending raise(SIGTSTP) immediately suspends the pgm
	sigemptyset (&newMask);
	sigaddset (&newMask, SIGTSTP);
	sigprocmask (SIG_UNBLOCK, &newMask, &oldMask);

	// execution resumes here after SIGCONT
	sigprocmask (SIG_SETMASK, &oldMask, NULL);

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = signal_handler;
	sigaction (SIGTSTP, &sa, NULL);

	printf ("exiting SIGTSTP handler\n");
	errno = savedErrno;
}

int
main (void)
{
	struct sigaction sa;

	sigaction (SIGTSTP, NULL, &sa);
	if (sa.sa_handler != SIG_IGN) {
		sigemptyset (&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = signal_handler;
		sigaction (SIGTSTP, &sa, NULL);
	}

	for (;;) {
		pause ();
		printf ("main\n");
	}

	return 0;
}
