/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * The access(2) system call checks permissions using the process's real user and group
 * IDs. Write a corresponding function that performs the checks according to the
 * process's effective user and group IDs.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

static int eaccess (const char *pathname_p, int mode);
static void usage (const char *pgm_p);

int
main (int argc, char *argv[])
{
	int mode, ret;

	if (argc != 3) {
		usage (argv[0]);
		return 1;
	}

	mode = 0;
	if (strchr (argv[2], 'o') != NULL)
		mode = F_OK;
	if (strchr (argv[2], 'r') != NULL)
		mode |= R_OK;
	if (strchr (argv[2], 'w') != NULL)
		mode |= W_OK;
	if (strchr (argv[2], 'x') != NULL)
		mode |= X_OK;

	ret = eaccess (argv[1], mode);
	if (ret == -1) {
		perror ("");
		return -1;
	}

	printf ("%s is accessible for %08x\n", argv[1], mode);
	return 0;
}

static void
usage (const char *pgm_p)
{
	printf ("usage: %s <file> <perms>\n", pgm_p);
	printf ("  where:\n");
	printf ("    <file>   the filename to check\n");
	printf ("    <o|rwx>  the permissions to check, consisting of any of the following letters:\n");
	printf ("             o  the file exists\n");
	printf ("             r  the file exists and grants read\n");
	printf ("             w  the file exists and grants write\n");
	printf ("             x  the file exists and grants execute\n");
}

static int
eaccess (const char *pathname_p, int mode)
{
	struct stat statInfo;
	int ret;

	memset (&statInfo, 0, sizeof (statInfo));
	ret = stat (pathname_p, &statInfo);
	if (ret == -1)
		return -1;

	if (mode & F_OK)
		return 0;
	if (mode & R_OK) {
		if (!(statInfo.st_mode & S_IRUSR)) {
			errno = EACCES;
			return -1;
		}
	}
	if (mode & W_OK) {
		if (!(statInfo.st_mode & S_IWUSR)) {
			errno = EACCES;
			return -1;
		}
	}
	if (mode & X_OK) {
		if (!(statInfo.st_mode & S_IXUSR)) {
			errno = EACCES;
			return -1;
		}
	}

	return 0;
}
