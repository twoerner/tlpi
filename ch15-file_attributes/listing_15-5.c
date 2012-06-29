/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of umask() in conjunction
 * with open() and mkdir().
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ch15_util.h"

#define MYFILE "myfile"
#define MYDIR "mydir"
#define FILE_PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)
#define DIR_PERMS (S_IRWXU | S_IRWXG | S_IRWXO)
#define UMASK_SETTING (S_IWGRP | S_IXGRP | S_IWOTH | S_IXOTH)

int
main (void)
{
	int fd;
	struct stat statBuf;
	mode_t u;

	umask (UMASK_SETTING);

	fd = open (MYFILE, O_RDWR | O_CREAT | O_EXCL, FILE_PERMS);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}
	if (mkdir (MYDIR, DIR_PERMS) == -1) {
		perror ("mkdir()");
		return 1;
	}

	u = umask (0);

	if (stat (MYFILE, &statBuf) == -1) {
		perror ("stat()");
		return 1;
	}
	printf ("requested file perms: %s\n", file_perm_str (FILE_PERMS, 0));
	printf ("process mask:         %s\n", file_perm_str (u, 0));
	printf ("actual file perms:    %s\n", file_perm_str (statBuf.st_mode, 0));

	if (stat (MYDIR, &statBuf) == -1) {
		perror ("stat()");
		return 1;
	}
	printf ("requested dir  perms: %s\n", file_perm_str (DIR_PERMS, 0));
	printf ("process mask:         %s\n", file_perm_str (u, 0));
	printf ("actual dir  perms:    %s\n", file_perm_str (statBuf.st_mode, 0));

	if (unlink (MYFILE) == -1)
		perror ("unlink()");
	if (rmdir (MYDIR) == -1)
		perror ("rmdir()");

	return 0;
}
