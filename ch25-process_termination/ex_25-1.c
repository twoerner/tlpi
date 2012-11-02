/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * If a child process makes the call exit(-1), what exit status (as returned
 * by WEXITSTATUS()) will be seen by the parent?
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// set to 0 or 1
#define USE__EXIT 1

int
main (void)
{
	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
#if (USE__EXIT == 1)
			_exit (-1);
#else
			exit (-1);
#endif

		default: { // parent
			int sts;

			wait (&sts);
			if (WIFEXITED (sts))
				printf ("exit status of child: %d\n", WEXITSTATUS (sts));
			break;
		 }
	}

	return 0;
}
