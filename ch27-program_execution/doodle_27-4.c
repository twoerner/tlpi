/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * What happens if the disposition of SIGCHLD is set to SIG_IGN
 * (thereby eliminating zombies) but the parent calls waitpid()?
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

static void print_wait_status (int status);

int
main (void)
{
	struct sigaction sa;
	pid_t chldPid;

	// don't care about zombies
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction (SIGCHLD, &sa, NULL);

	switch (chldPid = fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			_exit (0);

		default: { // parent
			int status;
			pid_t ret;

			ret = waitpid (chldPid, &status, 0);
			if (ret == -1) {
				perror ("waitpid()");
				return 1;
			}
			print_wait_status (status);
			break;
		 }
	}

	return 0;
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
