/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Have bash call this doodle as though it were an interpreter.
 */

#include <stdio.h>

int
main (int argc, char *argv[])
{
	int i;

	for (i=0; i<argc; ++i)
		printf ("[%02d] %s\n", i, argv[i]);

	return 0;
}
