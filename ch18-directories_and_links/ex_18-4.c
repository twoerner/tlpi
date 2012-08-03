/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Modify the program in listing 18-2 to use readdir_r() instead of readdir().
 */

/*
 * (from listing_18-2)
 *
 * This program uses opendir(), readdir(), and closedir() to list the contents
 * of each of the directories specified on the cmdline (or the cwd if no aregs
 * are specified).
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stddef.h>

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
	int ret;
	DIR *dir_p;
	bool isCurrent;
	struct dirent *entry_p, *result_p;
	size_t len;

	isCurrent = ((strcmp (dirpath_p, ".") == 0)? true : false);

	dir_p = opendir (dirpath_p);
	if (dir_p == NULL) {
		perror ("opendir()");
		return;
	}

	// allocate "entry"
	len = offsetof (struct dirent, d_name) + PATH_MAX + 1;
	entry_p = (struct dirent*)malloc (len);
	if (entry_p == NULL)
		return;

	for (;;) {
		errno = 0;
		ret = readdir_r (dir_p, entry_p, &result_p);
		if ((ret != 0) || (result_p == NULL))
			break;

		if (strcmp (entry_p->d_name, ".") == 0 || strcmp (entry_p->d_name, "..") == 0)
			continue;

		if (!isCurrent)
			printf ("%s/", dirpath_p);
		printf ("%s\n", entry_p->d_name);
	}

	free (entry_p);

	if (errno != 0)
		perror ("readdir()");

	if (closedir (dir_p) == -1)
		perror ("closedir()");
}
