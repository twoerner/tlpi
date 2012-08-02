/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program uses readlink() and realpath() to read the contents of a
 * symbolic link and to resolve the link to an absolute pathname.
 *
 *	$ pwd
 *	/home/mtk
 *	$ touch x
 *	$ ln -s x y
 *	$ ./listing_18-4 y
 *	readlink: y --> x
 *	realpath: y --> /home/mtk/x
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	struct stat statBuf;
	char buf[PATH_MAX];
	ssize_t numBytes;

	if (argc != 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pathname>\n", argv[0]);
		return 1;
	}

	if (lstat (argv[1], &statBuf) == -1) {
		perror ("lstat()");
		return 1;
	}

	if (!S_ISLNK (statBuf.st_mode)) {
		printf ("%s is not a symbolic link\n", argv[1]);
		return 1;
	}

	numBytes = readlink (argv[1], buf, sizeof (buf) - 1);
	if (numBytes == -1) {
		perror ("readlink()");
		return 1;
	}
	buf[numBytes] = 0;
	printf ("readlink: %s --> %s\n", argv[1], buf);

	if (realpath (argv[1], buf) == NULL) {
		perror ("realpath()");
		return 1;
	}
	printf ("realpath: %s --> %s\n", argv[1], buf);

	return 0;
}
