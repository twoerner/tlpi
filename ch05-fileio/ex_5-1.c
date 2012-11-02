/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Modify the program in listing 5-3 to use the standard I/O system calls (open() and
 * lseek()) and the 'off_t' data type. Compile the program with the _FILE_OFFSET_BITS
 * macro set to 64, and test to show that a large file can be successfully created.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	int fd;
	off_t off;

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pathname> <offset>\n", argv[0]);
		return 1;
	}

	fd = open (argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	off = atoll (argv[2]);
	if (lseek (fd, off, SEEK_SET) == -1) {
		perror ("lseek()");
		return 1;
	}

	if (write (fd, "test", 4) == -1) {
		perror ("write()");
		return 1;
	}

	return EXIT_SUCCESS;
}
