/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates that termination of the controlling process
 * causes a SIGHUP signal to be sent to all members of the terminal's
 * foreground process group.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


static void
signal_handler (int sig)
{
	printf ("%ld: caught %s\n", (long)getpid (), strsignal (sig));
}

int
main (int argc, char *argv[])
{
	pid_t parentPid, chldPid;
	int i;
	struct sigaction sa;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s {d|s}... [ > <logfile> 2>&1 ]\n", argv[0]);
		return 1;
	}

	setbuf (stdout, NULL);

	parentPid = getpid ();
	printf ("parent PID...................%ld\n", (long)parentPid);
	printf ("foreground process group ID..%ld\n", (long)tcgetpgrp (STDIN_FILENO));

	for (i=0; i<argc; ++i) {
		chldPid = fork ();
		if (chldPid == -1) {
			perror ("fork()");
			return 1;
		}

		if (chldPid == 0) {
			if (argv[i][0] == 'd')
				setpgid (0, 0);
			sigemptyset (&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = signal_handler;
			sigaction (SIGHUP, &sa, NULL);
			break;
		}
	}

	alarm (60);

	printf ("PID:%ld PGID:%ld\n", (long)getpid (), (long)getpgrp ());
	for (;;)
		pause ();

	return 0;
}
