/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Is it possible to lseek() through a file backwards?
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

//#define BLKSZ 4096
#define BLKSZ 32

static void
usage (char *pgm_p)
{
	printf ("usage: %s <file>\n", pgm_p);
}

int
main (int argc, char *argv[])
{
	int c;
	struct option longOpts[] = {
		{"verbose", no_argument, NULL, 'v'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0},
	};
	int fd;
	ssize_t thisReadCnt;
	char buf[BLKSZ];
	char outbuf[BLKSZ + 1];
	off_t fileSz, tmpoff;
	size_t bytesLeft;
	bool verbose = false;

	// process cmdline args
	while (1) {
		c = getopt_long (argc, argv, "vh", longOpts, NULL);
		if (c == -1)
			break;

		switch (c) {
			case 'h':
				usage (argv[0]);
				return 0;

			case 'v':
				verbose = true;
				break;

			default:
				return 1;
		}
	}
	if (argc != (optind + 1)) {
		printf ("incorrect number of cmdline args\n");
		usage (argv[0]);
		return 1;
	}
	fd = open (argv[optind], O_RDONLY);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	// find the file size
	fileSz = lseek (fd, 0, SEEK_END);
	if (fileSz == (off_t)-1) {
		perror ("lseek (fileSz)");
		return 1;
	}
	if (verbose)
		printf ("file size: %ld\n", (long)fileSz);

	bytesLeft = (size_t)fileSz;
	while (bytesLeft > 0) {
		if (verbose)
			printf ("\n--------------------------------------------------\nbytes left: %zu\n", bytesLeft);

		if (bytesLeft > BLKSZ) {
			tmpoff = lseek (fd, -BLKSZ, SEEK_CUR);
			if (verbose)
				printf (">BLKSZ off:%ld\n", (long)tmpoff);
			if (tmpoff == (off_t)-1) {
				perror ("lseek (-BLKSZ)");
				return 1;
			}
			thisReadCnt = read (fd, buf, BLKSZ);
		}
		else {
			tmpoff = lseek (fd, -bytesLeft, SEEK_CUR);
			if (verbose)
				printf ("<=BLKSZ off:%ld\n", (long)tmpoff);
			if (tmpoff == (off_t)-1) {
				perror ("lseek(-bytesLeft)");
				return 1;
			}
			thisReadCnt = read (fd, buf, bytesLeft);
		}
		if (thisReadCnt < 0) {
			perror ("read (1)");
			return 1;
		}
		if (thisReadCnt == 0)
			break;
		if (verbose)
			printf ("thisReadCnt: %ld\n", (long)thisReadCnt);

		memcpy (outbuf, buf, thisReadCnt);
		outbuf[thisReadCnt] = 0;
		if (verbose)
			printf ("outbuf: '%s'\n", outbuf);
		else
			printf ("%s", outbuf);

		if ((size_t)thisReadCnt > bytesLeft)
			bytesLeft = 0;
		else
			bytesLeft -= (size_t)thisReadCnt;

		lseek (fd, tmpoff, SEEK_SET);
	}

	// cleanup
	close (fd);
	return 0;
}
