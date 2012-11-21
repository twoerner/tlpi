/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program that shows that the getrusage() RUSAGE_CHILDREN flag
 * retrieves information about only the children for which a wait() call has
 * been performed. (Have the program create a child process which consumes
 * some CPU time, then have the parent call getrusage() before and after
 * calling wait().)
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "print_rlimit.h"


int
main (void)
{
	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;


		case 0: { // child
			time_t start, now;

			alarm (10);
			start = time (NULL);
			while (1) {
				now = time (NULL);
				if ((now - start) > 5)
					break;
			}
			_exit (0);
		}

		default: { // parent
			int ret;
			struct rusage usg;
			pid_t pid;

			sleep (2);

			ret = getrusage (RUSAGE_CHILDREN, &usg);
			if (ret == -1) {
				perror ("getrusage()");
				return 1;
			}
			printf ("before: user CPU seconds = %ld\n", (long)usg.ru_utime.tv_sec);

			pid = wait (NULL);
			if (pid == (pid_t)-1) {
				perror ("wait()");
				return 1;
			}

			ret = getrusage (RUSAGE_CHILDREN, &usg);
			if (ret == -1) {
				perror ("getrusage()");
				return 1;
			}
			printf ("after:  user CPU seconds = %ld\n", (long)usg.ru_utime.tv_sec);
			break;
		}
	}

	return 0;
}
