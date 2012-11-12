/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program to verify that if one of the signals SIGTTIN, SIGTTOU, or
 * SIGTSTP is sent to a member of an orphaned process group, then the signal
 * is discarded (i.e. has no effect) if it would stop the process (i.e. the
 * disposition is SIG_DFL), but is delivered if a handler is installed for the
 * signal.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#define CATCH_SIG 0

#if (CATCH_SIG == 1)
static void
signal_handler (int sig)
{
	printf ("caught signal %d: %s\n", sig, strsignal (sig));
}
#endif

int
main (void)
{
	pid_t chldPid;

	switch ((chldPid = fork ())) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: { // child
#if (CATCH_SIG == 1)
			struct sigaction sa;

			sigemptyset (&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = signal_handler;
			sigaction (SIGTTIN, &sa, NULL);
			sigaction (SIGTTOU, &sa, NULL);
			sigaction (SIGTSTP, &sa, NULL);
#endif

			sleep (5);
			printf ("hello, world\n");
			_exit (0);
			break; }

		default:
			break;
	}

	sleep (2);
	kill (chldPid, SIGTSTP);
	printf ("parent exiting\n");

	return 0;
}
