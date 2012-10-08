/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Listing 26-4 uses sleep() to allow the child process a chance to execute
 * and terminate before the parent executes system(). This approach introduces
 * a potential race condition. Rewrite the program to eliminate this race
 * condition by using signals to synchronize the parent and child.
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <libgen.h>
#include <unistd.h>
#include <sys/types.h>

#define CMD_SIZE 128
#define UNUSED __attribute__((unused))

static void
signal_handler (UNUSED int sig)
{
}

int
main (UNUSED int argc, char *argv[])
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
			kill (getppid (), SIGUSR1);
			_exit (0);

		default: { // parent
			struct sigaction sa;

			sigemptyset (&sa.sa_mask);
			sa.sa_flags = 0;
			sa.sa_handler = signal_handler;
			sigaction (SIGUSR1, &sa, NULL);

			pause ();
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
	}

	return 0;
}
