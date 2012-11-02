/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Use thread-specific data to write thread-safe versions of dirname()
 * and basename().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <libgen.h>
#include <pthread.h>

// set to 0 (use thread-specific data) or 1 (don't)
#define DONT_USE_TSD 0

// *******
// this is a huge assumption:
//	that all passed-in strings will be less than 1023 in strlen()
#define BUFSZ 1024
// *******

#if (DONT_USE_TSD == 0)
static pthread_once_t dirnameOnce_G = PTHREAD_ONCE_INIT;
static pthread_once_t basenameOnce_G = PTHREAD_ONCE_INIT;
static pthread_key_t dirnameKey_G;
static pthread_key_t basenameKey_G;

static void
cleanup (void *buf_p)
{
	fprintf (stderr, "cleanup() %p\n", buf_p);
	free (buf_p);
}

static void
create_dirname_key (void)
{
	int ret;

	fprintf (stderr, "dirname: create_key()\n");
	ret = pthread_key_create (&dirnameKey_G, cleanup);
	if (ret != 0) {
		perror ("pthread_key_create (dirname)");
		exit (1);
	}
}

static void
create_basename_key (void)
{
	int ret;

	fprintf (stderr, "basename: create_key()\n");
	ret = pthread_key_create (&basenameKey_G, cleanup);
	if (ret != 0) {
		perror ("pthread_key_create (basename)");
		exit (1);
	}
}

static char *
my_dirname (char *str_p)
{
	int ret;
	char *buf_p;
	size_t len;

	len = strlen (str_p);
	if (len > (BUFSZ - 1)) {
		fprintf (stderr, "assumption not valid for '%s'\n", str_p);
		return NULL;
	}

	ret = pthread_once (&dirnameOnce_G, create_dirname_key);
	if (ret != 0) {
		perror ("pthread_once(dirname)");
		exit (1);
	}

	buf_p = pthread_getspecific (dirnameKey_G);
	if (buf_p == NULL) {
		buf_p = malloc (BUFSZ);
		if (buf_p == NULL) {
			perror ("malloc()");
			exit (1);
		}

		fprintf (stderr, "creating buf_p %p\n", buf_p);
		ret = pthread_setspecific (dirnameKey_G, buf_p);
		if (ret != 0) {
			perror ("pthread_setspecific()");
			exit (1);
		}
	}

	memcpy (buf_p, str_p, len);
	buf_p[len] = 0;
	return dirname (buf_p);
}

static char *
my_basename (char *str_p)
{
	int ret;
	char *buf_p;
	size_t len;

	len = strlen (str_p);
	if (len > (BUFSZ - 1)) {
		fprintf (stderr, "assumption not valid for '%s'\n", str_p);
		return NULL;
	}

	ret = pthread_once (&basenameOnce_G, create_basename_key);
	if (ret != 0) {
		perror ("pthread_once(basename)");
		exit (1);
	}

	buf_p = pthread_getspecific (basenameKey_G);
	if (buf_p == NULL) {
		buf_p = malloc (BUFSZ);
		if (buf_p == NULL) {
			perror ("malloc()");
			exit (1);
		}

		fprintf (stderr, "creating buf_p %p\n", buf_p);
		ret = pthread_setspecific (basenameKey_G, buf_p);
		if (ret != 0) {
			perror ("pthread_setspecific()");
			exit (1);
		}
	}

	memcpy (buf_p, str_p, len);
	buf_p[len] = 0;
	return basename (buf_p);
}
#endif

static void *
thread_func (void *arg_p)
{
	unsigned i, tblSz;
	uintptr_t td = (uintptr_t)arg_p;
	char *paths[] = {
		"/home/trevor/devel/code/the_linux_programming_interface__BOOK/ch31-threads__thread_safety_and_per_thread_storage",
		"/etc/sysconfig/network/scripts/",
		"/usr/share/locale/en_CA/LC_MESSAGES/",
		"/etc",
	};

	tblSz = sizeof (paths) / sizeof (paths[0]);

	fprintf (stderr, "[%tu] thread starting\n", td);
	for (i=0; i<tblSz; ++i) {
		fprintf (stderr, "[%u] thread:%tu\n", i, td);
#if (DONT_USE_TSD == 1)
		printf ("[%u] thread:%tu basename of '%s': '%s'\n", i, td, paths[i], basename (paths[i]));
		printf ("[%u] thread:%tu dirname  of '%s': '%s'\n", i, td, paths[i], dirname (paths[i]));
#else
		printf ("[%u] thread:%tu basename of '%s': '%s'\n", i, td, paths[i], my_basename (paths[i]));
		printf ("[%u] thread:%tu dirname  of '%s': '%s'\n", i, td, paths[i], my_dirname (paths[i]));
#endif
	}

	return NULL;
}

int
main (void)
{
	int ret;
	uintptr_t i;
	pthread_t threads[5];

	for (i=0; i<5; ++i) {
		ret = pthread_create (&threads[i], NULL, thread_func, (void*)i);
		if (ret != 0) {
			perror ("pthread_create()");
			return i+1;
		}
	}
	for (i=0; i<5; ++i) {
		ret = pthread_join (threads[i], NULL);
		if (ret != 0) {
			perror ("pthread_join()");
			return i+10;
		}
	}

	return 0;
}
