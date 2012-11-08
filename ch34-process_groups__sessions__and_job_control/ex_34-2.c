/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program to verify that a parent process can change the process
 * group ID of one of its children before the child performs an exec(), but
 * not afterward.
 *
 * Give a cmdline arg (anything) to indicate you want the code to try to
 * set the pgid after the exec(), otherwise the change will occur before.
 *
 * NOTE: i realize this program is full of races, but if the system is
 *       otherwise not too busy the correct results will be seen
 */

#include <stdio.h>
#include <unistd.h>

int
main (int argc, __attribute__((unused)) char *argv[])
{
	int ret;
	pid_t chldPid;

	chldPid = fork ();
	switch (chldPid) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			if (argc <= 1)
				sleep (5);
			execlp ("sleep", "sleep", "10", NULL);
			_exit (1);

		default:
			break;
	}

	sleep (2);
	if (argc > 1)
		sleep (5);
	ret = setpgid (chldPid, 0);
	if (ret == -1) {
		perror ("setpgid()");
		return 1;
	}

	printf ("setpgid() okay\n");
	return 0;
}
