/*
 * Copyright (C) 2013  Trevor Woerner <twoerner@gmail.com>
 */

/*
 * In Section 41.12 it is pointed out that if a symbol is defined in
 * both a relocatable object (executable) and a shared object, then the
 * symbol in the relocatable object is preferred EVEN IF it gets called
 * from within the shared object(!).
 *
 * However, Section 42.1.6 points out that if the shared object is loaded
 * dynamically at run time, then the symbol from the shared object would be
 * called instead of the one from the relocatable object UNLESS the
 * -export-dynamic (or -rdynamic) option is passed to gcc.
 *
 * This doodle asertains the truth behind the second statement.
 */

#include <stdio.h>
#include <dlfcn.h>

void xyz (void);

/* whether you put a static here or not is _NOT_ significant */
//static
void
myfunc (void)
{
	printf ("main myfunc\n");
}

int
main (void)
{
	void *libHandle_p;
	void (*xyz_p)(void);
	const char *err_p;

	libHandle_p = dlopen ("libdoodle_42-4.so", RTLD_LAZY);
	if (libHandle_p == NULL) {
		perror ("dlopen()");
		return 1;
	}
	(void)dlerror ();
	*(void**)(&xyz_p) = dlsym (libHandle_p, "xyz");
	err_p = dlerror ();
	if (err_p != NULL) {
		fprintf (stderr, "dlsym: %s\n", err_p);
		return 1;
	}
	if (xyz_p == NULL) {
		fprintf (stderr, "func pointer is NULL\n");
		return 1;
	}

	printf ("calling library\n");
	(*xyz_p) ();
	printf ("\ncalling myfunc() directly\n");
	myfunc ();

	dlclose (libHandle_p);
	return 0;
}
