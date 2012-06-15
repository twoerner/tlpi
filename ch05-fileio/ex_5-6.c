/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * After each of the calls to write() in the following code, explain what the contents of
 * the output file would be, and why:
 *
 *	fd1 = open (file, O_RDWR | 0_CREAT | O_TRUNC, S_ISUSR | S_IWUSR);
 *	fd2 = dup (fd1);
 *	fd3 = open (file, O_RDWR);
 *	write (fd1, "Hello,", 6);
 *	write (fd2, "world", 6);
 *	lseek (fd2, 0, SEEK_SET);
 *	write (fd1, "HELLO,", 6);
 *	write (fd3, "Gidday", 6);
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	int fd1, fd2, fd3;

	if (argc != 2) {
		fprintf (stderr, "usage\n");
		return 1;
	}

	fd1 = open (argv[1], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	if (fd1 == -1) {
		perror ("open (fd1)");
		return 1;
	}
	fd2 = dup (fd1);
	if (fd2 == -1) {
		perror ("dup (fd2)");
		return 1;
	}
	fd3 = open (argv[1], O_RDWR);
	if (fd3 == -1) {
		perror ("open (fd3)");
		return 1;
	}

	write (fd1, "Hello, ", 7);
	write (fd2, "world", 6);

	lseek (fd2, 0, SEEK_SET);

	write (fd1, "HELLO, ", 7);
	write (fd3, "Gidday", 6);

	close (fd1);
	close (fd2);
	close (fd3);

	return 0;
}
