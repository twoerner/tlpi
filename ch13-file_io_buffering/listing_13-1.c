/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program provides a simple example of the use of O_DIRECT while opening a file
 * for reading. This program takes up to 4 cmdline args specifying:
 *	1) the file to be read
 *	2) the number of bytes to read from the file
 *	3) the offset to which the program should seek() before reading from the file
 *	   [optional, defaults to 0]
 *	4) the alignment of the data buffer passed to read()
 *	   [optional, defaults to 4096]
 *
 * examples:
 *	read 512 bytes at offset 0
 *		$ ./listing_13-1 <file> 512
 *		read 512 bytes
 *
 *	length is not a multiple of 512
 *		$ ./listing_13-1 <file> 256
 *		read(): Invalid argument
 *
 *	offset is not a multiple of 512
 *		$ ./listing_13-1 <file> 512 1
 *		read(): Invalid argument
 *
 *	succeeds
 *		$ ./listing_13-1 <file> 4096 8192 512
 *		read 4096 bytes
 *
 *	alignment is not a multiple of 512
 *		$ ./listing_13-1 <file> 4096 512 256
 *		read(): Invalid argument
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>

int
main (int argc, char *argv[])
{
	int fd;
	ssize_t numRead;
	size_t length, alignment;
	off_t offset;
	char *buf_p;

	if (argc < 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <file> <bytes to read> [<offset> [<alignment]]\n", argv[0]);
		return 1;
	}

	if (sscanf (argv[2], "%zu", &length) != 1) {
		perror ("sscanf (length)");
		return 1;
	}
	if (argc > 3) {
		if (sscanf (argv[3], "%ji", &offset) != 1) {
			perror ("sscanf (offset)");
			return 1;
		}
	}
	else
		offset = 0;
	if (argc > 4) {
		if (sscanf (argv[4], "%zu", &alignment) != 1) {
			perror ("sscanf (alignment)");
			return 1;
		}
	}
	else
		alignment = 4096;

	fd = open (argv[1], O_RDONLY | O_DIRECT);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	/*
	 * memalign() allocates a block of memory aligned on an address that
	 * is a multiple of its first argument. By specifying this argument as
	 * 2 * 'alignment' and then adding 'alignment' to the returned pointer,
	 * we ensure that 'buf_p' is aligned on a non-power-of-2 multiple of
	 * 'alignment'. We do this to ensure that if, for example, we ask
	 * for a 256-byte aligned buffer, we don't accidentally get a
	 * buffer that is also aligned on a 512-byte boundary
	 */

	buf_p = memalign (alignment * 2, length + alignment);
	if (buf_p == NULL) {
		perror ("memalign()");
		return 1;
	}
	buf_p += alignment;

	if (lseek (fd, offset, SEEK_SET) == -1) {
		perror ("lseek()");
		return 1;
	}

	numRead = read (fd, buf_p, length);
	if (numRead == -1) {
		perror ("read()");
		return 1;
	}
	printf ("read %ld bytes\n", (long)numRead);

	return 0;
}
