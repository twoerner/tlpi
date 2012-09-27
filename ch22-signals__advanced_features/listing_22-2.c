/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates sending a realtime signal using sigqueue().
 * It takes up to 4 cmdline args, of which the first 3 are mandatory:
 *	- target PID
 *	- a signal number
 *	- an integer value for the signal data
 * If more than one instance of the specified signal is to be sent, the
 * optional 4th cmdline arg specifies the number of instances (in this
 * case the accompanying integer is incremented by one for each successive
 * signal.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

int
main (int argc, char *argv[])
{
	int sig, numSigs, i, sigData;
	pid_t pid;
	union sigval sv;

	if (argc < 4 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pid> <signal> <data> [<number>]\n", argv[0]);
		return 0;
	}

	printf ("%s: PID:%ld UID:%ld\n", argv[0], (long)getpid (), (long)getuid ());

	pid = (pid_t)atoi (argv[1]);
	if (pid < 2) {
		printf ("bad pid: %d\n", pid);
		return 1;
	}
	sig = atoi (argv[2]);
	if (sig <= 0) {
		printf ("bad signal: %d\n", sig);
		return 1;
	}
	sigData = atoi (argv[3]);
	if (argc > 4) {
		numSigs = atoi (argv[4]);
		if (numSigs < 1)
			numSigs = 1;
	}
	else
		numSigs = 1;

	for (i=0; i<numSigs; ++i) {
		sv.sival_int = sigData + i;
		if (sigqueue (pid, sig, sv) == -1) {
			perror ("sigqueue()");
			return 1;
		}
	}

	return 0;
}
