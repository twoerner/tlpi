/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Implement a function that performs the equivalent of getcwd().
 *
 * A useful tip for solving this problem is that you can find the name of the
 * current working directory by using opendir() and readdir() to walk through
 * each of the entries in the parent directory (..) to find an entry with the
 * same i-node and device number as the current working directory (i.e.
 * respectively the st_ino and st_dev fields in the 'stat' structure returned
 * by stat() and lstat()). Thus it is possible to construct  the directory
 * path by walking up the directory tree (chdir ("..");) one step at a time
 * and performing such scans. The walk can be finished when the parent
 * directory is the same as the current working directory (recall that '/..'
 * is the same as '/'). The caller should be left in the same directory in
 * which it started, regardless of whether your getcwd() function succeeds
 * or fails (open() plus fchdir() are handy for this purpose).
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

static char *my_getcwd (char *buf_p, size_t sz);

int
main (void)
{
	char buf[PATH_MAX];
	char *rtnPtr;

	memset (buf, 0, sizeof (buf));
	rtnPtr = getcwd (buf, sizeof (buf));
	if (rtnPtr == NULL)
		return 1;
	printf ("   getcwd: '%s'\n", buf);

	memset (buf, 0, sizeof (buf));
	rtnPtr = my_getcwd (buf, sizeof (buf));
	if (rtnPtr == NULL)
		return 1;
	printf ("my_getcwd: '%s'\n", buf);

	return 0;
}

static char *
get_this_dir_name (const char *path_p, char *outbuf_p, size_t sz)
{
	int startingFd;
	int ret;
	struct stat thisDirStatBuf, statBuf;
	DIR *prevDir_p;
	struct dirent *dirEntry_p;

	// get starting directory
	startingFd = open (".", O_RDONLY);
	if (startingFd == -1)
		return NULL;

	if (chdir (path_p) != 0)
		return NULL;

	ret = stat (".", &thisDirStatBuf);
	if (ret == -1)
		goto bad;

	if (chdir ("..") != 0)
		goto bad;
	prevDir_p = opendir (".");
	if (prevDir_p == NULL)
		goto bad;

	while ((dirEntry_p = readdir (prevDir_p)) != NULL) {
		ret = stat (dirEntry_p->d_name, &statBuf);
		if (ret == -1)
			continue;

		if ((statBuf.st_ino == thisDirStatBuf.st_ino) && (statBuf.st_dev == thisDirStatBuf.st_dev)) {
			snprintf (outbuf_p, sz, "%s", dirEntry_p->d_name);
			closedir (prevDir_p);
			fchdir (startingFd);
			return outbuf_p;
		}
	}

	closedir (prevDir_p);
bad:
	fchdir (startingFd);
	return NULL;
}

__attribute__((format (printf, 3, 4)))
size_t
my_snprintf (char *buf_p, size_t size, const char *format_p, ...)
{
	va_list args;
	int ret;

	if (size == 0)
		return 0;

	va_start (args, format_p);
	ret = vsnprintf (buf_p, size, format_p, args);
	va_end (args);

	if (ret < 0)
		return 0;
	if ((size_t)ret < size)
		return (size_t)ret;
	return size - 1;
}

static char *
my_getcwd (char *buf_p, size_t sz)
{
	char *ptr;
	char tmp[PATH_MAX];
	char buildPath[PATH_MAX];
	unsigned parts;
	char **partStrings_pp = NULL;
	size_t pos;

	// preconds
	if ((buf_p == NULL) || (sz == 0)) {
		errno = EINVAL;
		return NULL;
	}

	parts = 0;
	partStrings_pp = NULL;

	pos = 0;
	pos += my_snprintf (&buildPath[pos], sizeof (buildPath) - pos, ".");
	while (1) {
		ptr = get_this_dir_name (buildPath, tmp, sizeof (tmp));
		if (ptr == NULL)
			return NULL;
		if (tmp[0] == '.')
			break;

		partStrings_pp = (char**)realloc (partStrings_pp, sizeof (char*) * (parts + 1));
		if (partStrings_pp == NULL)
			return NULL;
		partStrings_pp[parts] = strdup (tmp);
		++parts;

		pos += my_snprintf (&buildPath[pos], sizeof (buildPath) - pos, "/..");
	}

	pos = 0;
	do {
		--parts;
		pos += my_snprintf (&buf_p[pos], sz - pos, "/");
		pos += my_snprintf (&buf_p[pos], sz - pos, "%s", partStrings_pp[parts]);
		free (partStrings_pp[parts]);
	} while (parts > 0);

	free (partStrings_pp);
	return buf_p;
}
