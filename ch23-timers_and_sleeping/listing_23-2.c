/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Example of using a realtime timer to place an upper limit on the time for
 * which a blocking system call can remain blocked.
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>

#define BUF_SIZE 200

static void
signal_handler (__attribute__((unused)) int sig)
{
	printf ("signal caught\n"); // UNSAFE
}

int
main (void)
{
	struct sigaction sa;
	char buf[BUF_SIZE];
	ssize_t readCnt;
	int savedErrno;

	// setup handler for alarm
	if (sigemptyset (&sa.sa_mask) == -1) {
		perror ("sigemptyset()");
		return 1;
	}
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	if (sigaction (SIGALRM, &sa, NULL) == -1) {
		perror ("sigaction()");
		return 1;
	}

	// break the block after 10 seconds
	alarm (10);

	readCnt = read (STDIN_FILENO, buf, BUF_SIZE - 1);
	savedErrno = errno;
	alarm (0);
	errno = savedErrno;

	if (readCnt == -1) {
		if (errno == EINTR)
			printf ("read timeout\n");
		else {
			perror ("read()");
			return 1;
		}
	}
	else
		printf ("good read (%ld): %s", (long)readCnt, buf);

	return 0;
}
