/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This exercise is designed to demonstrate why the atomicity guaranteed by opening
 * a file with the O_APPEND flag is necessary. Write a program that takes up to three
 * command-line arguments:
 *
 *	$ ./ex_5-3 <filename> <num-bytes> [x]
 *
 * This program should open the specified filename (creating it if necessary) and append
 * <num-bytes> bytes to the file by using write() to write a byte at a time. By default, the
 * program should open the file with the O_APPEND flag, but if a third command-line
 * argument (x) is supplied, then the O_APPEND flag should be omitted, and instead the
 * program should perform an lseek (fd, 0, SEEK_END) call before each write(). Run
 * two instances of this program at the same time without the 'x' argument to write
 * 1 million bytes to the same file:
 *
 *	$ ./ex_5-3 f1 10000000& ./ex_5-3a f1 100000000
 *
 * Repeat the same steps, writing to a different file, but this time specifying the 'x'
 * argument:
 *
 *	$ ./ex_5-3 f2 10000000 x& ./ex_5-3a f2 100000000 x
 *
 * List the sizes of the files f1 and f2 using 'ls -l' and explain the difference.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	int flags = O_RDWR | O_CREAT;
	int fd;
	int i, cnt;

	if ((argc != 3) && (argc != 4)) {
		printf ("usage: %s <file> <int:count> [<use O_APPEND>]\n", argv[0]);
		return 1;
	}

	if (argc == 4) {
		flags |= O_APPEND;
	}

	fd = open (argv[1], flags, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	cnt = atoi (argv[2]);
	for (i=0; i<cnt; ++i)
		write (fd, "a", 1);

	return 0;
}
