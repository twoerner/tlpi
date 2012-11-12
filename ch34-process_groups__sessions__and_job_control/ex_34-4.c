/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Modify the program in Listing 34-4 to verify that, if a controlling process
 * doesn't terminate as a consequence of receiving SIGHUP, then the kernel
 * doesn't send SIGHUP to the members of the foreground process.
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

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGHUP, &sa, NULL);

	for (i=0; i<argc; ++i) {
		chldPid = fork ();
		if (chldPid == -1) {
			perror ("fork()");
			return 1;
		}

		if (chldPid == 0) {
			if (i == 0)
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
