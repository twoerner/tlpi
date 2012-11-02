/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program that lists all process that have a particular file pathname open.
 * This can be achieved by inspecting the contents of all of the /proc/PID/fd/<asterisk> symbolic
 * links. This will require nested loops employing readdir(3) to scan all /proc/PID
 * directories, and then the contents of all /proc/PID/fd entries within each /proc/PID
 * directory. To read the contents of the /proc/PID/fd/n symbolic link requires the use
 * of readlink().
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>

#include "ch12_util.h"

int
main (int argc, char *argv[])
{
	unsigned procCnt, i;
	char **procDirs_pp;
	ssize_t ret;
	DIR *fdDir_p = NULL;
	struct dirent *thisDir_p;
	char dirName[PATH_MAX];
	char fullPath[PATH_MAX];
	char linkPath[PATH_MAX];
	bool haveProcName;
	char statusStr[256];
	char lineStr[256];
	FILE *statusFile_p;

	if (argc != 2) {
		printf ("usage: %s <filename>\n", argv[0]);
		return 1;
	}

	if (!get_proc_dirs (&procCnt, &procDirs_pp)) {
		printf ("can't get /proc directory list\n");
		return 1;
	}

	printf ("the (list of) process(es) which has/have the file '%s' opened is/are:\n", argv[1]);

	// process list
	for (i=0; i<procCnt; ++i) {
		if (fdDir_p != NULL) {
			closedir (fdDir_p);
			fdDir_p = NULL;
		}

		haveProcName = false;
		if (snprintf (statusStr, 256, "/proc/%s/status", procDirs_pp[i]) < 256) {
			statusFile_p = fopen (statusStr, "r");
			if (statusFile_p != NULL) {
				while (fgets (lineStr, 256, statusFile_p) != NULL) {
					if (strncmp ("Name:", lineStr, 5) == 0) {
						haveProcName = true;
						break;
					}
				}
				fclose (statusFile_p);
			}
		}

		// for each /proc/PID get the list of fd/*
		if (snprintf (dirName, PATH_MAX, "/proc/%s/fd", procDirs_pp[i]) >= PATH_MAX) {
			printf ("not enough room in dirName for '%s'\n", procDirs_pp[i]);
			return 1;
		}
		fdDir_p = opendir (dirName);
		if (fdDir_p == NULL)
			continue;

		while ((thisDir_p = readdir (fdDir_p)) != NULL) {
			if (thisDir_p->d_type == DT_LNK) {
				if (snprintf (fullPath, PATH_MAX, "/proc/%s/fd/%s", procDirs_pp[i], thisDir_p->d_name) >= PATH_MAX)
					continue;
				ret = readlink (fullPath, linkPath, PATH_MAX);
				if (ret == -1)
					continue;
				if (strncmp (linkPath, argv[1], strlen (argv[1])) == 0) {
					printf ("  %s", procDirs_pp[i]);
					if (haveProcName)
						printf ("%s", &lineStr[5]);
					else
						printf ("\n");
					break;
				}
			}
		}
	}

	if (fdDir_p != NULL)
		closedir (fdDir_p);

	// done
	free_proc_dirs (procCnt, &procDirs_pp);
	return 0;
}
