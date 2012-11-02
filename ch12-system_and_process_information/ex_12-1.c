/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program that lists the process ID and command name for all processes
 * being run by the user named in the program's command-line argument. This can
 * be done by inspecting the 'Name:' and 'Uid:' lines of all of the /proc/PID/status files on
 * the system. Walking through all of the /proc/PID directories on the system requires the
 * use of readdir(3). Make sure your program correctly handles the possibility that a
 * /proc/PID directory disappears between the time that the program determines that the
 * directory exists and the time that it tries to open the corresponding /proc/PID/status file.
 */

#define _BSD_SOURCE
#define LINE_SZ 128

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <limits.h>

#include "ch12_util.h"

int
main (int argc, char *argv[])
{
	unsigned i, cnt, info;
	char **procDirs_pp;
	struct passwd *userData_p;
	char fname[PATH_MAX];
	FILE *statusFile_p;
	char lineStr[LINE_SZ];
	char nameStr[LINE_SZ];
	char uidStr[LINE_SZ];
	long uid;

	// check usage
	if (argc != 2) {
		printf ("usage: %s <username>\n", argv[0]);
		return 1;
	}

	// get pw info to convert username -> uid
	if ((userData_p = getpwnam (argv[1])) == NULL) {
		printf ("can't find user: %s\n", argv[1]);
		return 1;
	}

	// get /proc directory list and process
	if (!get_proc_dirs (&cnt, &procDirs_pp)) {
		printf ("can't get /proc directory list\n");
		return 1;
	}

	printf ("user '%s' is running the following programs:\n", argv[1]);
	printf ("\tpid\tcmd\n");
	i = cnt;
	while (i > 0) {
		--i;
		if (snprintf (fname, PATH_MAX, "/proc/%s/status", procDirs_pp[i]) >= PATH_MAX) {
			printf ("couldn't create /proc fname with %s\n", procDirs_pp[i]);
			continue;
		}
		statusFile_p = fopen (fname, "r");
		if (statusFile_p == NULL) {
			printf ("can't read %s\n", fname);
			continue;
		}
		info = 0;
		while (fgets (lineStr, LINE_SZ, statusFile_p)) {
			if (strncmp (lineStr, "Name:", 5) == 0) {
				++info;
				memcpy (nameStr, lineStr, LINE_SZ);
			}
			if (strncmp (lineStr, "Uid:", 4) == 0) {
				++info;
				memcpy (uidStr, lineStr, LINE_SZ);
			}
			if (info == 2)
				break;
		}
		fclose (statusFile_p);

		if (info == 2) {
			// check the real uid
			if (sscanf (uidStr, "Uid: %ld", &uid) != 1) {
				printf ("can't scan uid from '%s'\n", uidStr);
				fclose (statusFile_p);
				continue;
			}
			if (uid == userData_p->pw_uid)
				printf ("\t%s\t%s", procDirs_pp[i], &nameStr[6]);
		}
	}
	free_proc_dirs (cnt, &procDirs_pp);

	return 0;
}
