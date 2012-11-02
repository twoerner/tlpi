/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Replace the use of waitpid() in listing 26-3 with waitid(). The call to
 * print_wait_status() will need to be replaced by code that prints relevant
 * fields from the siginfo_t structure returned by waitid().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void
print_child_info (siginfo_t *info_p)
{
	if (info_p == NULL)
		return;

	switch (info_p->si_code) {
		case CLD_EXITED:
			printf ("child exited\n");
			printf ("\tpid:    %ld\n", (long)info_p->si_pid);
			printf ("\tstatus: %ld\n", (long)info_p->si_status);
			break;

		default:
			printf ("not CHD_EXITED\n");
			break;
	}
}

static void
usage (char *pgm_p)
{
	printf ("usage: %s <exit status>\n", pgm_p);
}

int
main (int argc, char *argv[])
{
	int rtn;

	if (argc != 2) {
		usage (argv[0]);
		return 1;
	}
	if (strcmp (argv[1], "--help") == 0) {
		usage (argv[0]);
		return 1;
	}

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: { // child
			int exitVal;

			printf ("child started with PID: %ld\n", (long)getpid ());

			exitVal = atoi (argv[1]);
			_exit (exitVal);
		}

		default: { // parent
			siginfo_t info;

			rtn = waitid (P_ALL, 0, &info, WEXITED);
			if (rtn == -1) {
				perror ("waitid()");
				return 1;
			}

			print_child_info (&info);
			break;
		}
	}

	return 0;
}
