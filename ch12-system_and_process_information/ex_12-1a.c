/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program that lists the process ID and command name for all processes
 * being run by the user named in the program's command-line argument. This can
 * be done by inspecting the 'Name:' and 'Uid:' lines of all of the /proc/PID/status files on
 * the system. Walking through all of the /proc/PID directories on the system requires the
 * use of readdir(3). Make sure your program correctly handles the possibility that a
 * /proc/PID directory disappears between the time that the program determines that the
 * directory exists and the time that it tries to open the corresponding /proc/PID/status file.
 */

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

static unsigned procDirCnt_G = 0;
static char **procDirs_G = NULL;

static void get_proc_dirs (void);

int
main (int argc, char *argv[])
{
	unsigned i;

	get_proc_dirs ();
	for (i=0; i<procDirCnt_G; ++i)
		printf ("%s\n", procDirs_G[i]);

	for (i=0; i<procDirCnt_G; ++i)
		free (procDirs_G[i]);
	free (procDirs_G);

	return 0;
}

static void
get_proc_dirs (void)
{
	size_t namelen, i;
	DIR *procDir;
	struct dirent *dirent_p;

	procDir = opendir ("/proc");
	if (procDir == NULL) {
		perror ("opendir (/proc)");
		exit (1);
	}

	while ((dirent_p = readdir (procDir)) != NULL) {
		if (dirent_p->d_type != DT_DIR) {
			printf ("skipping /proc/%s: not a directory\n", dirent_p->d_name);
			continue;
		}
		namelen = strlen (dirent_p->d_name);
		for (i=0; i<namelen; ++i)
			if ((dirent_p->d_name[i] < '0') || (dirent_p->d_name[i] > '9'))
				break;
		if (i < namelen) {
			printf ("skipping /proc/%s: not a process entry\n", dirent_p->d_name);
			continue;
		}

		printf ("adding /proc/%s to list\n", dirent_p->d_name);
		++procDirCnt_G;
		procDirs_G = realloc (procDirs_G, procDirCnt_G * sizeof (char*));
		if (procDirs_G == NULL) {
			perror ("realloc()");
			exit (1);
		}
		procDirs_G[procDirCnt_G - 1] = strdup (dirent_p->d_name);
		if (procDirs_G[procDirCnt_G - 1] == NULL) {
			perror ("strdup()");
			exit (1);
		}
	}

	closedir (procDir);
}
