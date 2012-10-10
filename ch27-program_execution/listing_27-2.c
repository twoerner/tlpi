/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Other half of Listing 27-2.
 */

#include <stdio.h>

extern char **environ;

int
main (int argc, char *argv[])
{
	int i;
	char **ep;

	for (i=0; i<argc; ++i)
		printf ("argv[%d] = %s\n", i, argv[i]);
	for (ep=environ; *ep!=NULL; ++ep)
		printf ("environ: %s\n", *ep);

	return 0;
}
