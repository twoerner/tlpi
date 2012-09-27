/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates handling realtime signals.
 * It catches and displays various fields from the siginfo_t structure passed
 * to its signal handler. It takes 2 optional cmdline args:
 *	- if the first is supplied, the main() blocks all signals, and then
 *	  sleeps for the number of seconds specified by this argument; during
 *	  this time we can queue multiple realtime signals to the process and
 *	  observe what happens when the signals are unblocked
 *	- the second optional arg specifies the number of seconds the signal
 *	  handler should sleep before returning (default 1)
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

static void siginfo_handler (int sig, siginfo_t *sinfo_p, void *ucontext_p);

static volatile int handlerSleepTime_G = 1;
static volatile int sigCnt_G = 0;
static volatile bool allDone_G = false;

int
main (int argc, char *argv[])
{
	struct sigaction sa;
	int sig;
	sigset_t prevMask, blockMask;
	int sleepTime;

	if (argc > 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<queue sleep>] [<handler return pause>]\n", argv[0]);
		return 0;
	}
	printf ("%s: PID is %ld\n", argv[0], (long)getpid ());

	if (argc > 2) {
		handlerSleepTime_G = atoi (argv[2]);
		if (handlerSleepTime_G < 1)
			handlerSleepTime_G = 1;
	}

	sa.sa_sigaction = siginfo_handler;
	sa.sa_flags = SA_SIGINFO;
	sigfillset (&sa.sa_mask);

	for (sig=1; sig<NSIG; ++sig)
		if ((sig != SIGTSTP) && (sig != SIGQUIT))
			sigaction (sig, &sa, NULL);

	if (argc > 1) {
		sleepTime = atoi (argv[1]);
		if (sleepTime < 1)
			sleepTime = 1;

		sigfillset (&blockMask);
		sigdelset (&blockMask, SIGINT);
		sigdelset (&blockMask, SIGTERM);
		if (sigprocmask (SIG_SETMASK, &blockMask, &prevMask) == -1) {
			perror ("sigprocmask()");
			return 1;
		}

		printf ("%s: signals blocked, sleeping %d seconds\n", argv[0], sleepTime);
		sleep (sleepTime);
		printf ("%s: sleep complete\n", argv[0]);

		if (sigprocmask (SIG_SETMASK, &prevMask, NULL) == -1) {
			perror ("sigprocmask()");
			return 1;
		}
	}

	while (!allDone_G)
		pause ();
	printf ("%s: caught %d signals\n", argv[0], sigCnt_G);
	return 0;
}

/* UNSAFE: this handler uses printf() etc... */
static void
siginfo_handler (int sig, siginfo_t *sinfo_p, void __attribute__((unused))  *ucontext_p)
{
	if ((sig == SIGINT) || (sig == SIGTERM)) {
		allDone_G = true;
		return;
	}

	++sigCnt_G;
	printf ("\tcaught signal %d\n", sig);
	printf ("\t\tsi_signo:%d si_code:%d (%s) ",
			sinfo_p->si_signo, sinfo_p->si_code,
			(sinfo_p->si_code == SI_USER)? "SI_USER" :
			(sinfo_p->si_code == SI_QUEUE)? "SI_QUEUE" : "(other)");
	printf ("si_value:%d\n", sinfo_p->si_value.sival_int);
	printf ("\t\tsi_pid:%ld si_uid:%ld\n", (long)sinfo_p->si_pid, (long)sinfo_p->si_uid);

	sleep (handlerSleepTime_G);
}
