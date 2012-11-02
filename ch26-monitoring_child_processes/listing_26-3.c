/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Using waitpid() to retrieve the status of a child process.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

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
	int status;
	pid_t chldPid;

	if (argc < 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<exit status>]\n", argv[0]);
		return 1;
	}

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			printf ("child started with PID: %ld\n", (long)getpid ());
			if (argc > 1) {
				int exitVal;

				exitVal = atoi (argv[1]);
				if (exitVal < 0) {
					printf ("please provide positive or a zero exit value\n");
					return 1;
				}
				exit (exitVal);
			}

			for (;;)
				pause ();

		default: // parent
			for (;;) {
				chldPid = waitpid (-1, &status, WUNTRACED | WCONTINUED);
				if (chldPid == -1) {
					perror ("waitpid()");
					return 1;
				}

				printf ("waitpid returned: PID=%ld status=0x%04x (%d,%d)\n", (long)chldPid, (unsigned)status, status >> 8, status & 0xff);
				print_wait_status (NULL, status);

				if (WIFEXITED (status) || WIFSIGNALED (status))
					exit (0);
			}
	}
}
