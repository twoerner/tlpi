/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * What does the exec()'ed program do when fd's 0, 1, and 2 are
 * already closed when it is called?
 *
 * RESULT:
 * with DONT_CLOSE_ANY, CLOSE_STDIN, and CLOSE_STDERR all set to 0, we see
 * that the execlp() call succeeds but echo's STDIN is close and therefore
 * the call to "echo" fails. In other words, exec()'ed programs don't try
 * to open the standard file descriptors but instead use the existing ones
 * from their parent.
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

// set to 0 or 1
#define DONT_CLOSE_ANY 0
#define CLOSE_STDIN 0
#define CLOSE_STDERR 0

int
main (void)
{
	pid_t chldPid;
	int sts;

	chldPid = fork ();
	switch (chldPid) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: //child
#if (DONT_CLOSE_ANY == 0)
#if (CLOSE_STDIN == 1)
			if (close (0) == -1) {
				perror ("close()");
				return 1;
			}
#endif
			if (close (1) == -1) {
				perror ("close()");
				return 1;
			}
#if (CLOSE_STDERR == 1)
			if (close (2) == -1) {
				perror ("close()");
				return 1;
			}
#endif
#endif

			execlp ("echo", "echo", "this is good", NULL);
			perror ("execlp()");
			_exit (errno);

		default: // parent
			wait (&sts);
			if (WIFEXITED (sts))
				printf ("child exited with status: %d\n", WEXITSTATUS (sts));
			break;
	}

	return 0;
}
