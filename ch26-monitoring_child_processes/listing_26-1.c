/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrate the use of wait().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

static char *
date_string (void)
{
	static char buf[256];
	size_t sz;
	struct tm *now_tm_p;
	time_t now_timet;

	now_timet = time (NULL);
	now_tm_p = localtime (&now_timet);
	sz = strftime (buf, 256, "%T", now_tm_p);
	if (sz > 0)
		return buf;
	return NULL;
}

int
main (int argc, char *argv[])
{
	int numDead;
	pid_t chldPid;
	int i;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <sleep times>...\n", argv[0]);
		return 1;
	}

	for (i=1; i<argc; ++i) {
		switch (fork ()) {
			case -1:
				perror ("fork()");
				return 1;

			case 0: { // child
				int sleepSec;

				sleepSec = atoi (argv[i]);
				if (sleepSec < 1) {
					printf ("[%s] child %d can't sleep for less than 1 second (%d)\n", date_string (), i, sleepSec);
					_exit (1);
				}
				printf ("[%s] child %d started with pid:%ld, sleeping %s seconds\n", date_string (), i, (long)getpid (), argv[i]);
				sleep (sleepSec);
				_exit (0);
			}

			default: // parent
				break;
		}
	}

	numDead = 0;
	for (;;) {
		chldPid = wait (NULL);
		if (chldPid == -1) {
			if (errno == ECHILD) {
				printf ("no more children!\n");
				return 0;
			}
			else {
				perror ("wait()");
				return 1;
			}
		}

		++numDead;
		printf ("[%s] wait() returned child PID %ld (numDead:%d)\n", date_string (), (long)chldPid, numDead);
	}

	return 0;
}
