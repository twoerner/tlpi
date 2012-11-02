/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * The "chmod a+rX <file>" command enables read permission for all categories of user,
 * and likewise enables execute permission for all categories of user if <file> is a directory
 * or execute permission is enabled for any of the user categories for <file>, as
 * demonstrated in the following example:
 *
 *	$ ls -ld dir file prog
 *	dr-------- 2 mtk users       48 May  4 12:28  dir
 *	-r-------- 2 mtk users    19794 May  4 12:28  file
 *	-r-x------ 2 mtk users    19336 May  4 12:28  prog
 *	$ chmod a+rX dir file prog
 *	$ls -ld dir file prog
 *	dr-xr-xr-x 2 mtk users       48 May  4 12:28  dir
 *	-r--r--r-- 2 mtk users    19794 May  4 12:28  file
 *	-r-xr-xr-x 2 mtk users    19336 May  4 12:28  prog
 *
 * Write a program that uses stat() and chmod() to perform the equivalent of "chmod a+rX".
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

static void usage (const char *cmd_p);

int
main (int argc, char *argv[])
{
	int ret;
	struct stat statInfo;
	int i;

	if (argc <= 1) {
		usage (argv[0]);
		return 1;
	}

	for (i=1; i<argc; ++i) {
		ret = stat (argv[i], &statInfo);
		if (ret == -1) {
			perror ("");
			continue;
		}

		// regular file
		if (S_ISREG (statInfo.st_mode)) {
			if (statInfo.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
				ret = chmod (argv[i], S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
				if (ret == -1) {
					perror ("chmod(1)");
					continue;
				}
			}
			else {
				ret = chmod (argv[i], S_IRUSR | S_IRGRP | S_IROTH);
				if (ret == -1) {
					perror ("chmod(2)");
					continue;
				}
			}
		}

		// directory
		if (S_ISDIR (statInfo.st_mode)) {
			ret = chmod (argv[i], S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
			if (ret == -1) {
				perror ("chmod(3)");
				continue;
			}
		}
	}

	return 0;
}

static void
usage (const char *cmd_p)
{
	printf ("usage: %s <files...>\n", cmd_p);
	printf ("  perform the same function as \"chmod a+rX <files...>\"\n");
}
