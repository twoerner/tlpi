/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program shows a simple example of the use of clone() to create a child
 * process. It:
 * - opens a file descriptor (for /dev/null) that will be closed by the child
 * - sets the value of the clone() flag based on the cmdline args:
 *    - if no cmdline args, flags = 0
 *    - otherwise flags = CLONE_FILES
 * - allocates a stack for the child
 * - if CHILD_SIG is nonzero and is not equal to SIGCHLD, ignore it
 * - call clone() to create the child
 * - wait for the child to terminate
 * - check if /dev/null is still open by trying to write to it
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#ifndef CHILD_SIG
# define CHILD_SIG SIGUSR1
#endif

static int
child_func (void *arg_p)
{
	if (close (*((int*)arg_p)) == -1) {
		perror ("close()");
		return 1;
	}
	return 0;
}

int
main (int argc, __attribute__((unused)) char *argv[])
{
	const int STACK_SZ = 65536;
	char *stack_p;
	char *stackTop_p;
	int fd, flags;
	ssize_t s;

	// open /dev/null
	fd = open ("/dev/null", O_RDWR);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	// set flags based on existance of cmdline arg(s)
	flags = (argc > 1)? CLONE_FILES : 0;

	// allocate child's stack
	stack_p = malloc (STACK_SZ);
	if (stack_p == NULL) {
		perror ("malloc()");
		return 1;
	}
	stackTop_p = stack_p + STACK_SZ;

	// setup child's termination signal
	if (CHILD_SIG != 0 && CHILD_SIG != SIGCHLD)
		if (signal (CHILD_SIG, SIG_IGN) == SIG_ERR) {
			perror ("signal()");
			return 1;
		}

	// do clone
	if (clone (child_func, stackTop_p, flags | CHILD_SIG, (void*)&fd) == -1) {
		perror ("clone()");
		return 1;
	}

	// wait for child
	if (waitpid (-1, NULL, (CHILD_SIG != SIGCHLD)? __WCLONE : 0) == -1) {
		perror ("waitpid()");
		return 1;
	}
	printf ("child has terminated\n");

	// check if /dev/null is open
	s = write (fd, "x", 1);
	if (s == -1 && errno == EBADF)
		printf ("child closed fd\n");
	else if (s == -1)
		perror ("write()");
	else
		printf ("write() succeeded\n");

	return 0;
}
