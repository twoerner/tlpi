/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * The command 'tail [-n <num>] <file>' prints the last <num> (10 by default) lines of the
 * named <file>. Implement this command using I/O system calls (lseek(), read(), write(),
 * and so on). Keep in mind the buffering issues described in this chapter, in order to
 * make the implementation efficient.
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BLKSZ 4096

static void
usage (char *pgm_p)
{
	printf ("usage: %s [-n <num>] <file>\n", pgm_p);
}

int
main (int argc, char *argv[])
{
	int c;
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},
		{NULL, required_argument, NULL, 'n'},
		{NULL, 0, NULL, 0}
	};
	int linesFound, linesToPrint = 10;
	int fd;
	ssize_t thisReadCnt;
	char buf[BLKSZ];
	off_t fileSz, tmpoff, printStartPos;
	size_t bytesLeft;

	// process cmdline args
	while (1) {
		c = getopt_long (argc, argv, "hn:", longOpts, NULL);
		if (c == -1)
			break;

		switch (c) {
			case 'h':
				usage (argv[0]);
				return 0;

			case 'n':
				if (sscanf (optarg, "%i", &c) != 1) {
					printf ("can't convert <num>, using default: %d\n", linesToPrint);
					break;
				}
				if (c <= 0) {
					printf ("<num> must be positive, using default: %d\n", linesToPrint);
					break;
				}
				linesToPrint = c;
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

	// do the 'tail' stuff
	linesFound = 0;
	bytesLeft = (size_t)fileSz;
	printStartPos = fileSz;
	++linesToPrint;

	// figure out the starting point by working backwards in the file
	while (bytesLeft > 0) {
		if (bytesLeft > BLKSZ) {
			tmpoff = lseek (fd, -BLKSZ, SEEK_CUR);
			if (tmpoff == (off_t)-1) {
				perror ("lseek(1)");
				return 1;
			}
			thisReadCnt = read (fd, buf, BLKSZ);
		}
		else {
			tmpoff = lseek (fd, -bytesLeft, SEEK_CUR);
			if (tmpoff == (off_t)-1) {
				perror ("lseek(2)");
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

		if ((size_t)thisReadCnt > bytesLeft)
			bytesLeft = 0;
		else
			bytesLeft -= (size_t)thisReadCnt;

		--thisReadCnt;
		while (thisReadCnt >= 0) {
			if (buf[thisReadCnt] == '\n') {
				++linesFound;
				if (linesFound == linesToPrint)
					break;
			}
			--thisReadCnt;
			--printStartPos;
		}
		if (linesFound == linesToPrint)
			break;

		// undo the effects of the read()
		if (lseek (fd, tmpoff, SEEK_SET) == (off_t)-1) {
			perror ("lseek(3)");
			return 1;
		}
	}

	// write out the last lines
	if (lseek (fd, (int)printStartPos, SEEK_SET) == (off_t)-1) {
		perror ("lseek(4)");
		return 1;
	}
	while ((thisReadCnt = read (fd, buf, BLKSZ)) > 0)
		write (STDOUT_FILENO, buf, (size_t)thisReadCnt);

	// cleanup
	close (fd);
	return 0;
}
