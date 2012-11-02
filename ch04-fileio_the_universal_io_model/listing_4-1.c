/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program is a simple version of the cp(1) command. It copies the
 * contents of the existing file named in its first command-line argument
 * to the new file named in its second command-line argument.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int
main (int argc, char *argv[])
{
	int inputFd, outputFd, openFlags;
	mode_t filePerms;
	ssize_t numRead;
	char buf[BUF_SIZE];

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <source file> <new file>\n", argv[0]);
		return 1;
	}

	inputFd = open (argv[1], O_RDONLY);
	if (inputFd == -1) {
		perror ("open()");
		return 1;
	}

	openFlags = O_CREAT | O_WRONLY | O_TRUNC;
	filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	outputFd = open (argv[2], openFlags, filePerms);
	if (outputFd == -1) {
		perror ("open()");
		return 1;
	}

	while ((numRead = read (inputFd, buf, BUF_SIZE)) > 0)
		if (write (outputFd, buf, numRead) != numRead) {
			printf ("fatal: couldn't write whole buffer\n");
			return 1;
		}
	if (numRead == -1) {
		perror ("read()");
		return 1;
	}

	if (close (inputFd) == -1)
		perror ("close (inputFd)");
	if (close (outputFd) == -1)
		perror ("close (outputFd)");

	return 0;
}
