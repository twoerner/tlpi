/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstration that open file descriptors after a fork() are shared and
 * point to the same file description entry (meaning, for example, that if
 * the read() offset is updated in one process, the other sees the change
 * too).
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int
main (void)
{
	int fd, flags;
	char template[] = "/tmp/testXXXXXX";

	setbuf (stdout, NULL);

	fd = mkstemp (template);
	if (fd == -1) {
		perror ("mkstemp()");
		return 1;
	}
	unlink (template);

	printf ("file offset before fork(): %lld\n", (long long)lseek (fd, 0, SEEK_CUR));

	flags = fcntl (fd, F_GETFL);
	if (flags == -1) {
		perror ("fcntl()");
		return 1;
	}
	printf ("O_APPEND flags before fork(): %s\n", (flags & O_APPEND)? "on" : "off");

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			if (lseek (fd, 1000, SEEK_SET) == -1) {
				perror ("lseek (child)");
				return 1;
			}

			flags = fcntl (fd, F_GETFL);
			if (flags == -1) {
				perror ("fcntl (child)");
				return 1;
			}
			flags |= O_APPEND;
			if (fcntl (fd, F_SETFL, flags) == -1) {
				perror ("fcntl (child, set)");
				return 1;
			}
			
			_exit (0);

		default: // parent
			if (wait (NULL) == -1) {
				perror ("wait()");
				return 1;
			}
			printf ("child has exited\n");

			printf ("file offset in parent: %lld\n", (long long)lseek (fd, 0, SEEK_CUR));
			flags = fcntl (fd, F_GETFL);
			if (flags == -1) {
				perror ("fcntl (parent)");
				return 1;
			}
			printf ("O_APPEND flag in parent: %s\n", (flags & O_APPEND)? "on" : "off");
			break;
	}

	return 0;
}
