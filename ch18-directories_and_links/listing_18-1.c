/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program accepts 2 cmdline args: the first gives the name of the
 * filename to create, and the second optionally specifies the number of 1kB
 * blocks to write (100,000 otherwise). The program opens the specified file
 * and then immediately unlinks the filename. Although the filename disappears
 * the file itself continues to exist. The program then writes random blocks of
 * data to the file. At this point the program employs the df(1) command to
 * display the amount of space used on the file system. The program then closes
 * the file descriptor (at which point the file is removed) and uses df(1)
 * again to show the change in the amount of used disk space.
 *
 *	$ ./listing_18-1 tfile 1000000
 *	Filesystem            1K-blocks        Used   Available  Use%  Mounted on
 *      /dev/sda10              5245020     3204044     2040976   62%  /
 *      ************ fd closed
 *	Filesystem            1K-blocks        Used   Available  Use%  Mounted on
 *      /dev/sda10              5245020     2201128     3043892   42%  /
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int
main (int argc, char *argv[])
{
	int fd, i, numBlocks;
	char shellCmd[200];
	char buf[1024];

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <file> [<count of 1kB blocks>]\n", argv[0]);
		return 1;
	}
	numBlocks = (argc > 2)? atoi (argv[2]) : 100000;

	fd = open (argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	if (unlink (argv[1]) == -1) {
		perror ("unlink()");
		return 1;
	}

	for (i=0; i<numBlocks; ++i)
		if (write (fd, buf, sizeof (buf)) != sizeof (buf)) {
			perror ("write()");
			return 1;
		}

	snprintf (shellCmd, sizeof (shellCmd), "df -k `dirname %s`", argv[1]);
	system (shellCmd);

	if (close (fd) == -1) {
		perror ("close()");
		return 1;
	}

	printf ("************ fd closed\n");

	system (shellCmd);
	return 0;
}
