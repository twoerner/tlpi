/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2010.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU Affero General Public License as published   *
* by the Free Software Foundation, either version 3 or (at your option)   *
* any later version. This program is distributed without any warranty.    *
* See the file COPYING.agpl-v3 for details.                               *
\*************************************************************************/


/* demo_clone.c

   Demonstrate the use of the Linux-specific clone() system call.

   This program creates a child using clone(). Various flags can be included
   in the clone() call by specifying option letters in the first command-line
   argument to the program (see usageError() below for a list of the option
   letters).

   This program is Linux-specific.
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#ifndef CHILD_SIG
# define CHILD_SIG SIGUSR1
#endif

typedef struct {        /* For passing info to child startup function */
	int    fd;
	mode_t umask;
	int    exitStatus;
	int    signal;
} ChildParams_t;

/* Startup function for cloned child */
static int
child_func (void *arg_p)
{
	ChildParams_t *cp_p;

	printf ("Child:  PID=%ld PPID=%ld\n", (long)getpid (), (long)getppid ());

	cp_p = (ChildParams_t *)arg_p;   /* Cast arg to true form */

	/* The following changes may affect parent */
	umask (cp_p->umask);
	if (close (cp_p->fd) == -1) {
		perror ("close()");
		return 1;
	}
	if (signal (cp_p->signal, SIG_DFL) == SIG_ERR) {
		perror ("signal()");
		return 1;
	}

	return cp_p->exitStatus;      /* Child terminates now */
}

/* UNSAFE: This handler uses non-async-signal-safe functions
   (printf(), strsignal(); see Section 21.1.2) */

static void             /* Handler for child termination signal */
grim_reaper (int sig)
{
	int savedErrno;

	savedErrno = errno;         /* In case we change 'errno' here */

	printf ("Caught signal %d (%s)\n", sig, strsignal(sig));

	errno = savedErrno;
}

static void
usage_error (char *progName_p)
{
	fprintf (stderr, "Usage: %s [arg]\n", progName_p);
#define fpe(str) fprintf(stderr, "        " str)
	fpe ("'arg' can contain the following letters:\n");
	fpe ("    d - share file descriptors (CLONE_FILES)\n");
	fpe ("    f - share file-system information (CLONE_FS)\n");
	fpe ("    s - share signal dispositions (CLONE_SIGHAND)\n");
	fpe ("    v - share virtual memory (CLONE_VM)\n");
	exit (EXIT_FAILURE);
}

static void
print_wait_status (const char *msg_p, int status)
{
	if (msg_p != NULL)
		printf ("%s", msg_p);

	if (WIFEXITED (status))
		printf ("child exited, status:%d\n", WEXITSTATUS (status));
	else if (WIFSIGNALED (status)) {
		printf ("child killed by signal %s", strsignal (WTERMSIG (status)));
		if (WCOREDUMP (status))
			printf (" (core dumped)");
		printf ("\n");
	}
	else if (WIFSTOPPED (status))
		printf ("child stopped by signal %s\n", strsignal (WSTOPSIG (status)));
	else if (WIFCONTINUED (status))
		printf ("child continued\n");
	else
		printf ("what happened to this child? (status:0x%08x)\n", (unsigned)status);
}

int
main (int argc, char *argv[])
{
	const int STACK_SIZE = 65536;       /* Stack size for cloned child */
	char *stack;                        /* Start of stack buffer area */
	char *stackTop;                     /* End of stack buffer area */
	int flags;                          /* Flags for cloning child */
	ChildParams_t cp;                   /* Passed to child function */
	const mode_t START_UMASK = S_IWOTH; /* Initial umask setting */
	struct sigaction sa;
	char *p;
	int status;
	ssize_t s;
	pid_t pid;

	printf ("Parent: PID=%ld PPID=%ld\n", (long) getpid(), (long) getppid());

	/* Set up an argument structure to be passed to cloned child, and
	   set some process attributes that will be modified by child */
	cp.exitStatus = 22;                 /* Child will exit with this status */

	umask (START_UMASK);                /* Initialize umask to some value */
	cp.umask = S_IWGRP;                 /* Child sets umask to this value */

	cp.fd = open ("/dev/null", O_RDWR); /* Child will close this fd */
	if (cp.fd == -1) {
		perror ("open()");
		return 1;
	}

	cp.signal = SIGTERM;                /* Child will change disposition */
	if (signal (cp.signal, SIG_IGN) == SIG_ERR) {
		perror ("signal()");
		return 1;
	}

	/* Initialize clone flags using command-line argument (if supplied) */
	flags = 0;
	if (argc > 1) {
		for (p=argv[1]; *p!='\0'; ++p) {
			if      (*p == 'd') flags |= CLONE_FILES;
			else if (*p == 'f') flags |= CLONE_FS;
			else if (*p == 's') flags |= CLONE_SIGHAND;
			else if (*p == 'v') flags |= CLONE_VM;
			else usage_error (argv[0]);
		}
	}

	/* Allocate stack for child */
	stack = malloc(STACK_SIZE);
	if (stack == NULL) {
		perror ("malloc()");
		return 1;
	}
	stackTop = stack + STACK_SIZE;  /* Assume stack grows downwards */

	/* Establish handler to catch child termination signal */
	if (CHILD_SIG != 0) {
		sigemptyset(&sa.sa_mask);
		sa.sa_flags = SA_RESTART;
		sa.sa_handler = grim_reaper;
		if (sigaction(CHILD_SIG, &sa, NULL) == -1) {
			perror ("sigaction()");
			return 1;
		}
	}

	/* Create child; child commences execution in childFunc() */
	if (clone (child_func, stackTop, flags | CHILD_SIG, &cp) == -1) {
		perror ("clone()");
		return 1;
	}

	/* Parent falls through to here. Wait for child; __WCLONE option is
	   required for child notifying with signal other than SIGCHLD. */
	pid = waitpid (-1, &status, (CHILD_SIG != SIGCHLD) ? __WCLONE : 0);
	if (pid == -1) {
		perror ("waitpid()");
		return 1;
	}

	printf ("    Child PID=%ld\n", (long) pid);
	print_wait_status ("    Status: ", status);

	/* Check whether changes made by cloned child have affected parent */
	printf ("Parent - checking process attributes:\n");
	if (umask (0) != START_UMASK)
		printf ("    umask has changed\n");
	else
		printf ("    umask has not changed\n");

	s = write (cp.fd, "Hello world\n", 12);
	if (s == -1 && errno == EBADF)
		printf ("    file descriptor %d has been closed\n", cp.fd);
	else if (s == -1)
		printf ("    write() on file descriptor %d failed (%s)\n",
				cp.fd, strerror(errno));
	else
		printf ("    write() on file descriptor %d succeeded\n", cp.fd);

	if (sigaction (cp.signal, NULL, &sa) == -1) {
		perror ("sigaction");
		return 1;
	}
	if (sa.sa_handler != SIG_IGN)
		printf ("    signal disposition has changed\n");
	else
		printf ("    signal disposition has not changed\n");

	return EXIT_SUCCESS;
}
