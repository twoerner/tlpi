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

static void get_proc_dirs (unsigned *cnt_p, char ***procDirs_ppp);

int
main (int argc, char *argv[])
{
	unsigned i, cnt = 0;
	char **procDirs_pp = NULL;

	get_proc_dirs (&cnt, &procDirs_pp);
	for (i=0; i<cnt; ++i)
		printf ("%s\n", procDirs_pp[i]);

	for (i=0; i<cnt; ++i)
		free (procDirs_pp[i]);
	free (procDirs_pp);

	return 0;
}

static void
get_proc_dirs (unsigned *cnt_p, char ***procDirs_ppp)
{
	size_t namelen, i;
	DIR *procDir;
	struct dirent *dirent_p;

	*cnt_p = 0;
	*procDirs_ppp = NULL;

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
		++(*cnt_p);
		*procDirs_ppp = realloc (*procDirs_ppp, (*cnt_p) * sizeof (char*));
		if (*procDirs_ppp == NULL) {
			perror ("realloc()");
			exit (1);
		}
		(*procDirs_ppp)[(*cnt_p) - 1] = strdup (dirent_p->d_name);
		if ((*procDirs_ppp)[(*cnt_p) - 1] == NULL) {
			perror ("strdup()");
			exit (1);
		}
	}

	closedir (procDir);
}
