/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstration of use of kill().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

int
main (int argc, char *argv[])
{
	int rtn, sig, pid;

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <signal> <pid>\n", argv[0]);
		return 1;
	}

	sig = atoi (argv[1]);
	pid = atoi (argv[2]);
	rtn = kill ((pid_t)pid, sig);

	if (sig != 0) {
		if (rtn == -1) {
			perror ("kill()");
			return 1;
		}
	}
	else {
		if (rtn == 0)
			printf ("process %d exists and we can send it a signal\n", pid);
		else {
			if (errno == EPERM)
				printf ("process %d exists, but we can't send it a signal\n", pid);
			else if (errno == ESRCH)
				printf ("process %d does not exist\n", pid);
			else {
				perror ("kill");
				return 1;
			}
		}
	}

	return 0;
}
