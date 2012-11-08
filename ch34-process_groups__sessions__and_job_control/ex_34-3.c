/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program to verify that a call to setsid() from a process group
 * leader fails.
 */

#include <stdio.h>
#include <unistd.h>

int
main (void)
{
	int ret;

	ret = setsid ();
	if (ret == -1) {
		perror ("setsid()");
		return 1;
	}

	return 0;
}
