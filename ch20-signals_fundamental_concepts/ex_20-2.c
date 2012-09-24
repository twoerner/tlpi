/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program that shows that when the disposition of a pending signal
 * is changed to be SIG_IGN, the program never sees (catches) the signal.
 *
 * Run this program from bash, when done run:
 *	$ echo $?
 * If the signal was not caught the return value will be 0, if the program
 * terminated because of a signal, the return value will be 128 + <int> = 130.
 */

#include <stdio.h>
#include <signal.h>

int
main (void)
{
	int ret;
	sigset_t procMask;

	// block SIGINT
	ret = sigemptyset (&procMask);
	if (ret != 0) {
		perror ("sigemptyset()");
		return 1;
	}
	ret = sigaddset (&procMask, SIGINT);
	if (ret != 0) {
		perror ("sigaddset()");
		return 1;
	}
	ret = sigprocmask (SIG_BLOCK, &procMask, NULL);
	if (ret != 0) {
		perror ("sigprocmask()");
		return 1;
	}

	// send yourself SIGINT
	raise (SIGINT);

	// set disposition of SIGINT to SIG_IGN
	signal (SIGINT, SIG_IGN);

	// unblock SIGINT
	ret = sigemptyset (&procMask);
	if (ret != 0) {
		perror ("sigemptyset()");
		return 1;
	}
	ret = sigaddset (&procMask, SIGINT);
	if (ret != 0) {
		perror ("sigaddset()");
		return 1;
	}
	ret = sigprocmask (SIG_UNBLOCK, &procMask, NULL);
	if (ret != 0) {
		perror ("sigprocmask()");
		return 1;
	}

	return 0;
}
