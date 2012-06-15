/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program that opens an existing file for writing with the O_APPEND flag, and
 * then seeks to the beginning of the file before writing some data. Where does the
 * data appear in the file? Why?
 */

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	int fd;
	off_t offset;

	if (argc != 2) {
		fprintf (stderr, "usage: %s <existing file>\n", argv[0]);
		return 1;
	}

	fd = open (argv[1], O_RDWR | O_APPEND);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	offset = lseek (fd, 0, SEEK_SET);
	if (offset == -1) {
		perror ("lseek()");
		return 1;
	}
	printf ("offset: %jd\n", offset);

	write (fd, "this is some text", 17);
	close (fd);

	return 0;
}
