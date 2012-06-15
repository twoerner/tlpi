/*
 * Copyright (C) 2012  Trevor Woerner
 */

#define _BSD_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

bool
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
		return false;
	}

	while ((dirent_p = readdir (procDir)) != NULL) {
		if (dirent_p->d_type != DT_DIR)
			continue;
		namelen = strlen (dirent_p->d_name);
		for (i=0; i<namelen; ++i)
			if ((dirent_p->d_name[i] < '0') || (dirent_p->d_name[i] > '9'))
				break;
		if (i < namelen)
			continue;

		++(*cnt_p);
		*procDirs_ppp = realloc (*procDirs_ppp, (*cnt_p) * sizeof (char*));
		if (*procDirs_ppp == NULL) {
			perror ("realloc()");
			return false;
		}
		(*procDirs_ppp)[(*cnt_p) - 1] = strdup (dirent_p->d_name);
		if ((*procDirs_ppp)[(*cnt_p) - 1] == NULL) {
			perror ("strdup()");
			return false;
		}
	}

	closedir (procDir);
	return true;
}

void
free_proc_dirs (unsigned cnt, char ***procDirs_ppp)
{
	while (cnt > 0)
		free ((*procDirs_ppp)[--cnt]);
	free (*procDirs_ppp);
}
