/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * An implementation of system() which excludes signal handling.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static int my_system (char *cmd_p);
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
my_system (char *cmd_p)
{
	int status;
	pid_t chldPid;

	switch (chldPid = fork ()) {
		case -1:
			return -1;

		case 0: // child
			execl ("/bin/sh", "sh", "-c", cmd_p, NULL);
			_exit (127);

		default: // parent
			if (waitpid (chldPid, &status, 0) == -1)
				return -1;
			return status;
	}
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
