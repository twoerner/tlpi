/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Reaping dead children via a handler for SIGCHLD.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 256

static volatile sig_atomic_t numLiveChildren_G = 0;

static char *
current_time (const char *format_p)
{
	static char buf[BUF_SIZE];
	time_t tm;
	size_t sz;
	struct tm *tm_p;

	tm = time (NULL);
	tm_p = localtime (&tm);
	if (tm_p == NULL)
		return NULL;

	sz = strftime (buf, BUF_SIZE, (format_p != NULL)? format_p : "%c", tm_p);
	return (sz == 0)? NULL : buf;

}

static void
print_wait_status (const char *msg_p, int status)
{
	if (msg_p != NULL)
		printf ("%s", msg_p);

	if (WIFEXITED (status))
		printf ("child exited, status:%d\n", WEXITSTATUS (status));
	else if (WIFSIGNALED (status)) {
		printf ("child killed by signal %s", strsignal (WTERMSIG (status)));
		if (WCOREDUMP (status))
			printf (" (core dumped)");
		printf ("\n");
	}
	else if (WIFSTOPPED (status))
		printf ("child stopped by signal %s\n", strsignal (WSTOPSIG (status)));
	else if (WIFCONTINUED (status))
		printf ("child continued\n");
	else
		printf ("what happened to this child? (status:0x%08x)\n", (unsigned)status);
}

// UNSAFE!
static void
signal_handler (__attribute__((unused)) int sig)
{
	int status, savedErrno;
	pid_t chldPid;

	savedErrno = errno;
	printf ("[%s] SIGCHLD handler\n", current_time ("%T"));

	while ((chldPid = waitpid (-1, &status, WNOHANG)) > 0) {
		printf ("[%s] SIGCHLD handler: reaped child %ld - ", current_time ("%T"), (long)chldPid);
		print_wait_status (NULL, status);
		--numLiveChildren_G;
	}

	if (chldPid == -1 && errno != ECHILD) {
		perror ("waitpid()");
		exit (1);
	}

	sleep (5);
	printf ("[%s] SIGCHLD handler: returning\n", current_time ("%T"));

	errno = savedErrno;
}

int
main (int argc, char *argv[])
{
	int i, sigCnt;
	sigset_t blockMask, emptyMask;
	struct sigaction sa;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <child sleep time>...\n", argv[0]);
		return 1;
	}

	setbuf (stdout, NULL);
	sigCnt = 0;
	numLiveChildren_G = argc - 1;

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGCHLD, &sa, NULL);

	// block SIGCHLD to prevent race (if SIGCHLD is delivered before parent runs sigsuspend())
	sigemptyset (&blockMask);
	sigaddset (&blockMask, SIGCHLD);
	sigprocmask (SIG_BLOCK, &blockMask, NULL);

	for (i=1; i<argc; ++i) {
		switch (fork ()) {
			case -1:
				perror ("fork()");
				return 1;

			case 0: { // child, sleeps then exits
				int sleepTime;

				sleepTime = atoi (argv[i]);
				if (sleepTime < 1) {
					printf ("please provide a sleep time greater than 1\n");
					return 1;
				}
				sleep (sleepTime);
				printf ("[%s] child %d (PID:%ld) exiting\n", current_time ("%T"), i, (long)getpid ());
				_exit (0);
			}

			default:
				break;
		}
	}

	sigemptyset (&emptyMask);
	while (numLiveChildren_G > 0) {
		if (sigsuspend (&emptyMask) == -1 && errno != EINTR) {
			perror ("sigsuspend()");
			return 1;
		}
		++sigCnt;
	}

	printf ("[%s] all %d children have terminated\n", current_time ("%T"), argc - 1);
	printf ("        SIGCHLD caught %d times\n", sigCnt);

	return 0;
}
