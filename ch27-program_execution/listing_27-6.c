/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the close-on-exec flag. Depending on the presence
 * of a cmdline argument (anything) this program first sets the close-on-exec
 * flag for stdout and then exec()s the 'ls' program:
 *
 *	$ ./listing_27-6
 *	-rwxr-xr-x  1 mtk    users   28098 Jun 15 13:59 listing_27-6
 *	$ ./listing_27-6 something
 *	ls: write error: Bad file desriptor
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int
main (int argc, char *argv[])
{
	int flags;

	if (argc > 1) {
		flags = fcntl (STDOUT_FILENO, F_GETFD);
		if (flags == -1) {
			perror ("fcntl (GET)");
			return 1;
		}

		flags |= FD_CLOEXEC;

		if (fcntl (STDOUT_FILENO, F_SETFD, flags) == -1) {
			perror ("fcntl (SET)");
			return 1;
		}
	}

	execlp ("ls", "ls", "-l", argv[0], NULL);
	perror ("execlp()");
	return 1;
}
