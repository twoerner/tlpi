/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates that when the shell receives SIGHUP, it in turn
 * sends SIGHUP to the jobs it has created. If the program is given an
 * optional cmdline arg (anything will do), the child places itself in a
 * different process group (within the same session) to demonstrate the shell
 * doesn't send SIGHUP to a process group it did not create (even if it is in
 * the same session as the shell).
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))

static void
signal_handler (int sig)
{
	printf ("%ld: caught %s\n", (long)getpid (), strsignal (sig));
}

int
main (int argc, UNUSED char *argv[])
{
	pid_t chldPid;
	struct sigaction sa;

	setbuf (stdout, NULL);

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGHUP, &sa, NULL);

	chldPid = fork ();
	if (chldPid == -1) {
		perror ("fork()");
		return 1;
	}

	if (chldPid == 0 && argc > 1)
		if (setpgid (0, 0) == -1) {
			perror ("getpgid()");
			return 1;
		}

	printf ("PID:%ld PPID:%ld PGID:%ld SID:%ld\n", (long)getpid (), (long)getppid (), (long)getpgrp (), (long)getsid (0));
	alarm (60);

	pause ();
	if (chldPid == 0)
		_exit (0);

	return 0;
}
