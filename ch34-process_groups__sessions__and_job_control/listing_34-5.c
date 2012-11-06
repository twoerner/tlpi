/*
 * Copyright (C) 2012  Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program allows us to see how the shell organizes the commands in a
 * pipeline into a job (i.e. process group).
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>

static int cmdNum_G;

// UNSAFE: printf(), strsignal()
static void
signal_handler (int sig)
{
	// process group leader?
	if (getpid () == getpgrp ())
		fprintf (stderr, "terminal FG process group: %ld\n", (long)tcgetpgrp (STDERR_FILENO));
	fprintf (stderr, "process %ld (%d) received signal %d (%s)\n",
			(long)getpid (), cmdNum_G, sig, strsignal (sig));

	// if we catch SIGTSTP it won't actually stop us
	// therefore make sure to stop ourselves
	if (sig == SIGTSTP)
		raise (SIGSTOP);
}

int
main (void)
{
	struct sigaction sa;

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = signal_handler;
	sigaction (SIGINT, &sa, NULL);
	sigaction (SIGTSTP, &sa, NULL);
	sigaction (SIGCONT, &sa, NULL);

	// try to figure out cmdNum_G
	if (isatty (STDIN_FILENO)) {
		fprintf (stderr, "terminal FG process group: %ld\n", (long)tcgetpgrp (STDIN_FILENO));
		fprintf (stderr, "Command   PID   PPID   PGRP    SID\n");
		cmdNum_G = 0;
	}
	else {
		if (read (STDIN_FILENO, &cmdNum_G, sizeof (cmdNum_G)) <= 0) {
			perror ("read()");
			return 1;
		}
	}

	++cmdNum_G;
	fprintf (stderr, "%4d    %5ld  %5ld  %5ld  %5ld\n", cmdNum_G, (long)getpid (), (long)getppid (), (long)getpgrp (), (long)getsid (0));
	if (!isatty (STDOUT_FILENO))
		write (STDOUT_FILENO, &cmdNum_G, sizeof (cmdNum_G));

	for (;;)
		pause ();

	return 0;
}
