/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program accesses 'environ' in order to list all the values
 * in the process environment.
 */

#include <stdio.h>

extern char **environ;

int
main (void)
{
	char **pptr;

	for (pptr = environ; *pptr!=NULL; ++pptr)
		puts (*pptr);

	return 0;
}
