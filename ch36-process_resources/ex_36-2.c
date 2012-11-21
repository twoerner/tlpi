/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program that executes a command and then displays its resource
 * usage. This is analogous to what the time(1) command does. Thus, we would
 * use this program as follows:
 *
 *	$ ./ex_36-2 <command> [<args>...]
 */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "print_rlimit.h"


int
main (int argc, char *argv[])
{
	int ret;
	struct timeval start;

	if (argc < 2) {
		printf ("usage: %s <command> [<args>...]\n", argv[0]);
		return 1;
	}

	ret = gettimeofday (&start, NULL);
	if (ret == -1) {
		perror ("gettimeofday(start)");
		return 1;
	}

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;


		case 0: // child
			execvp (argv[1], &argv[1]);
			if (errno == ENOENT)
				_exit (127);
			_exit (126);

		default: { // parent
			int stats;
			struct rusage usg;
			pid_t pid;
			struct timeval end, elapsed;

			pid = wait (&stats);
			if (pid == (pid_t)-1) {
				perror ("wait()");
				return 1;
			}

			ret = gettimeofday (&end, NULL);
			if (ret == -1) {
				perror ("gettimeofday(end)");
				return 1;
			}

			ret = getrusage (RUSAGE_CHILDREN, &usg);
			if (ret == -1) {
				perror ("getrusage()");
				return 1;
			}

			elapsed.tv_sec = end.tv_sec - start.tv_sec;
			elapsed.tv_usec = end.tv_usec;
			if (end.tv_usec < start.tv_usec) {
				elapsed.tv_usec += 1000000;
				--elapsed.tv_sec;
			}
			elapsed.tv_usec -= start.tv_usec;

			printf ("\n");
			printf ("real     %ld.%02ld\n", (long)elapsed.tv_sec, (long)elapsed.tv_usec / 10000);
			printf ("user     %ld.%02ld\n", (long)usg.ru_utime.tv_sec, (long)usg.ru_utime.tv_usec / 10000);
			printf ("sys      %ld.%02ld\n", (long)usg.ru_stime.tv_sec, (long)usg.ru_stime.tv_usec / 10000);
			printf ("rss      %ld [kb]\n", usg.ru_maxrss);
			printf ("soft pf  %ld\n", usg.ru_minflt);
			printf ("hard pf  %ld\n", usg.ru_majflt);
			printf ("blk in   %ld\n", usg.ru_inblock);
			printf ("blk out  %ld\n", usg.ru_oublock);
			printf ("v ctx sw %ld\n", usg.ru_nvcsw);
			printf ("i ctx sw %ld\n", usg.ru_nivcsw);

			if (WIFEXITED (stats))
				return WEXITSTATUS (stats);

			break;
		}
	}

	return 5;
}
