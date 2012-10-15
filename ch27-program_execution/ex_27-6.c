/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Suppose a parent process has established a handler for SIGCHLD and also
 * blocked this signal. Subsequently, one of its children exits, and the
 * parent then does a wait() to collect the child's status. What happens when
 * then parent unblocks SIGCHLD?
 *
 * What is the relevance for a program calling the system() function.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

static void
print_wait_status (int status)
{
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

static void
signal_handler (int sig)
{
	printf ("caught signal %d (%s)\n", sig, strsignal (sig)); // UNSAFE
}

int
main (void)
{
	struct sigaction sa;
	sigset_t sigMask;
	pid_t chldPid;
	int status;

	// establish handler for SIGCHLD
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGCHLD, &sa, NULL);

	// block SIGCHLD
	sigemptyset (&sigMask);
	sigaddset (&sigMask, SIGCHLD);
	sigprocmask (SIG_BLOCK, &sigMask, NULL);

	switch (chldPid = fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			_exit (0);

		default:
			// wait for child to exit
			sleep (2);

			// unblock SIGCHLD
			sigprocmask (SIG_UNBLOCK, &sigMask, NULL);

			waitpid (chldPid, &status, 0);
			print_wait_status (status);
			break;
	}

	return 0;
}
