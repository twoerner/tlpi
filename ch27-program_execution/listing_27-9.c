/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * An better implementation of system() which includes signal handling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static int my_system (const char *cmd_p);
static void print_wait_status (int status);

int
main (void)
{
	int sts;

	sts = my_system ("ls");
	print_wait_status (sts);

	return 0;
}

static int
my_system (const char *cmd_p)
{
	sigset_t blockMask, origMask;
	struct sigaction saIgnore, saOrigQuit, saOrigInt, saDefault;
	int status, savedErrno;
	pid_t chldPid;

	if (cmd_p == NULL)
		return system (":") == 0;

	sigemptyset (&blockMask);
	sigaddset (&blockMask, SIGCHLD);
	sigprocmask (SIG_BLOCK, &blockMask, &origMask);

	saIgnore.sa_handler = SIG_IGN;
	saIgnore.sa_flags = 0;
	sigemptyset (&saIgnore.sa_mask);
	sigaction (SIGINT, &saIgnore, &saOrigInt);
	sigaction (SIGQUIT, &saIgnore, &saOrigQuit);

	switch (chldPid = fork ()) {
		case -1:
			status = -1;
			break;

		case 0: // child
			saDefault.sa_handler = SIG_DFL;
			saDefault.sa_flags = 0;
			sigemptyset (&saDefault.sa_mask);

			if (saOrigInt.sa_handler != SIG_IGN)
				sigaction (SIGINT, &saDefault, NULL);
			if (saOrigQuit.sa_handler != SIG_IGN)
				sigaction (SIGQUIT, &saDefault, NULL);

			sigprocmask (SIG_SETMASK, &origMask, NULL);

			execl ("/bin/sh", "sh", "-c", cmd_p, NULL);
			_exit (127);

		default: // parent
			while (waitpid (chldPid, &status, 0) == -1) {
				if (errno != EINTR) {
					status = -1;
					break;
				}
			}
			break;
	}

	// unblock SIGCHLD
	// restore dispositions of SIGINT and SIGQUIT
	savedErrno = errno;
	sigprocmask (SIG_SETMASK, &origMask, NULL);
	sigaction (SIGINT, &saOrigInt, NULL);
	sigaction (SIGQUIT, &saOrigQuit, NULL);
	errno = savedErrno;

	return status;
}


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
