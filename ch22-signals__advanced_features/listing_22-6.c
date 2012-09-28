/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * An example of the use of sigwaitinfo().
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>

int
main (int argc, char *argv[])
{
	int sig;
	siginfo_t sigInfo;
	sigset_t allSigs;

	if (argc < 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<seconds>]\n", argv[0]);
		return 0;
	}
	printf ("[%s] pid:%ld\n", argv[0], (long)getpid ());

	// block all signals except KIGKILL and SIGSTOP
	sigfillset (&allSigs);
	if (sigprocmask (SIG_SETMASK, &allSigs, NULL) == -1) {
		perror ("sigprocmask()");
		return 1;
	}
	printf ("[%s] signals blocked\n", argv[0]);

	if (argc > 1) {
		int sleepTime;

		sleepTime = atoi (argv[1]);
		if (sleepTime > 0) {
			printf ("[%s] about to delay %d seconds\n", argv[0], sleepTime);
			sleep (sleepTime);
			printf ("[%s] done sleeping\n", argv[0]);
		}
	}

	for (;;) {
		sig = sigwaitinfo (&allSigs, &sigInfo);
		switch (sig) {
			case -1:
				perror ("sigwaitinfo()");
				return 1;

			case SIGINT:
			case SIGTERM:
				return 0;

			default:
				printf ("got signal %d (%s)\n", sig, strsignal (sig));
				printf ("\tsi_signo:%d si_code=%d si_value:%d\n",
						sigInfo.si_signo, sigInfo.si_code, sigInfo.si_value.sival_int);
				printf ("\tsi_pid:%ld si_uid:%ld\n",
						(long)sigInfo.si_pid, (long)sigInfo.si_uid);
		}
	}

	return 0;
}
