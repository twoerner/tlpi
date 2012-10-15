/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * What happens if the disposition of SIGCHLD is set to SIG_IGN
 * (thereby eliminating zombies) but we call system()?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

// set to 0 or 1
#define DONT_HANDLE_SIGCHLD 1

static void print_wait_status (int status);

int
main (void)
{
	int ret;

#if (DONT_HANDLE_SIGCHLD == 0)
	struct sigaction sa;

	// don't care about zombies
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIG_IGN;
	sigaction (SIGCHLD, &sa, NULL);
#endif

	ret = system ("sleep 3");
	if (ret == -1) {
		perror ("system()");
		return 1;
	}
	print_wait_status (ret);

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
