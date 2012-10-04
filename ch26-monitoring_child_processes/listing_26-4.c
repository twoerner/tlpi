/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrates the creation of a zombie.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <libgen.h>
#include <unistd.h>

#define CMD_SIZE 128

int
main (__attribute__((unused)) int argc, char *argv[])
{
	char cmd[CMD_SIZE];
	pid_t chldPid;

	setbuf (stdout, NULL);
	printf ("parent (PID:%ld)\n", (long)getpid ());

	switch (chldPid = fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child, immediately exits to create the zombie
			printf ("child (PID:%ld) exiting\n", (long)getpid ());
			_exit (0);

		default: // parent
			sleep (3);
			snprintf (cmd, CMD_SIZE, "ps | grep %s", basename (argv[0]));
			system (cmd);

			if (kill (chldPid, SIGKILL) == -1) {
				perror ("kill (SIGKILL)");
				return 1;
			}
			sleep (3);
			printf ("after sending SIGKILL to zombie (PID:%ld):\n", (long)chldPid);
			system (cmd);
	}

	return 0;
}
