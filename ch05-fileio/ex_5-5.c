/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program to verify that duplicated file descriptors share a file offset value
 * and open file status flags.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	int fd1, fd2;
	char buf[256];
	ssize_t bytesRead;
	off_t pos1, pos2;
	int flags1, flags2;

	if (argc != 2) {
		fprintf (stderr, "usage: %s <file>\n", argv[0]);
		return 1;
	}

	fd1 = open (argv[1], O_RDWR);
	if (fd1 == -1) {
		perror ("open()");
		return 1;
	}

	fd2 = dup (fd1);
	if (fd2 == -1) {
		perror ("dup()");
		return 1;
	}

	bytesRead = read (fd1, buf, 12);
	if (bytesRead != 12) {
		perror ("read()");
		return 1;
	}

	pos1 = lseek (fd1, 0, SEEK_CUR);
	pos2 = lseek (fd2, 0, SEEK_CUR);
	if (pos1 != pos2) {
		fprintf (stderr, "pos1 (%ju) != pos2 (%ju)\n", pos1, pos2);
		return 1;
	}
	printf ("pos1 (%ju) = pos2 (%ju)\n", pos1, pos2);

	flags1 = fcntl (fd1, F_GETFL);
	if (flags1 == -1) {
		perror ("fcntl (fd1, F_GETFL)");
		return 1;
	}
	flags2 = fcntl (fd2, F_GETFL);
	if (flags2 == -1) {
		perror ("fcntl (fd2, F_GETFL)");
		return 1;
	}
	if (flags1 != flags2) {
		fprintf (stderr, "bad flags\n");
		return 1;
	}
	printf ("good flags\n");

	close (fd2);
	close (fd1);

	return 0;
}
