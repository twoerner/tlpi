/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program echos its cmdline args, one per line of output, preceeded
 * by a string showing which element of 'argv' is being displayed.
 */

#include <stdio.h>

int
main (int argc, char *argv[])
{
	int j;

	for (j=0; j<argc; ++j)
		printf ("argv[%d] = %s\n", j, argv[j]);

	return 0;
}
