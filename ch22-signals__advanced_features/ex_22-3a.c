/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Section 22.10 stated that accepting signals using sigwaitinfo() is faster
 * than the use of a signal handler plus sigsuspend(). Write two programs
 * to implement each approach which exchange a million signals between a
 * parent a child to validate this claim.
 *
 * This solution uses sigwaitinfo().
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>

static void run_child (void);
static void run_parent (pid_t childPid);

int
main (void)
{
	pid_t pid;
	sigset_t sigSet;

	// block SIGUSR1 to start
	if (sigemptyset (&sigSet) == -1) {
		perror ("sigemptyset()");
		return 1;
	}
	if (sigaddset (&sigSet, SIGUSR1) == -1) {
		perror ("sigaddset()");
		return 1;
	}
	if (sigprocmask (SIG_SETMASK, &sigSet, NULL) == -1) {
		perror ("sigprocmask()");
		return 1;
	}

	pid = fork ();
	switch (pid) {
		case -1: // error
			perror ("fork()");
			return 1;

		case 0: // child
			run_child ();
			break;

		default: // parent
			run_parent (pid);
			break;
	}

	return 0;
}

static void
run_child (void)
{
	sigset_t sigSet;
	pid_t pid;

	if (sigemptyset (&sigSet) == -1) {
		perror ("sigemptyset()");
		exit (1);
	}
	if (sigaddset (&sigSet, SIGUSR1) == -1) {
		perror ("sigaddset()");
		exit (1);
	}

	pid = getppid ();

	while (1) {
		if ((sigwaitinfo (&sigSet, NULL) == -1) && (errno != EINTR)) {
			perror ("child sigwaitinfo()");
			exit (1);
		}
		if (kill (pid, SIGUSR1) == -1) {
			perror ("child kill()");
			exit (1);
		}
	}
}

static void
run_parent (pid_t childPid)
{
	unsigned i;
	sigset_t sigSet;

	if (sigemptyset (&sigSet) == -1) {
		perror ("sigemptyset()");
		kill (childPid, SIGINT);
		exit (1);
	}
	if (sigaddset (&sigSet, SIGUSR1) == -1) {
		perror ("sigaddset()");
		kill (childPid, SIGINT);
		exit (1);
	}

	for (i=0; i<1000000; ++i) {
		if (kill (childPid, SIGUSR1) == -1) {
			perror ("parent kill()");
			break;
		}
		if ((sigwaitinfo (&sigSet, NULL) == -1) && (errno != EINTR)) {
			perror ("parent sigwaitinfo()");
			break;
		}
	}

	kill (childPid, SIGINT);
}
