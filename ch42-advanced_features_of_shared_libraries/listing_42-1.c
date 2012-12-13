/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the use of the dlopen API. It takes two
 * cmdline args: the name of a shared library to load, and the name
 * of a function to execute with that library
 */

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

int
main (int argc, char *argv[])
{
	void *libHandle_p;
	void (*func_p)(void);
	const char *err_p;

	if (argc != 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage, %s <library> <function>\n", argv[0]);
		return 1;
	}

	// load the shared library and get a handle for future use
	libHandle_p = dlopen (argv[1], RTLD_LAZY);
	if (libHandle_p == NULL) {
		fprintf (stderr, "dlopen: %s\n", dlerror());
		return 1;
	}

	// search library for symbol named in argv[2]
	(void)dlerror ();
	*(void **)(&func_p) = dlsym (libHandle_p, argv[2]);
	err_p = dlerror ();
	if (err_p != NULL) {
		fprintf (stderr, "dlsym: %s\n", err_p);
		return 1;
	}

	// try calling function
	if (func_p == NULL)
		printf ("%s is NULL\n", argv[2]);
	else
		(*func_p) ();

	dlclose (libHandle_p);
	return 0;
}
