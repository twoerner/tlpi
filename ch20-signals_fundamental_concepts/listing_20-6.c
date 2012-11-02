/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program can be used to observe that signals are not queued. This
 * program takes up to 4 cmdline args:
 *
 *	$ ./listing_20-6 <pid> <signal count> <signal> [<signal2>]
 *
 * The first argument is the process ID of the process to which the program
 * should send signals. The second argument specifies the number of signals
 * to be sent to the target process.  The third argument specifies the signal
 * that is to be sent to the target process. If the fourth argument is given,
 * this program sends one instance of that signal to the process after sending
 * the signals specified by the second and third cmdline args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int
main (int argc, char *argv[])
{
	int numSigs, sig, i;
	pid_t pid;

	if (argc < 4 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pid> <signal count> <signal> [<signal2>]\n", argv[0]);
		return 1;
	}

	pid = atol (argv[1]);
	numSigs = atoi (argv[2]);
	if (numSigs <= 0) {
		printf ("please specify a positive integer for option 2 <signal count>\n");
		return 1;
	}
	sig = atoi (argv[3]);

	printf ("%s: sending signal %d to process %ld %d times\n",
			argv[0], sig, (long)pid, numSigs);
	for (i=0; i<numSigs; ++i)
		if (kill (pid, sig) == -1) {
			perror ("kill()");
			return 1;
		}

	if (argc > 4) {
		sig = atoi (argv[4]);
		if (kill (pid, sig) == -1) {
			perror ("kill()");
			return 1;
		}
	}

	return 0;
}
