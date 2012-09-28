/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrates signalfd() to read signals from a file descriptor.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/signalfd.h>

int
main (int argc, char *argv[])
{
	sigset_t mask;
	int fd, i;
	struct signalfd_siginfo sigInfo;
	ssize_t cnt;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<signal num>...]\n", argv[0]);
		return 0;
	}

	printf ("[%s] pid:%ld\n", argv[0], (long)getpid ());

	sigemptyset (&mask);
	for (i=1; i<argc; ++i)
		sigaddset (&mask, atoi (argv[i]));

	if (sigprocmask (SIG_BLOCK, &mask, NULL) == -1) {
		perror ("sigprocmask()");
		return 1;
	}

	fd = signalfd (-1, &mask, 0);
	if (fd == -1) {
		perror ("signalfd (-1)");
		return 1;
	}

	for (;;) {
		cnt = read (fd, &sigInfo, sizeof (sigInfo));
		if (cnt != sizeof (sigInfo)) {
			perror ("read()");
			return 1;
		}

		printf ("[%s] signal:%d (%s)", argv[0], sigInfo.ssi_signo, strsignal (sigInfo.ssi_signo));
		if (sigInfo.ssi_code == SI_QUEUE) {
			printf (" pid:%d int:%d\n", sigInfo.ssi_pid, sigInfo.ssi_int);
		}
		printf ("\n");
	}

	return 0;
}
