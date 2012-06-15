/*
 * Copyright (C) 2012  Trevor Woerner
 */

#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	int fd;
	off64_t off;

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pathname> <offset>\n", argv[0]);
		return 1;
	}

	fd = open64 (argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror ("open64()");
		return 1;
	}

	off = atoll (argv[2]);
	if (lseek64 (fd, off, SEEK_SET) == -1) {
		perror ("lseek64()");
		return 1;
	}

	if (write (fd, "test", 4) == -1) {
		perror ("write()");
		return 1;
	}

	return EXIT_SUCCESS;
}
