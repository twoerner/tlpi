/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Implement realpath().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

static char *my_realpath (const char *path_p, char *resolvedPath_p);

int
main (int __attribute__((unused)) argc, char *argv[])
{
	char *absPath_p;

	for (++argv; *argv; ++argv) {
		printf ("%s\n", *argv);

		absPath_p = realpath (*argv, NULL);
		if (absPath_p != NULL) {
			printf ("   -> %s\n", absPath_p);
			free (absPath_p);
		}
		else
			printf ("   -> ERROR (%s)\n", strerror (errno));

		absPath_p = my_realpath (*argv, NULL);
		if (absPath_p != NULL) {
			printf ("   -> %s\n", absPath_p);
			free (absPath_p);
		}
		else
			printf ("   -> ERROR (%s)\n", strerror (errno));
	}

	return 0;
}

/*
 * 1) coalesce (into 1) repeated forward slashes
 * 2) remove sequences of './'
 * 3) keep sequences of '../'
 */
static void
cleanup_path (const char *oldPath_p, char *newPath_p)
{
	unsigned slash;

	slash = 0;
	while (*oldPath_p) {
		if (*oldPath_p == '/') {
			++oldPath_p;
			if (slash == 0) {
				*newPath_p++ = '/';
				++slash;
			}
			continue;
		}

		if (memcmp (oldPath_p, "./", 2) == 0) {
			oldPath_p += 2;
			continue;
		}

		// need to keep this block so that sequences of
		// '../' aren't confused with sequences of './'
		if (memcmp (oldPath_p, "../", 3) == 0) {
			oldPath_p += 3;
			*newPath_p++ = '.';
			*newPath_p++ = '.';
			*newPath_p++ = '/';
			++slash;
			continue;
		}

		slash = 0;
		*newPath_p++ = *oldPath_p++;
	}

	*newPath_p = 0;
}

static void
remove_dot_dot_slash_sequences (const char *oldPath_p, char *newPath_p)
{
	unsigned level = 0;

	*newPath_p = 0;
	if (*oldPath_p != '/')
		return;
	*newPath_p++ = '/';
	++oldPath_p;

	while (*oldPath_p) {
		if (memcmp (oldPath_p, "../", 3) == 0) {
			oldPath_p += 3;

			// rewind newPath_p to last '/'
			if (level > 0) {
				newPath_p -= 2;
				while (*newPath_p != '/')
					--newPath_p;
				++newPath_p;
				--level;
			}
			continue;
		}

		if (*oldPath_p == '/')
			++level;

		*newPath_p++ = *oldPath_p++;
	}

	*newPath_p = 0;
}

static char *
replace_symlinks (const char *oldPath_p, char *newPath_p)
{
	int ret;
	struct stat statBuf;
	char tmp[PATH_MAX + 1];
	char *token_p;
	char *oldPathDup_p;

	// check for starting '/'
	if (*oldPath_p != '/') {
		errno = EINVAL;
		return NULL;
	}
	++oldPath_p;
	tmp[0] = '/';
	tmp[1] = 0;
	newPath_p[0] = '/';
	newPath_p[1] = 0;

	oldPathDup_p = strdup (oldPath_p);
	if (oldPathDup_p == NULL)
		return NULL;

	token_p = strtok (oldPathDup_p, "/");
	while (token_p != NULL) {
//printf ("tmp: '%s'\n", tmp);
		strncat (tmp, token_p, PATH_MAX - strlen (tmp));

		ret = lstat (tmp, &statBuf);
		if (ret == -1) {
			free (oldPathDup_p);
			return NULL;
		}
		if (S_ISLNK (statBuf.st_mode)) {
			char tmpbuf[PATH_MAX];

			memset (tmpbuf, 0, PATH_MAX);
			readlink (tmp, tmpbuf, PATH_MAX);
//printf ("readlink (%s) -> %s\n", tmp, tmpbuf);

			if (tmpbuf[0] == '/') {
				newPath_p[0] = 0;
				strncat (newPath_p, tmpbuf, PATH_MAX - 1);
			}
			else
				strncat (newPath_p, tmpbuf, PATH_MAX - strlen (newPath_p) - 1);
		}
		else
			strncat (newPath_p, token_p, PATH_MAX - strlen (newPath_p) - 1);
		strncat (newPath_p, "/", PATH_MAX - strlen (newPath_p) - 1);

		strncat (tmp, "/", PATH_MAX - strlen (tmp));

		token_p = strtok (NULL, "/");
	}

	free (oldPathDup_p);
	if (newPath_p[strlen (newPath_p) - 1] == '/')
		newPath_p[strlen (newPath_p) - 1] = 0;
	return newPath_p;
}

static char *
my_realpath (const char *path_p, char *resolvedPath_p)
{
	int ret;
	struct stat statBuf;
	char *buf_p;
	char cwd[PATH_MAX];
	char *cleanPath_p;
	char *absCleanPath_p;
	char *absCleanPathNoRel_p;
	char *absCleanPathNoRelNoSym_p;

	ret = stat (path_p, &statBuf);
	if (ret == -1)
		return NULL;

	// it is the responsibility of the caller to free buf_p
	if (resolvedPath_p == NULL) {
		buf_p = malloc (PATH_MAX);
		if (buf_p == NULL)
			return NULL;
	}
	else
		buf_p = resolvedPath_p;
	*buf_p = 0;

	// cleanup path_p
	cleanPath_p = (char*)malloc (strlen (path_p) + 1);
	if (cleanPath_p == NULL)
		return NULL;
	cleanup_path (path_p, cleanPath_p);

	// if path is relative we have to start with the CWD
	// and build up the absolute path
	if (*cleanPath_p != '/') {
		if (getcwd (cwd, PATH_MAX) == NULL) {
			free (cleanPath_p);
			return NULL;
		}

		absCleanPath_p = (char*)malloc (strlen (cwd) + strlen (cleanPath_p) + 2);
		if (absCleanPath_p == NULL) {
			free (cleanPath_p);
			return NULL;
		}
		strcpy (absCleanPath_p, cwd);
		strcat (absCleanPath_p, "/");
		strcat (absCleanPath_p, cleanPath_p);
	}
	else {
		absCleanPath_p = (char*)malloc (strlen (cleanPath_p) + 1);
		if (absCleanPath_p == NULL) {
			free (cleanPath_p);
			return NULL;
		}
		strcpy (absCleanPath_p, cleanPath_p);
	}
	free (cleanPath_p);

	// remove any '../' sequences
	absCleanPathNoRel_p = (char*)malloc (strlen (absCleanPath_p) + 1);
	if (absCleanPathNoRel_p == NULL) {
		free (absCleanPath_p);
		return NULL;
	}
	remove_dot_dot_slash_sequences (absCleanPath_p, absCleanPathNoRel_p);
	free (absCleanPath_p);

	// now that we have an absolute path we have to walk it
	// looking for and replacing any symlinks
	absCleanPathNoRelNoSym_p = (char*)malloc (PATH_MAX);
	if (absCleanPathNoRelNoSym_p == NULL) {
		free (absCleanPathNoRel_p);
		return NULL;
	}
	memset (absCleanPathNoRelNoSym_p, 0, PATH_MAX);
	if (replace_symlinks (absCleanPathNoRel_p, absCleanPathNoRelNoSym_p) == NULL) {
		free (absCleanPathNoRelNoSym_p);
		free (absCleanPathNoRel_p);
		return NULL;
	}
	free (absCleanPathNoRel_p);

	strcpy (buf_p, absCleanPathNoRelNoSym_p);
	free (absCleanPathNoRelNoSym_p);
	return buf_p;
}
