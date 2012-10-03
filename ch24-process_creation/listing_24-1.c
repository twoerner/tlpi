/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrate the use of fork().
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

static int idata_G = 111;

int
main (void)
{
	int istack = 222;
	pid_t chldPid;

	switch ((chldPid = fork ())) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			idata_G *= 3;
			istack *= 3;
			break;

		default: // parent
			sleep (3);
			break;
	}

	printf ("pid:%ld %s idata:%d istack:%d\n", (long)getpid (), (chldPid == 0)? "(child)  " : "(parent) ", idata_G, istack);
	return 0;
}
