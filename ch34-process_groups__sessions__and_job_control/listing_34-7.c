/*
 * Copyright (C) 2012  Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the treatment of orphaned processes
 * (process groups?)
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

// UNSAFE: printf()
static void
signal_handler (int sig)
{
	printf ("pid:%ld caught signal %s (%d)\n", (long)getpid (), strsignal (sig), sig);
}

int
main (int argc, char *argv[])
{
	int i;
	struct sigaction sa;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s {s|p}...\n", argv[0]);
		return 1;
	}

	setbuf (stdout, NULL);

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGHUP, &sa, NULL);
	sigaction (SIGCONT, &sa, NULL);

	printf ("[parent] pid:%ld ppid:%ld pgid:%ld sid:%ld\n", (long)getpid (), (long)getppid (), (long)getpgrp (), (long)getsid (0));

	// create one child for each cmdline arg
	for (i=1; i<argc; ++i) {
		switch (fork ()) {
			case -1:
				perror ("fork()");
				return 1;

			case 0: // child
				printf ("[child]  pid:%ld ppid:%ld pgid:%ld sid:%ld\n", (long)getpid (), (long)getppid (), (long)getpgrp (), (long)getsid (0));
				if (argv[i][0] == 's') {
					printf ("pid:%ld stopping\n", (long)getpid ());
					raise (SIGSTOP);
				}
				else {
					alarm (60);
					printf ("pid:%ld pausing\n", (long)getpid ());
					pause ();
				}

				_exit (0);

			default:
				break;
		}
	}

	sleep (3);
	printf ("parent exiting\n");
	return 0;
}
