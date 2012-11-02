/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * When we run the following program, we find it produces no output.
 * Why is this?
 */

#include <stdio.h>
#include <unistd.h>

int
main (void)
{
	printf ("Hello, world!");
	execlp ("sleep", "sleep", "0", NULL);
	return 1;
}
