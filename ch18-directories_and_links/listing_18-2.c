/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program uses opendir(), readdir(), and closedir() to list the contents
 * of each of the directories specified on the cmdline (or the cwd if no aregs
 * are specified).
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

static void list_files (const char *dirpath_p);

int
main (int argc, char *argv[])
{
	if (argc > 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<directories>...]\n", argv[0]);
		return 1;
	}

	if (argc == 1)
		list_files (".");
	else
		for (++argv; *argv; ++argv)
			list_files (*argv);

	return 0;
}

static void
list_files (const char *dirpath_p)
{
	DIR *dir_p;
	struct dirent *dirEnt_p;
	bool isCurrent;

	isCurrent = ((strcmp (dirpath_p, ".") == 0)? true : false);

	dir_p = opendir (dirpath_p);
	if (dir_p == NULL) {
		perror ("opendir()");
		return;
	}

	for (;;) {
		errno = 0;
		dirEnt_p = readdir (dir_p);
		if (dirEnt_p == NULL)
			break;

		if (strcmp (dirEnt_p->d_name, ".") == 0 || strcmp (dirEnt_p->d_name, "..") == 0)
			continue;

		if (!isCurrent)
			printf ("%s/", dirpath_p);
		printf ("%s\n", dirEnt_p->d_name);
	}

	if (errno != 0) {
		perror ("readdir()");
		return;
	}

	if (closedir (dir_p) == -1)
		perror ("closedir()");
}
