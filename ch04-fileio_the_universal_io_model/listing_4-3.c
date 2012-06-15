/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of lseek() in conjunction with read() and write().
 * The first command-line argument to this program is the name of the file to be
 * opened. The remaining arguments specify I/O operations to be performed on the
 * file. Each of these operations consists of a letter followed by an associated value
 * (with no separating space):
 *
 *	soffset: seek to byte 'offset' from the start of the file
 *	rlength: read 'length' bytes from the file, starting at the current file offset, and display in text form
 *	Rlength: read 'length' bytes from the file, starting at the current file offset, and display in hex
 *	wstr:    write the string of characters specified in 'str' at the current file offset
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	size_t len;
	off_t offset;
	int fd, ap, j;
	char *buf_p;
	ssize_t numRead, numWritten;

	if (argc < 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <file> {r<length>|R<length>|w<string>|s<offset>}...\n", argv[0]);
		return 1;
	}

	fd = open (argv[1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	for (ap=2; ap<argc; ++ap) {
		switch (argv[ap][0]) {
			case 'r':
			case 'R':
				len = atoi (&argv[ap][1]);
				buf_p = malloc (len);
				if (buf_p == NULL) {
					perror ("malloc()");
					return 1;
				}

				numRead = read (fd, buf_p, len);
				if (numRead == -1) {
					perror ("read()");
					return 1;
				}

				if (numRead == 0)
					printf ("%s: EOF\n", argv[ap]);
				else {
					printf ("%s: ", argv[ap]);
					for (j=0; j<numRead; ++j) {
						if (argv[ap][0] == 'r')
							printf ("%c", isprint ((unsigned char)buf_p[j])? buf_p[j] : '?');
						else
							printf ("%02x ", (unsigned int)buf_p[j]);
					}
					printf ("\n");
				}

				free (buf_p);
				break;

			case 'w':
				numWritten = write (fd, &argv[ap][1], strlen (&argv[ap][1]));
				if (numWritten == -1) {
					perror ("write()");
					return 1;
				}
				printf ("%s: wrote %ld bytes\n", argv[ap], (long)numWritten);
				break;

			case 's':
				offset = atoi (&argv[ap][1]);
				if (lseek (fd, offset, SEEK_SET) == -1) {
					perror ("lseek()");
					return 1;
				}
				printf ("%s: seek succeeded\n", argv[ap]);
				break;

			default:
				printf ("argument must start with [rRws]: %s\n", argv[ap]);
				return 1;
		}
	}

	return 0;
}
